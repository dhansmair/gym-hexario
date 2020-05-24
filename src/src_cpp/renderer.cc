#include <iostream>
#include <cmath>
#include "include/renderer.h"
#include "include/utils.h"

using namespace std;


const SDL_Color OBSERVATION_PLAYER_COLOR = GREEN;
const SDL_Color OBSERVATION_PLAYER_COLOR_LIGHT = lighten(OBSERVATION_PLAYER_COLOR, 0.4);
const SDL_Color OBSERVATION_PLAYER_COLOR_DARK1 = darken(OBSERVATION_PLAYER_COLOR);
const SDL_Color OBSERVATION_PLAYER_COLOR_DARK2 = darken(OBSERVATION_PLAYER_COLOR, 0.4);
const SDL_Color OBSERVATION_ENEMY_COLOR = BLUE;
const SDL_Color OBSERVATION_ENEMY_COLOR_LIGHT = lighten(OBSERVATION_ENEMY_COLOR, 0.4);
const SDL_Color OBSERVATION_ENEMY_COLOR_DARK1 = darken(OBSERVATION_ENEMY_COLOR);
const SDL_Color OBSERVATION_ENEMY_COLOR_DARK2 = darken(OBSERVATION_ENEMY_COLOR, 0.4);


Renderer::Renderer(Game &game, int w, int h) : game(game)
{
    width = w;
    height = h;

    surface = nullptr;
    renderer = nullptr;

    // initialize with dummy values, later overwritten by setScale()
    scale = 1;
    tileRadiusScreen = 0;
    tileHeightScreen = 0;

    // assign default values to these properties
    setScale(10);
    setFocus(CartCoord{0, 0});
}

Renderer::~Renderer() = default;

int Renderer::getWidth()
{
    return width;
}

int Renderer::getHeight()
{
    return height;
}

float Renderer::getScale()
{
    return scale;
}

CartCoord Renderer::getFocus()
{
    return focus;
}

void Renderer::setScale(float s)
{
    scale = s;
    tileRadiusScreen = (Sint16) (TILE_UNIT_SIZE * scale);
    tileHeightScreen = (Sint16) round((TILE_UNIT_SIZE * scale) / 2. * SQRT3);
}

void Renderer::setFocus(CartCoord f)
{
    focus = f;
}

TileBoundingBox Renderer::getViewportBoundingBox()
{
    // given width, height, focus, scale
    // get the four corners in cartesian coordinates
    float halfX = (width * 0.5) / scale;
    float halfY = (height * 0.5) / scale;

    float t = focus.y + halfY;
    float b = focus.y - halfY;
    float l = focus.x - halfX;
    float r = focus.x + halfX;

    // transform to hex coordinates, round to nearest tile
    Tile cornerTL = hexToTile(cartToHex(CartCoord{l, t}));
    Tile cornerBR = hexToTile(cartToHex(CartCoord{r, b}));

    // get the minimum and maximum q, r together, so the r
    // need to get flipped (axial coordinates are confusing)
    Tile lowerBound = Tile{cornerTL.q, cornerBR.r};
    Tile upperBound = Tile{cornerBR.q, cornerTL.r};

    return TileBoundingBox{lowerBound, upperBound};
}

