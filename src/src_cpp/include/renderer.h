#ifndef HEXAR_RENDERER_H
#define HEXAR_RENDERER_H

#include <vector>
#include <SDL2/SDL.h>
#include <SDL2_gfxPrimitives.h>
#include "definitions.h"
#include "game.h"


class Renderer
{
protected:
    Sint16 tileRadiusScreen;
    Sint16 tileHeightScreen;
    ScreenCoord transform(CartCoord c);
    Sint16 transform(float f);

    int width;
    int height;
    float scale;
    CartCoord focus{};

    Game &game;
    SDL_Surface *surface;
    SDL_Renderer *renderer;

    TileBoundingBox getViewportBoundingBox();

    void drawLines(std::vector<CartCoord> points, float width, SDL_Color color);
    void drawCircle(CartCoord center, float radius, SDL_Color color);
    void drawHexagon(CartCoord center, float radius, SDL_Color color);
    void drawTile(Tile tile, SDL_Color color);

    void render();

public:
    Renderer(Game &game, int width, int height);
    ~Renderer();

    // getter
    int getWidth();
    int getHeight();
    float getScale();
    CartCoord getFocus();

    // setter
    void setScale(float scale);
    void setFocus(CartCoord focus);

};


/**
 *
 */
class DemoRenderer : public Renderer
{
private:
    SDL_Window *window;
    bool handleEvent(SDL_Event &e);

public:
    DemoRenderer(Game &game, int width, int height);
    ~DemoRenderer();
    void setScale(float s);
    bool render();
};


/**
 * - focus is always on a specific player
 */
class ObservationRenderer : public Renderer
{
private:
    Uint32 getPixel(int x, int y);
    

public:
    ObservationRenderer(Game &game, int width, int height);
    ~ObservationRenderer();
    void renderForPlayer(Player &player);
    void writeTo(float *obsMemAddr, unsigned long size);
    //void getPixel(int x, int y, uint8_t *r, uint8_t *g, uint8_t *b);
    void showOnly(CartCoord pos, int hexRadius);
};


#endif //HEXAR_RENDERER_H
