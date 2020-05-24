#ifndef HEXAR_INTERFACE_H
#define HEXAR_INTERFACE_H

#include "game.h"
#include "renderer.h"


class Controller {
private:
    const int mapRadius;
    const int numPlayers;
    const bool localPerspective;
    const bool featureObservations;
    const bool discreteActionSpace;
    const int observationRadius;
    const int pxObservationWidth;
    const int pxObservationHeight;

    Game game;
    ObservationRenderer *observationRenderer;
    DemoRenderer *windowRenderer;

    // the old ones
    //void getPixelObservation(Player &player, float *obsMemAddr, long int size);
    //void getFeatureObservation(Player &player, float *obsMemAddr, long int size);

    // the new ones
    void getLocalPixelObservation(Player &player, float *obsMemAddr, long int size);
    void getGlobalPixelObservation(Player &playerm, float *obsMemAddr, long int size);
    void getLocalFeatureObservation(Player &player, float *obsMemAddr, long int size);
    void getGlobalFeatureObservation(Player &player, float *obsMemAddr, long int size);

    float getFeatureObservationCode(Player &player, Tile tile);

public:
    Controller(int mapRadius,
               int numPlayers,
               bool localPerspective,
               bool featureObservations,
               bool discreteActionSpace,
               int observationRadius,
               int pxObservationWidth,
               int pxObservationHeight,
               float defaultVelocity);

    ~Controller();
    void resetPlayer(int playerId);
    void printInfo();
    void takeActions(std::vector<float> actions);
    bool isDead(int playerId);
    bool isWinner(int playerId);
    int getNumKills(int playerId);
    int getNumCaptures(int playerId);
    int getNumClaims(int playerId);
    std::vector<float> getDistancesToBorder(int playerId);

    /**
     * show
     * returns true if a quit event was fired from the demo window
     */
    bool show();
    void getObservation(int playerId, float *obsMemAddr, long int size);
    long int getObservationSize();

    bool isPerspectiveLocal();
};


# endif