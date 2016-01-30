#pragma once
#include <vector>
#include "onut.h"

class Player;
class DanceMove;

typedef std::vector<Player*>                    PlayerVect;
typedef std::vector<DanceMove>                  DanceMoveVect;
typedef std::vector<onut::GamePad::eGamePad>    DanceMoveButtonVect;
