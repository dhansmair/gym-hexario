#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <cmath>
#include <cassert>
#include <queue>
#include <algorithm>

#include "include/board.h"
#include "include/utils.h"

using namespace std;
/*
tile := (q, r)

Map for radius = 3:

       (0,-3) ---------
          \
           \
            \
             \
              \
(-3,0) ------------------- (3,0)
                 \
                  \
                   \
                    \
        ----------(0, 3)

*/

Board::Board(int radius) : radius(radius), numTiles(1 + 3 * radius * (radius - 1)) 
{
    reset();
}

/**
 * delete all tiles and set up the border again
 */
void Board::reset()
{
    tiles.clear();
    for (auto &tile : hexRing(CENTER, radius+1)) {
        tiles.insert({tile, BORDER_TILE});
    }
}

int Board::getRadius()
{
    return radius;
}

int Board::getNumTiles()
{
    return numTiles;
}

void Board::setFree(Tile tile)
{
    if (tiles.count(tile) > 0) {
       tiles.erase(tile);
    }
}

void Board::setOwner(Tile tile, Player &player)
{
    if (tiles.count(tile) == 0) {
        tiles.insert({tile, player.getId()});
    } else {
        tiles.at(tile) = player.getId();
    }
}

void Board::setOwner(const vector<Tile> &ts, Player &player)
{
    for (auto &tile : ts) {
        setOwner(tile, player);
    }
}

void Board::setOwner(const unordered_set<Tile> &ts, Player &player)
{
    for (auto &tile : ts) {
        setOwner(tile, player);
    }
}

int Board::getOwner(Tile tile)
{
    if (tiles.count(tile) == 0) {
        return FREE_TILE;
    }

    return tiles.at(tile);
}

bool Board::isOwner(Tile tile, Player &player)
{
    return (getOwner(tile) == player.getId());
}

unordered_set<Tile> Board::getAll()
{
    unordered_set<Tile> result;

    for (auto &tile : tiles) {
        result.insert(tile.first);
    }

    return result;
}

unordered_set<Tile> Board::getAllByOwner(Player &player)
{
    unordered_set<Tile> result;

    for (auto &tile : tiles) {
        if (tile.second == (int) player.getId()) {
            result.insert(tile.first);
        }
    }

    return result;
}

int Board::freeAllByOwner(Player &player)
{
    int num = 0;
    auto it = tiles.cbegin();
    
    while (it != tiles.cend()) {
        auto curr = it++;
        
        if (curr->second == (int) player.getId()) {
            tiles.erase(curr);
            num++;
        }
    }
    
    return num;
}

bool Board::isValid(Tile t)
{
    return hexDistance(CENTER, t) <= radius;
}

/**
 * algorithm to calculate the set of tiles which is 
 * completely surrounded by the captured area of a player.
 * (the area of the player itself is not included)
 * 
 * The algorithm goes as follows:
 * 1. look at all tiles within the range of the players bounding box 
 *      increased by 1 in all directions
 * 2. store all of them which are not captured by the player in set A
 * 3. perform breath-first-search and remove all visited tiles from A.
 *      the root node is one of the outermost tiles.
 * 4. after the bfs, the remaining tiles in A are the included tiles.
 */
unordered_set<Tile> Board::getInclusions(Player &player)
{
    unordered_set<Tile> candidates = unordered_set<Tile>{};
    TileBoundingBox bounds = player.getBoundingBox();

    // get all inclusion candidates from the bounding box
    for (int q = bounds.min.q - 1; q <= bounds.max.q + 1; q++) {
        for (int r = bounds.min.r - 1; r <= bounds.max.r + 1; r++) {
            Tile tile = Tile{q, r};

            if (getOwner(tile) != (int) player.getId()) {
                candidates.insert(tile);
            }
        }
    }

    // create the queue for breadth-first-search and add the root element
    queue<Tile> bfsQueue;
    Tile front = Tile{bounds.min.q - 1, bounds.min.r - 1};
    candidates.erase(front);
    bfsQueue.push(front);

    // perform bfs search and instant elimination
    while (!bfsQueue.empty()) {
        front = bfsQueue.front();
        bfsQueue.pop();

        // get the neighbors of the current front tile
        for (int i = 0; i < 6; i++) {
            Tile n = getNeighbor(front, i);

            // check if it is in candidates
            if (candidates.count(n) > 0) {
                candidates.erase(n);
                bfsQueue.push(n);
            }
        }
    }

    // the remaining candidates are the ones who are included by the player
    return candidates;
}

Tile Board::getRandomTile()
{
    Tile tile;

    do {
        int randomQ = rand() % (radius * 2) - radius;
        int randomR = rand() % (radius * 2) - radius;
        
        tile = Tile{randomQ, randomR};
    } while (hexDistance(CENTER, tile) > radius-3);

    return tile;
    //return Tile{3, 3};
}