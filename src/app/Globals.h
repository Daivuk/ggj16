#pragma once
#include <vector>
#include <random>
#include "onut.h"

class Player;
class DanceMove;
class Entity;

typedef std::vector<Player*>                    PlayerVect;
typedef std::vector<DanceMove>                  DanceMoveVect;
typedef std::vector<onut::GamePad::eGamePad>    DanceMoveButtonVect;
typedef std::vector<Entity*>                    EntityVect;


// Randomizes the content of the vector passed in

template<typename T>
static void RandomizeVector(std::vector<T>& inOut_vectorToRandomize)
{
    std::random_device rd;
    std::mt19937 g(rd());

    std::shuffle(inOut_vectorToRandomize.begin(), inOut_vectorToRandomize.end(), g);
}

template<typename T>
static T CubicBezier(const std::vector<T>& in_controlPoints, float in_t)
{
    // paranthesis added for clarity
    const float t = 1.f - in_t;
    return ((t)(t)(t)* in_controlPoints[0]) +
        (3  ((t)(t))  in_t  in_controlPoints[1]) +
        (3  (t)(in_t  in_t)  in_controlPoints[2]) +
        ((in_t*in_t*in_t) * in_controlPoints[3]);
}

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

#define MAX_MONSTER_COUNT 50

enum class DropType
{
    INVALID,
    Wood,
    Rock,
};

extern bool g_activePlayer[4];
