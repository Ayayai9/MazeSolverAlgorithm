#ifndef ASSIGN_MAZE_H
#define ASSIGN_MAZE_H

#include "BlockCoord.h"
#include "templatelinkedlist.h"
#include <algorithm>
#include <chrono>
#include <iostream>
#include <mcpp/mcpp.h>
#include <random>
#include <thread>
#include <vector>

class Maze {

public:
    Maze(const bool &mode);
    ~Maze();

    /*reads user maze from terminal*/
    void readMaze(void);
    /*builds the maze into minecraft*/
    void buildMaze();
    /*generates random maze of specified length/width*/
    void generateMaze(void);
    /*generate maze using recursive backtracking algorithm*/
    void generateMazeAboveTerrain(void);
    /*cleans the world for the next maze to build in minecraft*/
    void cleanWorld(void);

    /*spawns randomly within the maze*/
    void SpawnRandom(void);

    /*checks for isolation in a maze*/
    void markReachable(std::vector<std::vector<char>> &maze,
                       std::vector<std::vector<bool>> &visited, unsigned int x,
                       unsigned int y);
    void checkForIsolation();
    /*Checks for loop in a maze*/
    void checkForLoops();
    // recursive function
    void floodFill(std::vector<std::vector<char>> &copyMaze, unsigned int x,
                   unsigned int y, char oldChar, char newChar);

    /* Function to check for any obstacles within the maze*/
    bool checkForObstacles();
    /*generate random maze through recursive back tracking and walling off
     * obstacles*/
    void generateRandomMazeWithObstacles(void);
    /* recursive back tracking algorithm function for e1*/
    void recursiveBackTrackingAlgorithm(int startrow, int startcolumn);

private:
    /*x y z is the location of the basepoint rows and columns is length and
     * width of maze mode is testing or normal*/
    int rows, columns, x, y, z;
    bool mode, flatLand;
    mcpp::MinecraftConnection mc;

    /*stores all walls*/
    LinkedListTemplate<BlockCoord> wallList;
    /*stores the maze of length rows and columns of maze * for passage x for
     * wall*/
    std::vector<std::vector<char>> maze;

    /*stores blocktype and their coorosponding coordinate in a node*/
    LinkedListTemplate<BlockCoord> blockAndCoord;

    /*recursive division algorithm to generate random maze*/
    void divide(const int &top, const int &left, const int &bottom,
                const int &right, int &counter);
    /*asks user to enter done and stores the coordinates in x,y,z*/
    void isDone(void);
    /*asks user to enter length and width*/
    void enterLandW(void);
    /*asks user to length and witdth of odd pos ints*/
    void enterLandWforRandom(void);
    /*asks user to enter maze from terminal*/
    void enterMazeToTerminal(void);
    /*prints maze info into console*/
    void printMazeInfo(void);
    /*generates a random maze*/
    void generateRandomMaze(void);
    void AddCarpet(int x, int y, int z);
    void buildWithoutClearing();
};

#endif // ASSIGN_MAZE_H
