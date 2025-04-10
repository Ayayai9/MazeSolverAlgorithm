#include "../include/Agent.h"

Agent::Agent(bool mode) { this->mode = mode; }
void Agent::SolveMaze() {
    InitialiseOritentation();

    /*initilise variables*/
    currentLocation = mc.getPlayerPosition();
    LinkedListTemplate<mcpp::Coordinate> storage;
    solved = false;
    int counter = 1;

    /*print path while not solved*/
    while (!solved) {
        storage.addToEnd(currentLocation);
        std::cout << "Step[" << counter << "]: (" << currentLocation.x << ", "
                  << currentLocation.y;
        std::cout << ", " << currentLocation.z << ")\n";
        mc.setBlock(currentLocation, mcpp::Blocks::LIME_CARPET);
        std::this_thread::sleep_for(std::chrono::seconds(1));
        if (storage.getSize() > 1) {
            mc.setBlock(storage.at(storage.getSize() - 2), mcpp::Blocks::AIR);
        }
        CheckOrientation();
        counter++;
    }
}
void Agent::InitialiseOritentation() {
    if (mode) {
        orientation = Z_PLUS;
    } else {
        if (!(mc.getBlock(mcpp::Coordinate(1, 0, 0)) == mcpp::Blocks::AIR)) {
            orientation = Z_PLUS;
        } else if (!(mc.getBlock(mcpp::Coordinate(-1, 0, 0)) ==
                     mcpp::Blocks::AIR)) {
            orientation = Z_MINUS;
        } else if (!(mc.getBlock(mcpp::Coordinate(0, 0, 1)) ==
                     mcpp::Blocks::AIR)) {
            orientation = X_MINUS;
        } else if (!(mc.getBlock(mcpp::Coordinate(0, 0, -1)) ==
                     mcpp::Blocks::AIR)) {
            orientation = X_PLUS;
        }
    }
}

void Agent::CheckOrientation() {
    if (orientation == X_PLUS) {
        Move(Z_MINUS, Z_PLUS, MOVE_ZMINUS, MOVE_XPLUS);
    } else if (orientation == Z_PLUS) {
        Move(X_PLUS, X_MINUS, MOVE_XPLUS, MOVE_ZPLUS);
    } else if (orientation == X_MINUS) {
        Move(Z_PLUS, Z_MINUS, MOVE_ZPLUS, MOVE_XMINUS);
    } else if (orientation == Z_MINUS) {
        Move(X_MINUS, X_PLUS, MOVE_XMINUS, MOVE_ZMINUS);
    }
}

bool Agent::isFree(const mcpp::Coordinate &loc) {
    return mc.getBlock(loc) == mcpp::Blocks::AIR;
}

bool Agent::isExit(const mcpp::Coordinate &loc) {
    return (mc.getBlock(loc) == mcpp::Blocks::BLUE_CARPET);
}

Agent::~Agent() {}

void Agent::BreadthFirstSearch(void) {
    /*initialise variables*/
    LinkedListTemplate<mcpp::Coordinate> path;
    LinkedListTemplate<BreadthAndCoord> predecessor;
    int breadth = 0;
    std::queue<mcpp::Coordinate> queue;
    std::vector<mcpp::Coordinate> visited;
    mcpp::Coordinate start = mc.getPlayerPosition();
    mcpp::Coordinate marker(-1, -1, -1);

    /*add node to search*/
    predecessor.addToEnd(BreadthAndCoord(breadth, start));
    breadth++;
    queue.push(start);
    queue.push(marker);
    visited.push_back(start);

    /*because we check relative to block we do getblock on all 4 directions of
    currentLocation therefore, it is a bit slow so we notify user that it may
    take some time to find exit*/
    std::cout << "finding path please wait...!\n";

    while (!queue.empty()) {
        currentLocation = queue.front();
        queue.pop();
        // If we've reached the end of the current depth
        if (currentLocation == marker) {
            if (!queue.empty()) {
                breadth++;
                // Add a marker for the next depth
                queue.push(marker);
            }
        }

        if (isExit(currentLocation)) {
            HandleExitCondition(currentLocation, start, path, predecessor,
                                queue, breadth);
        }
        for (const mcpp::Coordinate neighbour :
             {currentLocation + MOVE_XPLUS, currentLocation + MOVE_XMINUS,
              currentLocation + MOVE_ZPLUS, currentLocation + MOVE_ZMINUS}) {
            CheckAndUpdateVisited(neighbour, visited, queue, predecessor,
                                  breadth);
        }
    }
    PrintPath(path);
}

void Agent::CheckAndUpdateVisited(
    const mcpp::Coordinate &neighbour, std::vector<mcpp::Coordinate> &visited,
    std::queue<mcpp::Coordinate> &queue,
    LinkedListTemplate<BreadthAndCoord> &predecessor, int breadth) {
    if ((isFree(neighbour) || isExit(neighbour)) &&
        std::find(visited.begin(), visited.end(), neighbour) == visited.end()) {
        visited.push_back(neighbour);
        queue.push(neighbour);
        predecessor.addToEnd(BreadthAndCoord(breadth, neighbour));
    }
}
void Agent::HandleExitCondition(
    mcpp::Coordinate &currentLocation, const mcpp::Coordinate &start,
    LinkedListTemplate<mcpp::Coordinate> &path,
    LinkedListTemplate<BreadthAndCoord> &predecessor,
    std::queue<mcpp::Coordinate> &queue, int breadth) {
    breadth = predecessor.distanceAt(currentLocation);
    while (!(currentLocation == start)) {
        breadth--;
        path.addToStart(currentLocation);
        for (const mcpp::Coordinate neighbour :
             {currentLocation + MOVE_XPLUS, currentLocation + MOVE_XMINUS,
              currentLocation + MOVE_ZPLUS, currentLocation + MOVE_ZMINUS}) {
            if (predecessor.contains(neighbour, breadth)) {
                currentLocation = neighbour;
            }
        }
    }
    while (!queue.empty()) {
        queue.pop();
    }
}
void Agent::PrintPath(LinkedListTemplate<mcpp::Coordinate> &path) {
    path.deleteLast();
    int counter = 0;
    mcpp::Coordinate prev;

    for (Node<mcpp::Coordinate> *node = path.getFirst(); node != nullptr;
         node = path.getNext(node)) {
        counter++;
        std::cout << "Step[" << counter << "]: (" << node->node.x << ", ";
        std::cout << node->node.y << ", " << node->node.z << ")\n";
        mc.setBlock(node->node, mcpp::Blocks::LIME_CARPET);
        if (counter > 1) {
            mc.setBlock(prev, mcpp::Blocks::AIR);
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
        prev = node->node;
    }
    mc.setBlock(prev, mcpp::Blocks::AIR);
}

void Agent::Move(AgentOrientation DIR1, AgentOrientation DIR2,
                 mcpp::Coordinate MOV1, mcpp::Coordinate MOV2) {
    if (isExit(currentLocation + MOV1)) {
        solved = true;
    } else if (isFree(currentLocation + MOV1)) {
        orientation = DIR1;
        currentLocation = currentLocation + MOV1;
    } else if (isFree(currentLocation + MOV2)) {
        currentLocation = currentLocation + MOV2;
    } else {
        orientation = DIR2;
    }
}
