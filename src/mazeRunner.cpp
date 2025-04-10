#include "../include/Agent.h"
#include "../include/Maze.h"
#include "../include/menuUtils.h"

#define NORMAL_MODE 0
#define TESTING_MODE 1

enum States {
    ST_Main,
    ST_GetMaze,
    ST_SolveMaze,
    ST_Creators,
    ST_Exit,
    ST_Algorithm
};

int main(int argc, char *argv[]) {
    // read mode from command line
    bool mode = NORMAL_MODE;
    if (argc > 1 && std::string(argv[1]) == "-testmode") {
        mode = TESTING_MODE;
    }

    Maze maze{mode};
    Agent agent{mode};

    mcpp::MinecraftConnection mc;
    mc.doCommand("time set day");
    mc.doCommand("weather clear");

    // mcpp::Coordinate loc1 = mcpp::Coordinate(1, 1, 1);
    // mcpp::Coordinate loc2 = mcpp::Coordinate(1 + 31, 1, 1 + 31);
    // std::vector<std::vector<int>> heights = mc.getHeights(loc1, loc2);

    States curState = ST_Main;
    printStartText();
    int choice;

    while (curState != ST_Exit) {
        if (curState == ST_Main) {
            printMainMenu();
            std::cin >> choice;
            if (choice == 1) {
                curState = ST_GetMaze;
            } else if (choice == 2) {
                maze.buildMaze();

            } else if (choice == 3) {
                curState = ST_SolveMaze;
            } else if (choice == 4) {
                curState = ST_Creators;
            } else if (choice == 5) {
                curState = ST_Exit;
            } else if (std::cin.eof()) {
                std::cout << "Input ended unexpectedly. Exiting...\n";
                exit(1);
            } else {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(),
                                '\n');
                std::cout << "Input Error: Enter a number between 1 and 5 ..."
                          << std::endl;
            }
        } else if (curState == ST_GetMaze) {
            printGenerateMazeMenu();
            std::cin >> choice;
            if (choice == 1) {

                maze.readMaze();
                curState = ST_Main;
            } else if (choice == 2) {
                maze.generateMaze();
                curState = ST_Main;
            } else if (choice == 3) {
                maze.generateMazeAboveTerrain();
                curState = ST_Main;
            } else if (choice == 4) {
                curState = ST_Main;
            } else if (std::cin.eof()) {
                std::cout << "Input ended unexpectedly. Exiting...\n";
                exit(1);
            } else {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(),
                                '\n');
                std::cout << "Input Error: Enter a number between 1 and 3 ..."
                          << std::endl;
            }
        } else if (curState == ST_SolveMaze) {
            printSolveMazeMenu();
            std::cin >> choice;
            if (choice == 1) {
                maze.SpawnRandom();
                curState = ST_Main;
            } else if (choice == 2) {
                curState = ST_Algorithm;
            } else if (choice == 3) {
                curState = ST_Main;
            } else if (std::cin.eof()) {
                std::cout << "Input ended unexpectedly. Exiting...\n";
                exit(1);
            } else {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(),
                                '\n');
                std::cout << "Input Error: Enter a number between 1 and 3 ..."
                          << std::endl;
            }
        } else if (curState == ST_Algorithm) {
            printSolveAlgorithm();
            std::cin >> choice;
            if (choice == 1) {
                agent.SolveMaze();
                curState = ST_Main;
            } else if (choice == 2) {
                agent.BreadthFirstSearch();
                curState = ST_Main;
            } else if (choice == 3) {
                curState = ST_SolveMaze;
            } else if (std::cin.eof()) {
                std::cout << "Input ended unexpectedly. Exiting...\n";
                exit(1);
            } else {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(),
                                '\n');
                std::cout << "Input Error: Enter a number between 1 and 3 ..."
                          << std::endl;
            }
        } else if (curState == ST_Creators) {
            printTeamInfo();
            curState = ST_Main;
        } else if (std::cin.eof()) {
            std::cout << "Input ended unexpectedly. Exiting...\n";
            exit(1);
        } else {
            std::cout << "Invalid state!" << std::endl;
        }
    }

    printExitMassage();

    return EXIT_SUCCESS;
}