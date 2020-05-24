#ifndef HEXAR_DEFINITIONS_H
#define HEXAR_DEFINITIONS_H

#include <cmath>
#include <functional>
#include <SDL2/SDL.h>


typedef struct Tile {
    int q, r;

    bool operator==(const Tile &other) const
    {
        return (q == other.q && r == other.r);
    }

    bool operator!=(const Tile &other) const
    {
        return (q != other.q || r != other.r);
    }

} Tile;

/**
 * define the hash function for Tile,
 * so it can be used as key in a map
 */
namespace std
{
    template <>
    struct hash<Tile>
    {
        size_t operator()(const Tile& t) const
        {
            return 13 * t.q + 17 * t.r;
        }
    };
}

typedef struct {
    float q, r;
} HexCoord;

typedef struct {
    float x, y;
} CartCoord;

typedef struct {
    float x, y, z;
} CubeCoord;

typedef struct {
    Sint16 x, y;
} ScreenCoord;

typedef struct {
    Tile min, max;
} TileBoundingBox;

const Tile DIRECTIONS[6] = {
        Tile{1, 0},
        Tile{1, -1},
        Tile{0, -1},
        Tile{-1, 0},
        Tile{-1, 1},
        Tile{0, 1}
};
const Tile CENTER = Tile{0,0};

const float PLAYER_DEFAULT_VELOCITY = 0.1;
const float TILE_UNIT_SIZE = 1.0;
const float CLAIMED_TILE_UNIT_SIZE = 0.7;
const float PLAYER_PATH_WIDTH = 0.5;

const int FREE_TILE = -1;
const int BORDER_TILE = -2;

const int WIN_WIDTH  = 800;
const int WIN_HEIGHT = 600;
const int WIN_SCALE  = 10;

const int OBS_SCALE  = 10;

const SDL_Color COLOR_WHITE = SDL_Color{255, 255, 255, 255};
const SDL_Color COLOR_RED   = SDL_Color{255, 0, 0, 255};
const SDL_Color COLOR_GREEN = SDL_Color{0, 255, 0, 255};
const SDL_Color COLOR_BLUE  = SDL_Color{0, 0, 255, 255};
const SDL_Color COLOR_BACKGROUND = COLOR_WHITE;
const SDL_Color COLOR_DEMO_BACKGROUND = SDL_Color{230, 230, 230, 255};
const SDL_Color COLOR_BORDER = COLOR_RED;
const SDL_Color COLOR_INVALID = COLOR_DEMO_BACKGROUND;

// very very important constant :D
const double SQRT3 = sqrt(3.0);

const SDL_Color BLACK  = SDL_Color{0,0,0, 255};
const SDL_Color GRAY   = SDL_Color{120, 120, 120, 255};
const SDL_Color WHITE  = SDL_Color{255, 255, 255, 255};
const SDL_Color BLUE   = SDL_Color{0, 102, 255, 255};
const SDL_Color GREEN  = SDL_Color{0,255,0, 255};
const SDL_Color RED    = SDL_Color{255,0,0, 255};
const SDL_Color PURPLE = SDL_Color{255,51,153, 255};
const SDL_Color BROWN  = SDL_Color{135, 67, 51, 255};
const SDL_Color ORANGE = SDL_Color{245, 105, 12, 255};
const SDL_Color YELLOW = SDL_Color{233, 240, 43, 255};

const SDL_Color PLAYER_COLORS[] = {
    GREEN,
    BLUE,
    PURPLE,
    BROWN,
    ORANGE,
    YELLOW,
    GRAY,
    GRAY,
    GRAY,
    GRAY,
    GRAY,
    GRAY,
    GRAY,
    GRAY,
    GRAY,
    GRAY,
    GRAY,
    GRAY,
    GRAY,
    GRAY
};


#endif //HEXAR_DEFINITIONS_H
