#pragma once
#include <vector>
#include "onut.h"

class Player;
class DanceMove;
class Entity;

typedef std::vector<Player*>                    PlayerVect;
typedef std::vector<DanceMove>                  DanceMoveVect;
typedef std::vector<onut::GamePad::eGamePad>    DanceMoveButtonVect;
typedef std::vector<Entity*>                    EntityVect;

#define SPRITE_SCALE (1.f / 16.f)

#define NIGHT_DURATION 60.f
#define DAWN_DURATION 5.f
#define DAY_DURATION 30.f
#define DUSK_DURATION 15.f

#define DAY_TOTAL_DURATION (NIGHT_DURATION + DAWN_DURATION + DAY_DURATION + DUSK_DURATION)

#define DAWN_START  (NIGHT_DURATION * .5f)
#define DAWN_END    (NIGHT_DURATION * .5f + DAWN_DURATION)
#define DUSK_START  (NIGHT_DURATION * .5f + DAWN_DURATION + DAY_DURATION)
#define DUSK_END    (NIGHT_DURATION * .5f + DAWN_DURATION + DAY_DURATION + DUSK_DURATION)

#define MIDNIGHT 0.f
#define NOON (NIGHT_DURATION * .5f + DAWN_DURATION + DAY_DURATION * .5f)
