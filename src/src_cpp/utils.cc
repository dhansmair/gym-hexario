#include "include/utils.h"

using namespace std;


Tile add(Tile a, Tile b)
{
    return Tile{a.q + b.q, a.r + b.r};
}

Tile subtract(Tile a, Tile b)
{
    return Tile{a.q - b.q, a.r - b.r};
}

HexCoord tileToHex(Tile t)
{
    return HexCoord{(float)t.q, (float)t.r};
}

Tile hexToTile(HexCoord c)
{
    c = hexRound(c);
    return Tile{(int)c.q, (int)c.r};
}

HexCoord cartToHex(CartCoord c)
{
    float q = SQRT3 / 3. * c.x - 1. / 3. * c.y;
    float r = 2. / 3. * c.y;
    return HexCoord{q, r};
}

CartCoord hexToCart(HexCoord c)
{
    float x = SQRT3 * c.q + SQRT3 / 2.0 * c.r;
    float y = 3. / 2. * c.r;
    return CartCoord{x, y};
}

HexCoord cubeToHex(CubeCoord c)
{
    return HexCoord{c.x, c.z};
}

CubeCoord hexToCube(HexCoord c)
{
    float x = c.q;
    float z = c.r;
    float y = -x - z;
    return CubeCoord{x, y, z};
}

Tile cartToTile(CartCoord c)
{
    return hexToTile(cartToHex(c));
}

CubeCoord cubeRound(CubeCoord c)
{
    float rx = round(c.x);
    float ry = round(c.y);
    float rz = round(c.z);

    float dx = abs(rx - c.x);
    float dy = abs(ry - c.y);
    float dz = abs(rz - c.z);

    if (dx > dy && dx > dz) {
        rx = -ry - rz;
    } else if (dy > dz) {
        ry = -rx - rz;
    } else {
        rz = -rx - ry;
    }

    return CubeCoord{rx, ry, rz};
}

HexCoord hexRound(HexCoord c)
{
    return cubeToHex(cubeRound(hexToCube(c)));
}

vector<Tile> hexRing(Tile center, int radius)
{
    vector<Tile> ring;
    radius -= 1;
    Tile tile = add(center, Tile{-radius, radius});

    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < radius; j++) {
            ring.push_back(tile);
            tile = getNeighbor(tile, i);
        }
    }

    return ring;
}

vector<Tile> hexArea(Tile center, int radius)
{
    vector<Tile> area = {center};

    for (int r = 1; r < radius; r++) {
        Tile tile = add(center, Tile{-r, r});
        for (int i = 0; i < 6; i++) {
            for (int j = 0; j < r; j++) {
                area.push_back(tile);
                tile = getNeighbor(tile, i);
            }
        }
    }

    return area;
}

float distance(CartCoord a, CartCoord b)
{
    float dx = a.x - b.x;
    float dy = a.y - b.y;

    return sqrt(dx * dx + dy * dy);
}

int hexDistance(Tile a, Tile b)
{
    return (abs(a.q - b.q)
          + abs(a.q + a.r - b.q - b.r)
          + abs(a.r - b.r)) / 2;
}

bool isNeighbor(Tile a, Tile b)
{
    return 1. == abs(a.q - b.q) + abs(a.r - b.r);
}

Tile getNeighbor(Tile t, int i)
{
    Tile dir = DIRECTIONS[i];
    return Tile{t.q + dir.q, t.r + dir.r};
}

void getNeighbors(Tile t, Tile *dst)
{
    for (int i = 0; i < 6; i++) {
        dst[i] = getNeighbor(t, i);
    }
}

bool doIntersect(TileBoundingBox a, TileBoundingBox b)
{
    return !(a.min.q > b.max.q ||
             a.max.q < b.min.q ||
             a.max.r < b.min.r ||
             a.min.r > b.max.r);
}

SDL_Color lighten(SDL_Color color, float percentage)
{
    auto val = (Uint8) (255. * percentage);
    Uint8 r = min(color.r + val, 255);
    Uint8 g = min(color.g + val, 255);
    Uint8 b = min(color.b + val, 255);
    return SDL_Color{r, g, b, 255};
}

SDL_Color darken(SDL_Color color, float percentage)
{
    auto val = (Uint8) (255. * percentage);
    Uint8 r = max(color.r - val, 0);
    Uint8 g = max(color.g - val, 0);
    Uint8 b = max(color.b - val, 0);
    return SDL_Color{r, g, b, 255};
}