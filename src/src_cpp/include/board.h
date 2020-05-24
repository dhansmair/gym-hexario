#ifndef HEXAR_BOARD_H
#define HEXAR_BOARD_H

#include <unordered_map>
#include <unordered_set>
#include "definitions.h"
#include "player.h"


class Board
{
private:
    const int radius;
    const int numTiles;
    std::unordered_map<Tile, int> tiles;

public:
    explicit Board(int radius);
    void reset();
    int getRadius();
    int getNumTiles();
    int getOwner(Tile t);
    std::unordered_set<Tile> getAll();
    Tile getRandomTile();
    void setFree(Tile tile);
    void setOwner(Tile t, Player &player);
    void setOwner(const std::vector<Tile> &tiles, Player &player);
    void setOwner(const std::unordered_set<Tile> &tiles, Player &player);
    bool isValid(Tile tile);
    bool isOwner(Tile tile, Player &player);
    std::unordered_set<Tile> getAllByOwner(Player &player);
    int freeAllByOwner(Player &player);
    std::unordered_set<Tile> getInclusions(Player &player);
};


#endif //HEXAR_BOARD_H
