#include <iostream>
void printStartText(void) {
    std::cout << std::endl;
    std::cout << "Welcome to MineCraft MazeRunner!" << std::endl;
    std::cout << "--------------------------------" << std::endl;
}

void printMainMenu(void) {
    std::cout << std::endl;
    std::cout << "------------- MAIN MENU -------------" << std::endl;
    std::cout << "1) Create a Maze" << std::endl;
    std::cout << "2) Build Maze in MineCraft" << std::endl;
    std::cout << "3) Solve Maze" << std::endl;
    std::cout << "4) Show Team Information" << std::endl;
    std::cout << "5) Exit" << std::endl;
    std::cout << std::endl;
    std::cout << "Enter Menu item to continue: " << std::endl;
}

void printGenerateMazeMenu(void) {
    std::cout << std::endl;
    std::cout << "------------- CREATE A MAZE -------------" << std::endl;
    std::cout << "1) Read Maze from terminal" << std::endl;
    std::cout << "2) Generate Random Maze With Clearing Environment"
              << std::endl;
    std::cout << "3) Generate Random Maze Without Clearing Environment"
              << std::endl;
    std::cout << "4) Back" << std::endl;
    std::cout << std::endl;
    std::cout << "Enter Menu item to continue: " << std::endl;
}

void printSolveMazeMenu(void) {
    std::cout << std::endl;
    std::cout << "------------- SOLVE MAZE -------------" << std::endl;
    std::cout << "1) Solve Manually" << std::endl;
    std::cout << "2) Show Escape Route" << std::endl;
    std::cout << "3) Back" << std::endl;
    std::cout << std::endl;
    std::cout << "Enter Menu item to continue: " << std::endl;
}

void printSolveAlgorithm(void) {
    std::cout << std::endl;
    std::cout << "------------- SOLVE Algorithm -------------" << std::endl;
    std::cout << "1) Wallflower (left hand rule)" << std::endl;
    std::cout << "2) Breadth first search" << std::endl;
    std::cout << "3) Back" << std::endl;
    std::cout << std::endl;
    std::cout << "Enter Menu item to continue: " << std::endl;
}

void printTeamInfo(void) {
    std::cout << std::endl;
    std::cout << "Team members:" << std::endl;
    std::cout << "\t [1] Devon Katsuk (s3844510@Student.rmit.edu.au)"
              << std::endl;
    std::cout << "\t [2] Ishita . (s4020999@student.rmit.edu.au)" << std::endl;
    std::cout << "\t [3] Yunie Cho (s4001725@student.rmit.edu.au)" << std::endl;

    std::cout << std::endl;
}

void printExitMassage(void) {
    std::cout << std::endl;
    std::cout << "The End!" << std::endl;
    std::cout << std::endl;
}