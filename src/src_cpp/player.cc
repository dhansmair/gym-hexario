#include <iostream>
#include <cmath>
#include <algorithm>
#include "include/player.h"
#include "include/utils.h"

using namespace std;


Player::Player(int id, float velocity) : 
    id(id), position(CartCoord{0, 0}), velocity(velocity)
{
    dead = true;
    winner = false;
    kills = 0;
    captures = 0;
    path.clear();
    claimedArea.clear();
    
    boundingBox = TileBoundingBox{
        Tile{0, 0},
        Tile{0, 0}
    };
}

bool Player::operator==(Player &other)
{
    return id == other.getId();
}

bool Player::operator!=(Player &other)
{
    return id != other.getId();
}

int Player::getId()
{
    return id;
}

bool Player::isDead()
{
    return dead;
}

bool Player::isWinner()
{
    return winner;
}

int Player::getNumKills()
{
    return kills;
}

int Player::getNumCaptures()
{
    return captures;
}

int Player::getNumClaims()
{
    return claimedArea.size();
}

float Player::getVelocity()
{
    return velocity;
}

Tile Player::getHead()
{
    return head;
}

CartCoord Player::getPosition()
{
    return position;
}

std::unordered_set<Tile> &Player::getClaim()
{
    return claimedArea;
}

TileBoundingBox Player::getBoundingBox()
{
    return boundingBox;
}

vector<CartCoord> &Player::getPath()
{
    return path;
}

void Player::setDead(bool d)
{
    dead = d;
}

void Player::setWinner(bool w)
{
    winner = w;
}

void Player::addKill() 
{
    kills++;
}

void Player::setCaptures(int c)
{
    captures = c;
}

void Player::addCaptures(int c)
{
    captures += c;
}

void Player::setPosition(CartCoord pos)
{
    position = pos;
    head = hexToTile(cartToHex(pos));
}

void Player::setVelocity(float v)
{
    velocity = v;
}

void Player::claim(Tile t)
{
    if (path.empty() || distance(position, path.back()) > 1) {
        path.push_back(position);
    }

    if (!hasClaimed(t)) {
        claimedArea.insert(t);
        boundingBox.min.q = min(boundingBox.min.q, head.q);
        boundingBox.max.q = max(boundingBox.max.q, head.q);
        boundingBox.min.r = min(boundingBox.min.r, head.r);
        boundingBox.max.r = max(boundingBox.max.r, head.r);
    }
}

void Player::approveClaim()
{
    claimedArea.clear();
    path.clear();
}

bool Player::hasAnyClaim()
{
    return !claimedArea.empty();
}

bool Player::hasClaimed(Tile t)
{
    return find(claimedArea.begin(), claimedArea.end(), t) != claimedArea.end();
}

//void Player::reset()
//{
//    dead = true;
//    winner = false;
//    kills = 0;
//    captures = 0;
//    path.clear();
//    claimedArea.clear();

//    boundingBox = TileBoundingBox{
//        Tile{0, 0},
//        Tile{0, 0}
//    };
//}

void Player::spawn(Tile spawnPoint)
{
    dead = false;
    winner = false;
    kills = 0;
    captures = 7;
    path.clear();
    claimedArea.clear();

    // reset position
    head = spawnPoint;
    position = hexToCart(tileToHex(spawnPoint));

    boundingBox = TileBoundingBox{
        Tile{spawnPoint.q - 1, spawnPoint.r - 1},
        Tile{spawnPoint.q + 1, spawnPoint.r + 1}
    };
}