void Renderer::render()
{
    SDL_SetRenderDrawColor(renderer,
            COLOR_BACKGROUND.r,
            COLOR_BACKGROUND.g,
            COLOR_BACKGROUND.b,
            COLOR_BACKGROUND.a);
    SDL_RenderClear(renderer);

    TileBoundingBox scope = getViewportBoundingBox();

    for (int q = scope.min.q; q <= scope.max.q; q++) {
        for (int r = scope.min.r; r <= scope.max.r; r++) {
            Tile tile = Tile{q, r};
            int ownerId = game.getBoard().getOwner(tile);

            if (ownerId == BORDER_TILE) {
                drawTile(tile, COLOR_RED);
            } else if (ownerId >= 0) {
                drawTile(tile, PLAYER_COLORS[ownerId]);
            }
        }
    }

    // determine which players are relevant for the current rendering viewport
    vector<Player> playersToRender = vector<Player>{};
    for (auto &player : game.getPlayers()) {
        if (!player.isDead() && doIntersect(scope, player.getBoundingBox())) {
            playersToRender.push_back(player);
        }
    }

    // render the claimed territories
    for (auto &player : playersToRender) {
        SDL_Color color = lighten(PLAYER_COLORS[player.getId()], 0.4);
        for (auto &tile : player.getClaim()) {
            CartCoord center = hexToCart(tileToHex(tile));
            drawHexagon(center, CLAIMED_TILE_UNIT_SIZE, color);
        }
    }

    // render the paths
    for (auto &player : playersToRender) {
        SDL_Color color = PLAYER_COLORS[player.getId()];
        drawLines(player.getPath(), PLAYER_PATH_WIDTH, darken(color));
    }

    // render the players themselves
    for (auto &player : playersToRender) {
        SDL_Color color = PLAYER_COLORS[player.getId()];
        drawCircle(player.getPosition(), 1, darken(color, 0.3));
    }

    SDL_RenderPresent(renderer);
}

void Renderer::drawLines(vector<CartCoord> points, float w, SDL_Color color)
{
    if (points.size() < 2) return;

    Sint16 lineWidth = transform(w);
    ScreenCoord c1 = transform(points[0]);

    for (unsigned long i = 1; i < points.size(); i++) {
        ScreenCoord c2 = transform(points[i]);
        thickLineRGBA(renderer, c1.x, c1.y, c2.x, c2.y, lineWidth,
                color.r, color.g, color.b, color.a);
        c1 = c2;
    }
}

void Renderer::drawCircle(CartCoord center, float radius, SDL_Color color)
{
    ScreenCoord c = transform(center);
    Sint16 rad = transform(radius);
    filledCircleRGBA(renderer, c.x, c.y, rad, color.r, color.g, color.b, color.a);
}

void Renderer::drawHexagon(CartCoord center, float radius, SDL_Color color)
{
    ScreenCoord c = transform(center);

    Sint16 x = c.x;
    Sint16 y = c.y;
    Sint16 r = transform(radius);
    auto h = (Sint16) round(((float) r) / 2. * SQRT3);

    Sint16 vx[6] = {
            x,
            static_cast<Sint16>(x + h),
            static_cast<Sint16>(x + h),
            x,
            static_cast<Sint16>(x - h),
            static_cast<Sint16>(x - h)
    };

    Sint16 vy[6] = {
            static_cast<Sint16>(y - r),
            static_cast<Sint16>(y - r/2),
            static_cast<Sint16>(y + r/2),
            static_cast<Sint16>(y + r),
            static_cast<Sint16>(y + r/2),
            static_cast<Sint16>(y - r/2)
    };

    filledPolygonRGBA(renderer, vx, vy, 6, color.r, color.g, color.b, color.a);
}

void Renderer::drawTile(Tile tile, SDL_Color color)
{
    ScreenCoord c = transform(hexToCart(tileToHex(tile)));

    Sint16 vx[6] = {
            c.x,
            static_cast<Sint16>(c.x + tileHeightScreen),
            static_cast<Sint16>(c.x + tileHeightScreen),
            c.x,
            static_cast<Sint16>(c.x - tileHeightScreen),
            static_cast<Sint16>(c.x - tileHeightScreen)
    };

    Sint16 vy[6] = {
            static_cast<Sint16>(c.y - tileRadiusScreen - 1),
            static_cast<Sint16>(c.y - tileRadiusScreen/2 - 1),
            static_cast<Sint16>(c.y + tileRadiusScreen/2 + 1),
            static_cast<Sint16>(c.y + tileRadiusScreen + 1),
            static_cast<Sint16>(c.y + tileRadiusScreen/2 + 1),
            static_cast<Sint16>(c.y - tileRadiusScreen/2 - 1)
    };

    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    filledPolygonRGBA(renderer, vx, vy, 6, color.r, color.g, color.b, color.a);
}

