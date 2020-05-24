
#ifndef HEXAR_UTILS_H
#define HEXAR_UTILS_H

#include "definitions.h"
#include <vector>

Tile add(Tile a, Tile b);

Tile subtract(Tile a, Tile b);

HexCoord tileToHex(Tile t);

/**
 * rounds to the nearest tile
 * @param c
 * @return
 */
Tile hexToTile(HexCoord c);

HexCoord cartToHex(CartCoord c);

Tile cartToTile(CartCoord c);

CartCoord hexToCart(HexCoord c);

HexCoord cubeToHex(CubeCoord c);

CubeCoord hexToCube(HexCoord c);

CubeCoord cubeRound(CubeCoord c);

HexCoord hexRound(HexCoord c);

std::vector<Tile> hexRing(Tile center, int radius);

std::vector<Tile> hexArea(Tile center, int radius);

float distance(CartCoord a, CartCoord b);

int hexDistance(Tile a, Tile b);

bool isNeighbor(Tile a, Tile b);

Tile getNeighbor(Tile t, int i);

void getNeighbors(Tile t, Tile *dst);

bool doIntersect(TileBoundingBox a, TileBoundingBox b);

SDL_Color lighten(SDL_Color color, float percentage=0.1);

SDL_Color darken(SDL_Color color, float percentage=0.1);

#endif //HEXAR_UTILS_H
