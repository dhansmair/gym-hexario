#ifndef HEXAR_GAMESTATE_H
#define HEXAR_GAMESTATE_H

#include <vector>
#include <unordered_set>

#include "definitions.h"
#include "utils.h"
#include "board.h"
#include "player.h"


class Game {
    private:
        const int numPlayers;
        const int mapRadius;
        const bool discrete;
        Board board;
        std::vector<Player> players;

        CartCoord calcStep(Player &player, float direction);
        void checkBorderCollisions();
        void checkTileCaptures();
        void checkCollisions();

        /**
         * check if a player has captured the whole map
         * -> game is over, player is the winner
         */
        void checkHasCapturedAll(Player &player);

        void approveCaptures(Player &player, std::unordered_set<Tile> tiles);
        void setDead(Player &player);
        
    public:
        Game(int numPlayers, int mapRadius, bool discrete, float velocity);
        ~Game();

        Board &getBoard();
        Player &getPlayer(int playerId);
        std::vector<Player> &getPlayers();

        void takeActions(std::vector<float> actions);
        void resetPlayer(int playerId);
};



#endif