ScreenCoord Renderer::transform(CartCoord c)
{
    float x = ((c.x - focus.x) * scale) + width * 0.5;
    float y = ((c.y - focus.y) * scale) + height * 0.5;

    return ScreenCoord{(Sint16) round(x), (Sint16) round(y)};
}

Sint16 Renderer::transform(float f)
{
    return (Sint16) round(f * scale);
}

/*
 * #############################################################################################
 *
 *                                  Demo renderer starts here
 *
 * #############################################################################################
 * */

/**
 * constructor of the demo renderer; additionally adds the window instance
 * @param game
 * @param width
 * @param height
 */
DemoRenderer::DemoRenderer(Game &game, int width, int height) : Renderer(game, width, height)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
    }

    window  = SDL_CreateWindow("Hexar.io", 100, 100, width, height, SDL_WINDOW_RESIZABLE);
    surface = SDL_GetWindowSurface(window);
    renderer = SDL_GetRenderer(window);

    // somehow creating the renderer is different for various systems,
    // on my laptop I need to create a renderer explicitly whereas on the
    // cip computers the initialisation above works fine
    // TODO find a clean solution for this problem

    if (renderer == nullptr) {
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    }

    if (renderer == nullptr) {
        SDL_Log("Unable to initialize window software renderer: %s", SDL_GetError());
    }

    SDL_RenderPresent(renderer);
}

DemoRenderer::~DemoRenderer()
{
    if (renderer != nullptr) {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }

    if (surface != nullptr) {
        SDL_FreeSurface(surface);
        surface = nullptr;
    }

    if (window != nullptr) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }
}

bool DemoRenderer::handleEvent(SDL_Event &e)
{
    if (e.type == SDL_QUIT) {

        // TODO handle quit event correctly
        return true;

    } else if (e.type == SDL_WINDOWEVENT) {

        switch (e.window.event) {
            //Get new dimensions and repaint on window size change
            case SDL_WINDOWEVENT_SIZE_CHANGED:

                width = e.window.data1;
                height = e.window.data2;
                break;

            default:
                break;
        }
    } else if (e.type == SDL_MOUSEWHEEL) {

        if (e.wheel.y > 0) {
            // scroll up
            setScale(getScale() * 1.1);
        } else if (e.wheel.y < 0) {
            // scroll down
            setScale(getScale() * 0.9);
        }
    }

    return false;
}

void DemoRenderer::setScale(float s)
{
    float minScale = ((float) height) / ((4 + 2 * game.getBoard().getRadius()) * 1.5);
    float maxScale = height * 0.5;
    s = max(s, minScale);
    s = min(s, maxScale);
    Renderer::setScale(s);
}

/**
 * rendering loop function
 */
