#ifndef ASSIGN3_AGENT_H
#define ASSIGN3_AGENT_H

#include "BreadthAndCoord.h"
#include "templatelinkedlist.h"
#include <algorithm>
#include <chrono>
#include <iostream>
#include <mcpp/mcpp.h>
#include <queue>
#include <random>
#include <set>
#include <thread>
#include <vector>

#define MOVE_XPLUS mcpp::Coordinate(1, 0, 0)
#define MOVE_XMINUS mcpp::Coordinate(-1, 0, 0)
#define MOVE_ZPLUS mcpp::Coordinate(0, 0, 1)
#define MOVE_ZMINUS mcpp::Coordinate(0, 0, -1)

enum AgentOrientation { X_PLUS, Z_PLUS, X_MINUS, Z_MINUS };

class Agent {

public:
    Agent(bool mode);
    ~Agent();
    void SolveMaze();
    void BreadthFirstSearch(void);

private:
    bool mode;
    bool solved;
    mcpp::MinecraftConnection mc;
    mcpp::Coordinate currentLocation;
    AgentOrientation orientation;

    void CheckOrientation();
    void InitialiseOritentation();
    bool isExit(const mcpp::Coordinate &loc);
    bool isFree(const mcpp::Coordinate &loc);

    void PrintPath(LinkedListTemplate<mcpp::Coordinate> &path);
    void CheckAndUpdateVisited(const mcpp::Coordinate &neighbour,
                               std::vector<mcpp::Coordinate> &visited,
                               std::queue<mcpp::Coordinate> &queue,
                               LinkedListTemplate<BreadthAndCoord> &predecessor,
                               int breadth);
    void HandleExitCondition(mcpp::Coordinate &currentLocation,
                             const mcpp::Coordinate &start,
                             LinkedListTemplate<mcpp::Coordinate> &path,
                             LinkedListTemplate<BreadthAndCoord> &predecessor,
                             std::queue<mcpp::Coordinate> &queue, int breadth);
    void Move(AgentOrientation DIR1, AgentOrientation DIR2,
              mcpp::Coordinate MOV1, mcpp::Coordinate MOV2);
};

#endif