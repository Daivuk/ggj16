#include "GameView.h"
#include "Tile.h"

// A* stuff
float GameView::LeastCostEstimate(void* stateStart, void* stateEnd)
{
    int fromX, fromY, toX, toY;
    nodeToXY(stateStart, &fromX, &fromY);
    nodeToXY(stateEnd, &toX, &toY);

    int dx = toX - fromX;
    int dy = toY - fromY;

    return (float)sqrt((double)(dx * dx) + (double)(dy * dy));
}

void GameView::AdjacentCost(void* node, MP_VECTOR< micropather::StateCost > *neighbors)
{
    int x, y;
    const float cost[8] = {1.0f, 1.41f, 1.0f, 1.41f, 1.0f, 1.41f, 1.0f, 1.41f};

    nodeToXY(node, &x, &y);

    if (passable(x + 1, y))
    {
        micropather::StateCost nodeCost = {xyToNode(x + 1, y), 1.0f};
        if (nodeCost.state) neighbors->push_back(nodeCost);
    }
    else
    {
        micropather::StateCost nodeCost = {xyToNode(x + 1, y), 1.0f * 10.f};
        if (nodeCost.state) neighbors->push_back(nodeCost);
    }
    if (passable(x - 1, y))
    {
        micropather::StateCost nodeCost = {xyToNode(x - 1, y), 1.0f};
        if (nodeCost.state) neighbors->push_back(nodeCost);
    }
    else
    {
        micropather::StateCost nodeCost = {xyToNode(x - 1, y), 1.0f * 10.f};
        if (nodeCost.state) neighbors->push_back(nodeCost);
    }
    if (passable(x, y + 1))
    {
        micropather::StateCost nodeCost = {xyToNode(x, y + 1), 1.0f};
        if (nodeCost.state) neighbors->push_back(nodeCost);
    }
    else
    {
        micropather::StateCost nodeCost = {xyToNode(x, y + 1), 1.0f * 10.f};
        if (nodeCost.state) neighbors->push_back(nodeCost);
    }
    if (passable(x, y - 1))
    {
        micropather::StateCost nodeCost = {xyToNode(x, y - 1), 1.0f};
        if (nodeCost.state) neighbors->push_back(nodeCost);
    }
    else
    {
        micropather::StateCost nodeCost = {xyToNode(x, y - 1), 1.0f * 10.f};
        if (nodeCost.state) neighbors->push_back(nodeCost);
    }
    if (passable(x + 1, y + 1) && (passable(x + 1, y) || passable(x, y + 1)))
    {
        micropather::StateCost nodeCost = {xyToNode(x + 1, y + 1), 1.41f};
        if (nodeCost.state) neighbors->push_back(nodeCost);
    }
    else
    {
        micropather::StateCost nodeCost = {xyToNode(x + 1, y + 1), 1.41f * 10.f};
        if (nodeCost.state) neighbors->push_back(nodeCost);
    }
    if (passable(x + 1, y - 1) && (passable(x + 1, y) || passable(x, y - 1)))
    {
        micropather::StateCost nodeCost = {xyToNode(x + 1, y - 1), 1.41f};
        if (nodeCost.state) neighbors->push_back(nodeCost);
    }
    else
    {
        micropather::StateCost nodeCost = {xyToNode(x + 1, y - 1), 1.41f * 10.f};
        if (nodeCost.state) neighbors->push_back(nodeCost);
    }
    if (passable(x - 1, y + 1) && (passable(x - 1, y) || passable(x, y + 1)))
    {
        micropather::StateCost nodeCost = {xyToNode(x - 1, y + 1), 1.41f};
        if (nodeCost.state) neighbors->push_back(nodeCost);
    }
    else
    {
        micropather::StateCost nodeCost = {xyToNode(x - 1, y + 1), 1.41f * 10.f};
        if (nodeCost.state) neighbors->push_back(nodeCost);
    }
    if (passable(x - 1, y - 1) && (passable(x - 1, y) || passable(x, y - 1)))
    {
        micropather::StateCost nodeCost = {xyToNode(x - 1, y - 1), 1.41f};
        if (nodeCost.state) neighbors->push_back(nodeCost);
    }
    else
    {
        micropather::StateCost nodeCost = {xyToNode(x - 1, y - 1), 1.41f * 10.f};
        if (nodeCost.state) neighbors->push_back(nodeCost);
    }
}

void GameView::PrintStateInfo(void* state)
{
}

int GameView::findPath(const Vector2 &from, const Vector2 &to, std::vector<Vector2>* path, float* cost)
{
    static MP_VECTOR< void* > microPath;
    auto ret = m_pPather->Solve(xyToNode((int)from.x, (int)from.y),
                                xyToNode((int)to.x, (int)to.y),
                                &microPath, cost);
    path->clear();
    int x, y;
    for (decltype(microPath.size()) i = 0; i < microPath.size(); ++i)
    {
        nodeToXY(microPath[i], &x, &y);
        path->push_back({(float)x + .5f, (float)y + .5f});
    }
    return ret;
}

void GameView::nodeToXY(void* node, int* x, int* y)
{
    int mapWidth = m_pTilemap->getWidth();

    intptr_t index = (intptr_t)node;
    *y = index / mapWidth;
    *x = index - *y * mapWidth;
}

void* GameView::xyToNode(int x, int y)
{
    if (x < 0 || y < 0 || x >= m_pTilemap->getWidth() || y >= m_pTilemap->getHeight()) return nullptr;
    int mapWidth = m_pTilemap->getWidth();
    return (void*)(y * mapWidth + x);
}

bool GameView::passable(int x, int y)
{
    auto pTile = GetTileAt(x, y);
    if (!pTile) return false;
    return !pTile->isOccupied;
}
