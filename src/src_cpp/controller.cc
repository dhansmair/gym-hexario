#include <iostream>
#include <ctime>
#include <cstdlib>
#include <vector>
#include <algorithm>
#include "include/controller.h"
#include "include/utils.h"

using namespace std;

// INVALID is used for tiles outside the arena and 
// for tiles outside the observation on global partially observable setting
/*
const float FEATURE_OBSERVATION_INVALID          = 0.0;
const float FEATURE_OBSERVATION_FREE             = 1.0;
const float FEATURE_OBSERVATION_OWN_CLAIM        = 2.0;
const float FEATURE_OBSERVATION_OWN_CAPTURE      = 3.0;
const float FEATURE_OBSERVATION_OWN_POSITION     = 4.0;
const float FEATURE_OBSERVATION_ENEMY_CLAIM      = 5.0;
const float FEATURE_OBSERVATION_ENEMY_CAPTURE    = 6.0;
const float FEATURE_OBSERVATION_ENEMY_POSITION   = 7.0;
const float FEATURE_OBSERVATION_BORDER           = 8.0;
*/
const float FEATURE_OBSERVATION_INVALID          = -1.0;
const float FEATURE_OBSERVATION_FREE             = 0.0;
const float FEATURE_OBSERVATION_OWN_CLAIM        = 2.0;
const float FEATURE_OBSERVATION_OWN_CAPTURE      = 3.0;
const float FEATURE_OBSERVATION_OWN_POSITION     = 4.0;
const float FEATURE_OBSERVATION_ENEMY_CLAIM      = 5.0;
const float FEATURE_OBSERVATION_ENEMY_CAPTURE    = 6.0;
const float FEATURE_OBSERVATION_ENEMY_POSITION   = 7.0;
const float FEATURE_OBSERVATION_BORDER           = 8.0;


Controller::Controller(
        int mapRadius,
        int numPlayers,
        bool localPerspective,
        bool featureObservations,
        bool discreteActionSpace,
        int observationRadius,
        int pxObservationWidth,
        int pxObservationHeight,
        float defaultVelocity
        ) : 
    mapRadius(mapRadius), 
    numPlayers(numPlayers),
    localPerspective(localPerspective),
    featureObservations(featureObservations),
    discreteActionSpace(discreteActionSpace),
    observationRadius(observationRadius),
    pxObservationWidth(pxObservationWidth),
    pxObservationHeight(pxObservationHeight),
    game(numPlayers, mapRadius, discreteActionSpace, defaultVelocity)
{
    observationRenderer = new ObservationRenderer(game, pxObservationWidth, pxObservationHeight);
    observationRenderer->setScale(OBS_SCALE);
    windowRenderer = nullptr;
}

Controller::~Controller()
{
    delete observationRenderer;
    delete windowRenderer;
    SDL_Quit();
}

bool Controller::isPerspectiveLocal()
{
    return localPerspective;
}

void Controller::printInfo()
{
    cout << "Game info" << endl;
    cout << "-----------------------" << endl;
    cout << "number of players: " << numPlayers << endl;
    cout << "map radius: " << mapRadius << endl;
    cout << "perspective: " << (localPerspective ? "local" : "global") << endl;
    cout << "observation type: " << (featureObservations ? "features" : "pixel") << endl;
    cout << "action space: " << (discreteActionSpace ? "discrete" : "continuous") << endl;
    cout << "observation radius: " << observationRadius << endl;
    cout << "pixel observation size: " << pxObservationWidth << " x " << pxObservationHeight << " x 3" << endl; 
    cout << "-----------------------" << endl;
}

void Controller::resetPlayer(int playerId)
{
    game.resetPlayer(playerId);
}

void Controller::takeActions(vector<float> actions)
{
    game.takeActions(actions);
}

bool Controller::isDead(int playerId)
{
    return game.getPlayer(playerId).isDead();
}

bool Controller::isWinner(int playerId)
{
    return game.getPlayer(playerId).isWinner();
}

int Controller::getNumKills(int playerId)
{
    return game.getPlayer(playerId).getNumKills();
}

int Controller::getNumCaptures(int playerId)
{
    return game.getPlayer(playerId).getNumCaptures();
}

int Controller::getNumClaims(int playerId)
{
    return game.getPlayer(playerId).getNumClaims();
}

vector<float> Controller::getDistancesToBorder(int playerId)
{
    // for each direction, return the distance in tile coodinates to the next border
    float maxDist = mapRadius * 2;
    vector<float> result{maxDist, maxDist, maxDist, maxDist, maxDist, maxDist};

    // get the current tile position of the player
    CartCoord pos = game.getPlayer(playerId).getPosition();
    Tile tilePos = cartToTile(pos);

    for (int i = 0; i < 6; i++) {
        Tile t = tilePos;

        for (int d = 1; d < maxDist; d++) {
            t = getNeighbor(t, i);
            int ownerId = game.getBoard().getOwner(t);

            if (ownerId == BORDER_TILE) {
                result[i] = d;
                break;
            }
        }
    }

    return result;
}


bool Controller::show()
{
    if (windowRenderer == nullptr) {
        windowRenderer = new DemoRenderer(game, WIN_WIDTH, WIN_HEIGHT);
        windowRenderer->setScale(WIN_SCALE);
    }

    return windowRenderer->render();
}