bool DemoRenderer::render()
{
    bool quit = false;
    // handle events
    SDL_Event e;
    while (SDL_PollEvent(&e) != 0) {
        if (handleEvent(e)) {
            quit = true;
        }
    }

    // render the game
    SDL_SetRenderDrawColor(renderer,
            COLOR_DEMO_BACKGROUND.r,
            COLOR_DEMO_BACKGROUND.g,
            COLOR_DEMO_BACKGROUND.b,
            COLOR_DEMO_BACKGROUND.a);
    SDL_RenderClear(renderer);

    bool renderGrid = (game.getBoard().getRadius() < 20);

    TileBoundingBox scope = getViewportBoundingBox();

    for (int q = scope.min.q; q <= scope.max.q; q++) {
        for (int r = scope.min.r; r <= scope.max.r; r++) {
            Tile tile = Tile{q, r};

            if (!game.getBoard().isValid(tile)) {
                continue;
            }

            int ownerId = game.getBoard().getOwner(tile);

            if (ownerId == BORDER_TILE) {
                drawTile(tile, COLOR_RED);
            } else if (renderGrid && ownerId == FREE_TILE) {
                drawHexagon(hexToCart(tileToHex(tile)), 0.95, COLOR_WHITE);
            } else if (ownerId >= 0) {
                drawTile(tile, PLAYER_COLORS[ownerId]);
            }
        }
    }

    // determine which players are relevant for the current rendering viewport
    vector<Player> playersToRender = vector<Player>{};
    for (auto &player : game.getPlayers()) {
        if (!player.isDead() && doIntersect(scope, player.getBoundingBox())) {
            playersToRender.push_back(player);
        }
    }

    // render the claimed territories
    for (auto &player : playersToRender) {
        SDL_Color color = lighten(PLAYER_COLORS[player.getId()], 0.7);
        for (auto &tile : player.getClaim()) {
            CartCoord center = hexToCart(tileToHex(tile));
            drawHexagon(center, CLAIMED_TILE_UNIT_SIZE, color);
        }
    }

    // render the paths
    for (auto &player : playersToRender) {
        SDL_Color color = PLAYER_COLORS[player.getId()];
        drawLines(player.getPath(), PLAYER_PATH_WIDTH, darken(color));
    }

    // render the players themselves
    for (auto &player : playersToRender) {
        SDL_Color color = PLAYER_COLORS[player.getId()];
        drawCircle(player.getPosition(), 1, darken(color, 0.3));
    }

    SDL_RenderPresent(renderer);

    return quit;
}

/*
 * #############################################################################################
 *
 *                                  Observation renderer starts here
 *
 * #############################################################################################
 * */

ObservationRenderer::ObservationRenderer(Game &game, int width, int height) : Renderer(game, width, height)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
    }

    surface = SDL_CreateRGBSurfaceWithFormat(0, width, height, 32, SDL_PIXELFORMAT_ABGR8888);
    renderer = SDL_CreateSoftwareRenderer(surface);
    SDL_RenderPresent(renderer);
}

ObservationRenderer::~ObservationRenderer()
{
    if (renderer != nullptr) {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }

    if (surface != nullptr) {
        SDL_FreeSurface(surface);
        surface = nullptr;
    }
}

void ObservationRenderer::renderForPlayer(Player &player)
{
    SDL_SetRenderDrawColor(renderer,
                           COLOR_DEMO_BACKGROUND.r,
                           COLOR_DEMO_BACKGROUND.g,
                           COLOR_DEMO_BACKGROUND.b,
                           COLOR_DEMO_BACKGROUND.a);
    SDL_RenderClear(renderer);

    if (player.isDead()) {
        return;
    }


    TileBoundingBox scope = getViewportBoundingBox();

    for (int q = scope.min.q; q <= scope.max.q; q++) {
        for (int r = scope.min.r; r <= scope.max.r; r++) {
            Tile tile = Tile{q, r};

            if (!game.getBoard().isValid(tile)) {
                continue;
            }


            int ownerId = game.getBoard().getOwner(tile);

            if (ownerId == BORDER_TILE) {
                drawTile(tile, COLOR_RED);
            } else if (ownerId == FREE_TILE) {
                // do not draw
                drawTile(tile, COLOR_WHITE);
            } else if (ownerId == player.getId()) {
                drawTile(tile, OBSERVATION_PLAYER_COLOR);
            } else {
                drawTile(tile, OBSERVATION_ENEMY_COLOR);
            }
        }
    }

    // determine which players are relevant for the current rendering viewport
    vector<Player> enemiesToRender = vector<Player>{};
    for (auto &p : game.getPlayers()) {
        if (p != player && doIntersect(scope, p.getBoundingBox())) {
            enemiesToRender.push_back(p);
        }
    }

    // render the claimed territories
    for (auto &enemy : enemiesToRender) {
        for (auto &tile : enemy.getClaim()) {
            CartCoord center = hexToCart(tileToHex(tile));
            drawHexagon(center, CLAIMED_TILE_UNIT_SIZE, OBSERVATION_ENEMY_COLOR_LIGHT);
        }
    }

    for (auto &tile : player.getClaim()) {
        CartCoord center = hexToCart(tileToHex(tile));
        drawHexagon(center, CLAIMED_TILE_UNIT_SIZE, OBSERVATION_PLAYER_COLOR_LIGHT);
    }

    // render the paths
    for (auto &enemy : enemiesToRender) {
        drawLines(enemy.getPath(), PLAYER_PATH_WIDTH, OBSERVATION_ENEMY_COLOR_DARK1);
    }
    drawLines(player.getPath(), PLAYER_PATH_WIDTH, OBSERVATION_PLAYER_COLOR_DARK1);

    // render the players themselves
    for (auto &enemy : enemiesToRender) {
        drawCircle(enemy.getPosition(), 1, OBSERVATION_ENEMY_COLOR_DARK2);
    }
    drawCircle(player.getPosition(), 1, OBSERVATION_PLAYER_COLOR_DARK2);

    // erase everything that is not in the scope of the agent, in case this is 
    // a partially observable setting
    // TODO
    // I need the coordinates of the corner of the viewport


    SDL_RenderPresent(renderer);
}


