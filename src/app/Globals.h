#pragma once
#include <vector>
#include "onut.h"

class Player;
class DanceMove;

typedef std::vector<Player*>                    PlayerVect;
typedef std::vector<DanceMove>                  DanceMoveVect;
typedef std::vector<onut::GamePad::eGamePad>    DanceMoveButtonVect;

#define SPRITE_SCALE (1.f / 16.f)
