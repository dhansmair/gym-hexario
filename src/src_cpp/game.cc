#include <iostream>
#include <vector>
#include <unordered_set>

#include "include/game.h"

using namespace std;


Game::Game(int numPlayers, int mapRadius, bool discrete, float velocity) 
: numPlayers(numPlayers), mapRadius(mapRadius), discrete(discrete), board(mapRadius)
{
    for (int i = 0; i < numPlayers; i++) {
        players.push_back(Player(i, velocity));
        resetPlayer(i);
    }
}

Game::~Game() = default;

Board &Game::getBoard()
{
    return board;
}

Player &Game::getPlayer(int playerId)
{
    if (playerId >= 0 && playerId < numPlayers) {
        return players[playerId];
    } else {
        cerr << "error: trying to get a player that does not exist" << endl;
        return players[0];
    }
}

vector<Player> &Game::getPlayers()
{
    return players;
}

void Game::setDead(Player &player)
{
    player.setDead(true);
    board.freeAllByOwner(player);
}

void Game::resetPlayer(int playerId)
{
    Player &player = getPlayer(playerId);
    board.freeAllByOwner(player);
    Tile spawnPoint = board.getRandomTile();
    player.spawn(spawnPoint);
    board.setOwner(hexArea(spawnPoint, 2), player);
}

void Game::approveCaptures(Player &player, unordered_set<Tile> tiles)
{
    for (auto &tile : tiles) {
        // get the previous owner of the tile
        int prevOwnerId = board.getOwner(tile);

        if (prevOwnerId != FREE_TILE) {
            Player &prevOwner = getPlayer(prevOwnerId);
            prevOwner.addCaptures(-1);

            if (prevOwner.getNumCaptures() <= 0) {
                // that player was surrounded completely
                setDead(prevOwner);
                player.addKill();
            }
        }

        board.setOwner(tile, player);
    }

    player.addCaptures(tiles.size());
}

void Game::takeActions(vector<float> actions)
{
    // do all the moves
    for (auto &player : players) {
        if (player.isDead()) continue;

        CartCoord newPos = calcStep(player, actions[player.getId()]);
        player.setPosition(newPos);
    }

    checkBorderCollisions();

    if (numPlayers > 1) {
        checkCollisions();
    }

    checkTileCaptures();
}

CartCoord Game::calcStep(Player &player, float direction)
{
    CartCoord position = player.getPosition();

    if (discrete) {
        int dir = (int) direction;
        Tile pos = hexToTile(cartToHex(position));
        Tile newPos = getNeighbor(pos, dir);
        position = hexToCart(tileToHex(newPos));
    } else {
        // calculate the new player position
        //float alpha = direction * 2. * M_PI;

        // direction vector of length 1
        float x = -sin(direction);
        float y = cos(direction);

        // vector scaled by velocity
        x *= player.getVelocity();
        y *= player.getVelocity();

        // apply the change
        position.x += x;
        position.y += y;
    }

    return position;
}
        
void Game::checkBorderCollisions()
{
    for (auto &player : players) {
        if (player.isDead()) continue;

        // check if the head of this player is on a border tile
        Tile head = player.getHead();

        if (board.getOwner(head) == BORDER_TILE) {
            setDead(player);
        }
    }
}

void Game::checkTileCaptures()
{
    for (auto &player : players) {
        if (player.isDead()) continue;

        Tile head = player.getHead();
        
        if (!board.isOwner(head, player)) {
            player.claim(head);
        } else if (player.hasAnyClaim()) {
            // approve claim
            approveCaptures(player, player.getClaim());
            player.approveClaim();
            approveCaptures(player, board.getInclusions(player));
            checkHasCapturedAll(player);
        } 
    }
}

void Game::checkCollisions()
{
    for (auto &player : players) {
        if (player.isDead()) continue;

        for (auto &other : players) {
            if (other == player || other.isDead()) continue;

            if (player.getHead() == other.getHead()) { // head - head
                int ownerId = board.getOwner(player.getHead());

                if (ownerId == player.getId()) {
                    // other dies
                    player.addKill();
                    setDead(other);
                } else if (ownerId == other.getId()) {
                    // player dies
                    other.addKill();
                    setDead(player);
                } else {
                    // both die
                    setDead(player);
                    setDead(other);
                }

            } else if (other.hasClaimed(player.getHead()) && player.hasClaimed(other.getHead())) {
                // this is a special case I did not handle previously:
                // two players previously were exacly next to each other, and 'flipped' positions,
                // so A is now on prev pos of B and vice versa.
                // => players die if they are not on their homeland

                if (board.getOwner(player.getHead()) != player.getId()) {
                    setDead(player);
                }

                if (board.getOwner(other.getHead()) != other.getId()) {
                    setDead(other);
                }

                if (player.isDead() && !other.isDead()) {
                    other.addKill();
                }

                if (other.isDead() && !player.isDead()) {
                    player.addKill();
                }
                
            } else if (other.hasClaimed(player.getHead())) { // head - tail
                // player kills other
                player.addKill();
                setDead(other);
            }
        }
    }
}

void Game::checkHasCapturedAll(Player &player)
{
    if (player.getNumCaptures() == board.getNumTiles()) {
        player.setWinner(true);
    }
}