void ObservationRenderer::showOnly(CartCoord pos, int hexRadius)
{
    // calculate pixel position of pos
    ScreenCoord c = transform(pos);

    // calculate with and height of the area described by hexRadius around pos
    float numUnits = hexRadius * 1.5;
    float dist = numUnits * scale;

    // draw grey boxes everywhere above the image except the visible part 
    // draw 4 grey boxes
    SDL_SetRenderDrawColor(renderer,
                       COLOR_DEMO_BACKGROUND.r,
                       COLOR_DEMO_BACKGROUND.g,
                       COLOR_DEMO_BACKGROUND.b,
                       COLOR_DEMO_BACKGROUND.a);

    SDL_Rect r = {0, 0, c.x - dist, height};
    SDL_RenderFillRect(renderer, &r);

    r = {c.x + dist, 0, width - (c.x + dist) + 1, height};
    SDL_RenderFillRect(renderer, &r);

    r = {0, 0, width, c.y - dist};
    SDL_RenderFillRect(renderer, &r);

    r = {0, c.y + dist, width, height - (c.y + dist) + 1};
    SDL_RenderFillRect(renderer, &r);


    SDL_RenderPresent(renderer);
}


Uint32 ObservationRenderer::getPixel(int x, int y)
{
    int bpp = surface->format->BytesPerPixel;

    /* Here p is the address to the pixel we want to retrieve */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    switch (bpp) {
        case 1:
            return *p;
        case 2:
            return *(Uint16 *)p;
        case 3:
            if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
                return p[0] << 16 | p[1] << 8 | p[2];
            else
                return p[0] | p[1] << 8 | p[2] << 16;
        case 4:
            return *(Uint32 *)p;
        default:
            return 0;       /* shouldn't happen, but avoids warnings */
    }
}

void ObservationRenderer::writeTo(float *obsMemAddr, ulong size)
{
    if (size != ((ulong)(width * height * 3))) {
        cout << "error: given numpy array does provide a different size from what is required: " << size << endl;
        return;
    }

    SDL_LockSurface(surface);

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            Uint32 pixel = getPixel(x, y);
            Uint8 r, g, b;

            SDL_GetRGB(pixel, surface->format, &r, &g, &b);

            obsMemAddr[(y * width + x) * 3] = (float) r;
            obsMemAddr[(y * width + x) * 3 + 1] = (float) g;
            obsMemAddr[(y * width + x) * 3 + 2] = (float) b;
        }
    }

    SDL_UnlockSurface(surface);
}
