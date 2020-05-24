#ifndef HEXAR_PLAYER_H
#define HEXAR_PLAYER_H

#include <vector>
#include <unordered_set>
#include "definitions.h"

/**
 * Player class represents an agent
 * most properties are public since this is only an
 * information storage without any logic implemented
 * (except the id, which should be readonly)
 */
class Player
{
private:
    const int id;

    // once a player is dead, it cannot do anything 
    // unless it is reset
    bool dead;

    bool winner;

    // stores number of killed enemies
    // gets resetted when player dies.
    int kills;

    // stores number of captured tiles for that player
    // number is not (yet) decremented if another 
    // player steals a tile
    int captures;

    // currently not used
    float velocity;

    CartCoord position;

    Tile head;
    // history of claimed but not yet captured tiles
    //std::unordered_set<Tile> tail;
    std::unordered_set<Tile> claimedArea;

    // keep track of the players territory bounding box
    // while the player wanders around
    // required for efficient inclusion calculation
    TileBoundingBox boundingBox;
    
    // the players path in unit coordinates
    std::vector<CartCoord> path;
public:
    explicit Player(int id, float velocity);

    // getter
    int getId();
    bool isDead();
    bool isWinner();
    int getNumKills();
    int getNumCaptures();
    int getNumClaims();
    float getVelocity();
    Tile getHead();
    CartCoord getPosition();
    TileBoundingBox getBoundingBox();
    std::vector<CartCoord> &getPath();
    std::unordered_set<Tile> &getClaim();

    // setter
    void setDead(bool dead);
    void setWinner(bool winner);
    void addKill();
    void setCaptures(int captures);
    void addCaptures(int captures);
    void setPosition(CartCoord pos);
    void setVelocity(float v);
    
    // other
    void claim(Tile t);
    void approveClaim();
    bool hasAnyClaim();
    bool hasClaimed(Tile t);
    
    //void reset();
    void spawn(Tile spawnPoint);
   
    bool operator==(Player &other);
    bool operator!=(Player &other);
};


#endif //HEXAR_PLAYER_H