long int Controller::getObservationSize()
{
    if (featureObservations && localPerspective) {
        return 1 + 3 * observationRadius * (observationRadius - 1);
    } else if (featureObservations) {
        return 1 + 3 * mapRadius * (mapRadius - 1);
    } else {
        return pxObservationWidth * pxObservationHeight * 3;
    }
}

void Controller::getObservation(int playerId, float *obsMemAddr, long int size)
{
    if (playerId < 0 || playerId >= numPlayers) {
        cerr << "error: player with id " << playerId << " does not exist" << endl;
        return;
    }

    Player &player = game.getPlayer(playerId);

    if (player.isDead()) {
        // fill up with empty information 
        for (long int i = 0; i < size; i++) {
            obsMemAddr[i] = 0.0;
        }

        return;
    }

    if (featureObservations && localPerspective) {
        getLocalFeatureObservation(player, obsMemAddr, size);
    } else if (featureObservations && !localPerspective) {
        getGlobalFeatureObservation(player, obsMemAddr, size);
    } else if (!featureObservations && localPerspective) {
        getLocalPixelObservation(player, obsMemAddr, size);
    } else {
        getGlobalPixelObservation(player, obsMemAddr, size);
    }
}

float Controller::getFeatureObservationCode(Player &player, Tile tile)
{
    float result = FEATURE_OBSERVATION_INVALID;
    int ownerId = game.getBoard().getOwner(tile);

    if (!game.getBoard().isValid(tile)) {
        return result;
    } 

    if (ownerId == BORDER_TILE) {
        // border tile
        result = FEATURE_OBSERVATION_BORDER;
    } else if (ownerId == FREE_TILE) {
        // free
        result = FEATURE_OBSERVATION_FREE;
    } else if (ownerId == player.getId()) {
        // owned by player
        result = FEATURE_OBSERVATION_OWN_CAPTURE;
    } else {
        // owned by someone else
        result = FEATURE_OBSERVATION_ENEMY_CAPTURE;
    }

    // check claims, these will override the captures in the observation
    for (auto &p : game.getPlayers()) {
        if (hexToTile(cartToHex(p.getPosition())) == tile) {
            if (p == player) {
                // claimed by player
                result = FEATURE_OBSERVATION_OWN_POSITION;
            } else {
                // Claimed by someone else
                result = FEATURE_OBSERVATION_ENEMY_POSITION;
            }
        } else if (p.hasClaimed(tile)) {
            if (p == player) {
                // claimed by player
                result = FEATURE_OBSERVATION_OWN_CLAIM;
            } else {
                // Claimed by someone else
                result = FEATURE_OBSERVATION_ENEMY_CLAIM;
            }
        }
    }

    return result;
}

void Controller::getLocalPixelObservation(Player &player, float *obsMemAddr, long int size)
{
    float numUnits = (1 + observationRadius * 2) * 1.5;
    float scale = (float) pxObservationHeight / numUnits;

    observationRenderer->setScale(scale);
    observationRenderer->setFocus(player.getPosition());
    observationRenderer->renderForPlayer(player);
    observationRenderer->writeTo(obsMemAddr, size);
}

void Controller::getGlobalPixelObservation(Player &player, float *obsMemAddr, long int size)
{
    float numUnits = (1 + mapRadius * 2) * 1.5;
    float scale = (float) pxObservationHeight / numUnits;

    observationRenderer->setScale(scale);
    observationRenderer->setFocus(CartCoord{0, 0});
    observationRenderer->renderForPlayer(player);
    observationRenderer->showOnly(player.getPosition(), observationRadius);
    observationRenderer->writeTo(obsMemAddr, size);
}

void Controller::getLocalFeatureObservation(Player &player, float *obsMemAddr, long int size)
{
    Tile pos = hexToTile(cartToHex(player.getPosition()));
    vector<Tile> scope = hexArea(pos, observationRadius);
    
    if (scope.size() != (unsigned long int) size) {
        cerr << "provided wrong size of memory to store discrete observation!" << endl;
        cerr << "required is " << scope.size() << " given was " << size << endl;
        return;
    }

    for (ulong i = 0; i < scope.size(); i++) {
        obsMemAddr[i] = getFeatureObservationCode(player, scope[i]);
    }
}

void Controller::getGlobalFeatureObservation(Player &player, float *obsMemAddr, long int size)
{
    Tile pos = hexToTile(cartToHex(player.getPosition()));
    vector<Tile> mapScope = hexArea(CENTER, mapRadius);
    vector<Tile> visibleScope = hexArea(pos, observationRadius);
    
    if (mapScope.size() != (unsigned long int) size) {
        cerr << "provided wrong size of memory to store discrete observation!" << endl;
        cerr << "required is " << mapScope.size() << " given was " << size << endl;
        return;
    }

    for (ulong i = 0; i < mapScope.size(); i++) {
        Tile tile = mapScope[i];

        // check if the tile is also in the visible scope
        if (find(visibleScope.begin(), visibleScope.end(), tile) != visibleScope.end()) {
            obsMemAddr[i] = getFeatureObservationCode(player, tile);
        } else {
            obsMemAddr[i] = FEATURE_OBSERVATION_INVALID;
        }
    }
}