#include "../include/Maze.h"

const char WALL = 'x';
const char PASSAGE = '.';
const std::string TELEPORT_COMMAND = "tp 4848 71 4369";

Maze::Maze(const bool &mode) {
    rows = 0;
    columns = 0;
    x = 0;
    y = 0;
    z = 0;
    this->mode = mode;
}
/*clears world when program terminates*/
Maze::~Maze() { cleanWorld(); }
/*read maze from terminal*/
void Maze::readMaze() {
    if (mode) {
        mc.doCommand(TELEPORT_COMMAND);
    }
    isDone();
    enterLandW();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    enterMazeToTerminal();
    checkForIsolation();
    checkForLoops();
    std::cout << "Maze read successfully\n";
    printMazeInfo();
}
/*generate maze using recursive division algorithm*/
void Maze::generateMaze(void) {
    flatLand = true;
    isDone();
    enterLandWforRandom();
    generateRandomMaze();
    std::cout << "Maze generated successfully\n";
    printMazeInfo();
}

/*generate maze using recursive backtracking algorithm*/
void Maze::generateMazeAboveTerrain(void) {
    flatLand = false;
    isDone();
    enterLandWforRandom();
    generateRandomMazeWithObstacles();
    std::cout << "Maze generated successfully\n";
    printMazeInfo();
}

/*builds maze into the minecraft world*/
void Maze::buildMaze() {
    if (!flatLand) {
        buildWithoutClearing();
    } else {
        /*clear world and reset vectors for usage*/
        cleanWorld();

        /*map the 3d location of each block in the maze environment and store
         * the ones that are not air into linkedlist*/
        std::vector<std::vector<std::vector<mcpp::BlockType>>> blocksToStore;
        mcpp::Coordinate loc3D1 = mcpp::Coordinate(x, y, z);
        mcpp::Coordinate loc3D2 =
            mcpp::Coordinate(x + rows - 1, y + 2, z + columns - 1);

        blocksToStore = mc.getBlocks(loc3D1, loc3D2);

        for (unsigned int i = 0; i < blocksToStore.size(); i++) {
            for (unsigned int j = 0; j < blocksToStore[i].size(); j++) {
                for (unsigned int k = 0; k < blocksToStore[i][j].size(); k++) {
                    int l = (int)i;
                    int m = (int)j;
                    int o = (int)k;
                    if (!(blocksToStore[i][j][k] == mcpp::Blocks::AIR)) {
                        blockAndCoord.addToEnd(
                            BlockCoord(blocksToStore[i][j][k],
                                       mcpp::Coordinate(x + m, y + l, z + o)));
                        mc.setBlock(mcpp::Coordinate(x + m, y + l, z + o),
                                    mcpp::Blocks::AIR);
                    }
                }
            }
        }

        /*if block above maze store it and replace with air*/
        std::vector<std::vector<int>> heightsAboveGround;
        mcpp::Coordinate loc1 = mcpp::Coordinate(x, y, z);
        mcpp::Coordinate loc2 = mcpp::Coordinate(x + rows, y, z + columns);
        heightsAboveGround = mc.getHeights(loc1, loc2);
        for (unsigned int i = 0; i < heightsAboveGround.size(); i++) {
            for (unsigned int j = 0; j < heightsAboveGround[i].size(); j++) {
                heightsAboveGround[i][j] = heightsAboveGround[i][j] - y;
            }
        }
        for (unsigned int i = 0; i < heightsAboveGround.size(); i++) {
            for (unsigned int j = 0; j < heightsAboveGround[i].size(); j++) {
                int height = heightsAboveGround[i][j];
                while (height > 2) {
                    mcpp::BlockType block =
                        mc.getBlock(mcpp::Coordinate(x + i, y + height, z + j));
                    if (!(block == mcpp::Blocks::AIR)) {
                        blockAndCoord.addToEnd(BlockCoord(
                            block, mcpp::Coordinate(x + i, y + height, z + j)));
                        mc.setBlock(mcpp::Coordinate(x + i, y + height, z + j),
                                    mcpp::Blocks::AIR);
                    }
                    height--;
                }
            }
        }

        /*if blocks of height -2 is air replace with dirt/grass we use
        getheights aswell because getblocks struggles to recognise grass as
        blocks*/
        std::vector<std::vector<int>> heightsBelowGround;
        loc1 = mcpp::Coordinate(x, y, z);
        loc2 = mcpp::Coordinate(x + rows, y, z + columns);
        heightsBelowGround = mc.getHeights(loc1, loc2);
        for (unsigned int i = 0; i < heightsBelowGround.size(); i++) {
            for (unsigned int j = 0; j < heightsBelowGround[i].size(); j++) {
                /*normalise each height relative to player location*/
                heightsBelowGround[i][j] = heightsBelowGround[i][j] - y + 1;
            }
        }
        /*clears the wall list if a maze has already been made*/
        wallList.clear();

        /*flatten the area and place blue carpet at exit*/
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < columns; j++) {
                if (heightsBelowGround[i][j] == -1) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(50));
                    blockAndCoord.addToEnd(
                        BlockCoord(mcpp::Blocks::AIR,
                                   mcpp::Coordinate(x + i, y - 1, z + j)));
                    mc.setBlock(
                        mcpp::Coordinate(x + i, y - 1, z + j),
                        mc.getBlock(mcpp::Coordinate(x + i, y - 2, z + j)));
                } else if (heightsBelowGround[i][j] < -1) {
                    int height = heightsBelowGround[i][j] - 1;

                    std::this_thread::sleep_for(std::chrono::milliseconds(50));
                    blockAndCoord.addToEnd(
                        BlockCoord(mcpp::Blocks::AIR,
                                   mcpp::Coordinate(x + i, y - 1, z + j)));
                    mc.setBlock(mcpp::Coordinate(x + i, y - 1, z + j),
                                mc.getBlock(mcpp::Coordinate(x + i, y + height,
                                                             z + j)));
                    std::this_thread::sleep_for(std::chrono::milliseconds(50));
                    blockAndCoord.addToEnd(
                        BlockCoord(mcpp::Blocks::AIR,
                                   mcpp::Coordinate(x + i, y - 2, z + j)));
                    mc.setBlock(mcpp::Coordinate(x + i, y - 2, z + j),
                                mc.getBlock(mcpp::Coordinate(x + i, y + height,
                                                             z + j)));
                }

                /*push back walls into the vector to store the coordinates of
                 * walls*/
                if (maze[i][j] == WALL) {
                    wallList.addToEnd(
                        BlockCoord(mcpp::Blocks::ACACIA_WOOD_PLANK,
                                   mcpp::Coordinate(x + i, y, z + j)));
                }

                /*places blue carpet at exit/entrance and places air block one
                top of blue carpet and solid block below blue carpet if it is
                air and stores the coordinates and blocks replaced*/

                /*if the block is on the border of the maze and equals a
                 * PASSAGE*/
                if (maze[i][j] == PASSAGE &&
                    (i == rows - 1 || j == columns - 1 || i == 0 || j == 0)) {
                    if (i == 0) {
                        AddCarpet(x + i - 1, y, z + j);
                    } else if (j == 0) {
                        AddCarpet(x + i, y, z + j - 1);
                    } else if (j == columns - 1) {
                        AddCarpet(x + i, y, z + j + 1);
                    } else if (i == rows - 1) {
                        AddCarpet(x + i + 1, y, z + j);
                    }
                }
            }
        }

        /*place walls with height of 3 and store in total walls for all walls to
         * get cleared when calling*/
        for (int i = 0; i < wallList.getSize(); i++) {
            mcpp::Coordinate wall = wallList.at(i).getCoord2();
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            mc.setBlock(wall, mcpp::Blocks::ACACIA_WOOD_PLANK);
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            mc.setBlock(mcpp::Coordinate(wall.x, wall.y + 1, wall.z),
                        mcpp::Blocks::ACACIA_WOOD_PLANK);
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            mc.setBlock(mcpp::Coordinate(wall.x, wall.y + 2, wall.z),
                        mcpp::Blocks::ACACIA_WOOD_PLANK);
        }
    }
}

void Maze::AddCarpet(int x, int y, int z) {
    if (mc.getBlock(mcpp::Coordinate(x, y - 1, z)) == mcpp::Blocks::AIR) {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        blockAndCoord.addToEnd(
            BlockCoord(mc.getBlock(mcpp::Coordinate(x, y - 1, z)),
                       mcpp::Coordinate(x, y - 1, z)));
        mc.setBlock(mcpp::Coordinate(x, y - 1, z), mcpp::Blocks::GRASS);
    }
    /*set the blue carpet*/
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    blockAndCoord.addToEnd(BlockCoord(mc.getBlock(mcpp::Coordinate(x, y, z)),
                                      mcpp::Coordinate(x, y, z)));
    mc.setBlock(mcpp::Coordinate(x, y, z), mcpp::Blocks::BLUE_CARPET);
    /*replace the block above the blue carpet with air*/
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    blockAndCoord.addToEnd(
        BlockCoord(mc.getBlock(mcpp::Coordinate(x, y + 1, z)),
                   mcpp::Coordinate(x, y + 1, z)));
    mc.setBlock(mcpp::Coordinate(x, y + 1, z), mcpp::Blocks::AIR);
}
void Maze::divide(const int &top, const int &left, const int &bottom,
                  const int &right, int &counter) {
    /*divide function for normal mode*/
    if (!mode) {
        // std::default_random_engine engine(10); // seed
        std::random_device engine;
        /* 0 horizontal 1 vertical */
        std::uniform_int_distribution<int> split(0, 1);
        int horOrVert = split(engine);

        if (horOrVert == 0) {
            std::uniform_int_distribution<int> odd(top / 2 + 1,
                                                   (bottom - 2) / 2);
            int divideRow = odd(engine) * 2 + 1;

            /* Generate a random even number from left to right for creating
             * passage */
            std::uniform_int_distribution<int> even((left + 2) / 2,
                                                    (right - 2) / 2);
            int passageCol = even(engine) * 2;

            /* divide area and create a passage*/
            for (int j = left; j < right - 1; j++) {
                maze[divideRow - 1][j] = WALL;
            }
            maze[divideRow - 1][passageCol - 1] = PASSAGE;

            /*recursively divide the top area if it can be divided more*/
            if (divideRow - top > 3) {
                divide(top, left, divideRow, right, counter);
            }

            /*recursively divide the bottom area if it can be divided more*/
            if (bottom - divideRow > 3) {
                divide(divideRow, left, bottom, right, counter);
            }
        } else {
            /* Generate a random odd number from left to right for splitting the
             * maze */
            std::uniform_int_distribution<int> odd((left + 2) / 2,
                                                   (right - 2) / 2);
            int divideCol = odd(engine) * 2 + 1;

            /* Generate a random even number from top to bottom for creating
             * passage */
            std::uniform_int_distribution<int> even((top + 2) / 2,
                                                    (bottom - 2) / 2);
            int passageRow = even(engine) * 2;

            /* divide area and create passage */
            for (int j = top; j < bottom - 1; j++) {
                maze[j][divideCol - 1] = WALL;
            }
            maze[passageRow - 1][divideCol - 1] = PASSAGE;

            /*recursively divide the left area if it can be divided more*/
            if (divideCol - left > 3) {
                divide(top, left, bottom, divideCol, counter);
            }

            /*recursively divide the right area if it can be divided more*/
            if (right - divideCol > 3) {
                divide(top, divideCol, bottom, right, counter);
            }
        }
    }
    /*divide function for testing mode*/
    else {
        int horOrVert;
        horOrVert = counter;

        if (horOrVert % 2 == 0) {
            counter++;
            /* Calculate the middle odd number from top to bottom for splitting
             * the maze */
            int divideRow = ((top + bottom) / 4) * 2 + 1;

            /* Calculate the middle even number from left to right for creating
             * passage */
            int passageCol = ((left + right) / 4) * 2;

            /* divide area and create a passage*/
            for (int j = left; j < right - 1; j++) {
                maze[divideRow - 1][j] = WALL;
            }
            maze[divideRow - 1][passageCol - 1] = PASSAGE;

            /*recursively divide the top area if it can be divided more*/
            if (divideRow - top > 3) {
                divide(top, left, divideRow, right, counter);
            }

            /*recursively divide the bottom area if it can be divided more*/
            if (bottom - divideRow > 3) {
                divide(divideRow, left, bottom, right, counter);
            }
        } else {
            counter++;
            /* Calculate the middle odd number from left to right for splitting
             * the maze */
            int divideCol = ((left + right) / 4) * 2 + 1;

            /* Calculate the middle even number from top to bottom for creating
             * passage */
            int passageRow = ((top + bottom) / 4) * 2;

            /* divide area and create passage */
            for (int j = top; j < bottom - 1; j++) {
                maze[j][divideCol - 1] = WALL;
            }
            maze[passageRow - 1][divideCol - 1] = PASSAGE;

            /*recursively divide the left area if it can be divided more*/
            if (divideCol - left > 3) {
                divide(top, left, bottom, divideCol, counter);
            }

            /*recursively divide the right area if it can be divided more*/
            if (right - divideCol > 3) {
                divide(top, divideCol, bottom, right, counter);
            }
        }
    }
}

void Maze::SpawnRandom() {
    /*if maze is empty print error message*/
    if (maze.empty()) {
        std::cout << "Maze is empty. Please generate or read a maze first.\n";
    }
    /*if in testing mode spawn to upper left*/
    else if (mode) {
        std::string x1 = std::to_string(x + rows - 2);
        std::string y1 = std::to_string(y);
        std::string z1 = std::to_string(z + columns - 2);
        mc.doCommand("tp " + x1 + " " + y1 + " " + z1);
    } else {
        /* spawns random location in the maze check if the lcoation in the maze
         * is used*/
        std::random_device engine;
        std::uniform_int_distribution<int> randomx(1, columns - 2);
        std::uniform_int_distribution<int> randomz(1, rows - 2);

        int xdir = randomx(engine);
        int zdir = randomz(engine);

        while (maze[xdir][zdir] == WALL) {
            xdir = randomx(engine);
            zdir = randomz(engine);
        }

        xdir += x;
        zdir += z;
        int ydir = y;

        std::string x1 = std::to_string(xdir);
        std::string y1 = std::to_string(ydir);
        std::string z1 = std::to_string(zdir);

        mc.doCommand("tp " + x1 + " " + y1 + " " + z1);
    }
}

void Maze::cleanWorld(void) {
    /*clean world when a new build maze is called*/

    /*sets walls to air*/
    for (int i = 0; i < wallList.getSize(); i++) {
        mcpp::Coordinate wall = wallList.at(i).getCoord2();
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        mc.setBlock(wall, mcpp::Blocks::AIR);
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        mc.setBlock(mcpp::Coordinate(wall.x, wall.y + 1, wall.z),
                    mcpp::Blocks::AIR);
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        mc.setBlock(mcpp::Coordinate(wall.x, wall.y + 2, wall.z),
                    mcpp::Blocks::AIR);
    }
    /*sets stored blocks at their corrosponding coordinates*/
    for (int i = 0; i < blockAndCoord.getSize(); i++) {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        mc.setBlock(blockAndCoord.at(i).getCoord2(),
                    blockAndCoord.at(i).getBlock2());
    }
}

void Maze::isDone(void) {
    std::string done;
    bool validDone = false;
    std::cout << "In minecraft navigate to where you need the maze\nto be "
                 "build and type - done\n";
    while (!validDone) {
        std::cin >> done;

        if (std::cin.eof()) {
            std::cout << "Input ended unexpectedly. Exiting...\n";
            exit(1);
        } else if (std::cin.fail() || done != "done") {
            std::cout << "Invalid input. Please enter 'done' to continue: \n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        } else {
            validDone = true;
        }
    }
    /*get the coords of the player when entered done*/
    mcpp::Coordinate playerLocation = mc.getPlayerPosition();
    x = playerLocation.x;
    z = playerLocation.z;
    y = mc.getHeight(x, z) + 1;
}

void Maze::enterLandW() {
    std::cout << "Enter the length and width of the maze to build:\n";
    bool validLenWid = false;
    while (!validLenWid) {
        std::cin >> rows >> columns;

        if (std::cin.eof()) {
            std::cout << "Input ended unexpectedly. Exiting...\n";
            exit(1);
        } else if (std::cin.fail() || rows <= 0 || columns <= 0) {
            std::cout << "Invalid input. Please enter two positive integers "
                         "for the length and width: \n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        } else {
            validLenWid = true;
        }
    }
}
void Maze::enterLandWforRandom() {
    std::cout << "Enter the length and width of the maze to build:\n";
    bool validLenWid = false;
    while (!validLenWid) {
        std::cin >> rows >> columns;

        if (std::cin.eof()) {
            std::cout << "Input ended unexpectedly. Exiting...\n";
            exit(1);
        } else if (std::cin.fail() || rows <= 4 || columns <= 4) {
            std::cout << "Invalid input. Please enter two integers greater "
                         "than 4 for the length and width: \n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        } else if (rows % 2 == 0 || columns % 2 == 0) {
            std::cout << "Invalid input. Please enter two odd integers for the "
                         "length and width: \n";
        } else {
            validLenWid = true;
        }
    }
}
void Maze::enterMazeToTerminal(void) {
    maze.clear();
    std::cout << "Enter the maze structure (use 'x' for wall and '.' for empty "
                 "space):\n";
    for (int i = 0; i < rows; i++) {
        std::vector<char> row;
        std::string line;
        bool validMazeTerminal = false;

        while (!validMazeTerminal) {
            int counter = 0;
            std::getline(std::cin, line);
            for (unsigned int j = 0; j < line.length(); j++) {
                if (line[j] == WALL || line[j] == PASSAGE) {
                    counter++;
                }
            }
            if (std::cin.eof()) {
                std::cout << "Input ended unexpectedly. Exiting...\n";
                exit(1);
            } else if ((int)line.length() != columns) {
                std::cout << "Invalid input. Please enter a row of length "
                          << columns << ": \n";
            } else if (counter != columns) {
                std::cout << "Invalid input. Please enter only char 'x' or '.' "
                             "in the row\n";
            } else {
                validMazeTerminal = true;
            }
        }
        for (char c : line) {
            row.push_back(c);
        }
        maze.push_back(row);
    }
}

void Maze::printMazeInfo(void) {
    std::cout << "**Printing Maze**\n";
    std::cout << "BasePoint: (" << x << " " << y << " " << z << ")\n";
    std::cout << "Structure\n";
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < columns; ++j) {
            std::cout << maze[i][j];
        }
        std::cout << std::endl;
    }
    std::cout << "**End Printing Maze**\n";
}

void Maze::generateRandomMaze(void) {
    maze.clear();
    // Step 1: Initialize the maze with only the outer boundary walls
    for (int i = 0; i < rows; ++i) {
        std::vector<char> row;
        for (int j = 0; j < columns; ++j) {
            if (i == 0 || j == 0 || i == rows - 1 || j == columns - 1) {
                row.push_back(WALL);
            } else {
                row.push_back(PASSAGE);
            }
        }
        maze.push_back(row);
    }
    /*counter is used to swtich between horizontal and vertical
    when in testing mode*/
    int counter = 0;
    // Call the divide function for the whole maze
    divide(1, 1, rows, columns, counter);

    // Create an  exit
    std::vector<int> evenNumbersRows;
    for (int i = 2; i < rows; i++) {
        if (i % 2 == 0) {
            evenNumbersRows.push_back(i);
        }
    }
    std::vector<int> evenNumbersColumns;
    for (int i = 2; i < columns; i++) {
        if (i % 2 == 0) {
            evenNumbersColumns.push_back(i);
        }
    }
    std::random_device engine;
    std::uniform_int_distribution<int> exitHorOrVer(0, 1);
    std::uniform_int_distribution<int> exitWhichSide(0, 1);
    std::uniform_int_distribution<int> createExitVert(
        0, evenNumbersRows.size() - 1);
    std::uniform_int_distribution<int> createExitHor(0, evenNumbersRows.size() -
                                                            1);
    int horOrVert = exitHorOrVer(engine);
    int whichSide = exitWhichSide(engine);
    /*create passage horizontally or vertially*/
    if (mode) {
        maze[0][1] = PASSAGE;
    } else {
        if (horOrVert == 0) {
            /*create passage up or down*/
            if (whichSide == 0) {
                int passage = evenNumbersColumns[createExitHor(engine)];
                maze[0][passage - 1] = PASSAGE;
            } else {
                int passage = evenNumbersColumns[createExitHor(engine)];
                maze[rows - 1][passage - 1] = PASSAGE;
            }
        } else {
            /*create passage left or right*/
            if (whichSide == 0) {
                int passage = evenNumbersRows[createExitVert(engine)];
                maze[passage - 1][0] = PASSAGE;
            } else {
                int passage = evenNumbersRows[createExitVert(engine)];
                maze[passage - 1][columns - 1] = PASSAGE;
            }
        }
    }
}

void Maze::checkForIsolation() {
    // Create a 2D vector to keep track of visited cells

    bool isolationexists = true;
    bool neededToFix = false;
    bool printstatement = false;
    int count = 0;
    while (isolationexists) {

        neededToFix = false;
        std::vector<std::vector<bool>> visited(
            maze.size(), std::vector<bool>(maze[0].size(), false));

        // Mark reachable cells from the border
        for (unsigned int i = 0; i < maze.size(); i++) {
            for (unsigned int j = 0; j < maze[0].size(); j++) {
                // If the cell is on the border and is reachable, mark it as
                // visited
                if ((i == 0 || j == 0 || i == maze.size() - 1 ||
                     j == maze[0].size() - 1) &&
                    maze[i][j] == '.') {
                    markReachable(maze, visited, i, j);
                }
            }
        }

        // Check for isolated sections and fix them if found

        for (unsigned int i = 1; i < maze.size() - 1 && !neededToFix; i++) {
            for (unsigned int j = 1; j < maze[0].size() - 1 && !neededToFix;
                 j++) {
                // If a cell is '.' and not visited, it is part of an isolated
                // section
                if (maze[i][j] == '.' && !visited[i][j]) {
                    // Mark the isolated cell as visited
                    visited[i][j] = true;

                    // Check and adjust the cell below the isolated cell if it
                    // is within bounds and not on the boundary
                    if (i + 1 < maze.size() - 1 && maze[i + 1][j] == 'x') {
                        maze[i + 1][j] = '.';

                        isolationexists = true;
                        printstatement = true;
                    }
                    // Check and adjust the cell above the isolated cell if it
                    // is within bounds and not on the boundary
                    else if (i - 1 > 0 && maze[i - 1][j] == 'x') {
                        maze[i - 1][j] = '.';

                        isolationexists = true;
                        printstatement = true;
                    }
                    // Check and adjust the cell to the right of the isolated
                    // cell if it is within bounds and not on the boundary
                    else if (j + 1 < maze[0].size() - 1 &&
                             maze[i][j + 1] == 'x') {
                        maze[i][j + 1] = '.';

                        isolationexists = true;
                        printstatement = true;
                    }
                    // Check and adjust the cell to the left of the isolated
                    // cell if it is within bounds and not on the boundary
                    else if (j - 1 > 0 && maze[i][j - 1] == 'x') {
                        maze[i][j - 1] = '.';

                        isolationexists = true;
                        printstatement = true;
                    }
                    neededToFix = true;
                    count++;
                } else {
                    isolationexists = false;
                }
            }
        }
    }
    // Output a message if isolated sections were fixed based on my many
    // isolations were there
    if (printstatement) {
        if (count == 1) {
            std::cout << "fixed the isolation in the maze.\n";
        } else {
            std::cout << "fixed " << count << " isolations in the maze.\n";
        }
    }
}

// Recursive helper function to mark reachable cells from a given cell
void Maze::markReachable(std::vector<std::vector<char>> &maze,
                         std::vector<std::vector<bool>> &visited,
                         unsigned int x, unsigned int y) {
    // Base case: if cell is out of bounds, not a '.', or already visited,
    // return
    if (x < 0 || x >= maze.size() || y < 0 || y >= maze[0].size() ||
        maze[x][y] != '.' || visited[x][y]) {
        return;
    }

    // Mark the cell as visited
    visited[x][y] = true;

    // Recursively mark reachable cells in all four directions
    markReachable(maze, visited, x + 1, y);
    markReachable(maze, visited, x - 1, y);
    markReachable(maze, visited, x, y + 1);
    markReachable(maze, visited, x, y - 1);
}

void Maze::checkForLoops() {
    bool neededToFixLoop = false;
    bool loopExists = true;
    while (loopExists) {
        std::vector<std::vector<char>> mazeCopy = maze;
        unsigned int xdir = 1;
        unsigned int zdir = 1;
        for (unsigned int j = 0; j < mazeCopy[0].size(); j++) {
            if (mazeCopy[0][j] == 'x') {
                xdir = 0;
                zdir = j;
            }
        }

        floodFill(mazeCopy, xdir, zdir, 'x', '*');
        int isolatedx = -1;
        int isolatedz = -1;
        for (unsigned int i = 0; i < mazeCopy.size(); i++) {
            for (unsigned int j = 0; j < mazeCopy[i].size(); j++) {
                if (mazeCopy[i][j] == 'x') {
                    isolatedx = i;
                    isolatedz = j;
                }
            }
        }

        if (isolatedx != -1 && isolatedz != -1) {
            if (maze[isolatedx + 2][isolatedz] == 'x') {
                maze[isolatedx + 1][isolatedz] = 'x';
            } else {
                maze[isolatedx][isolatedz + 1] = 'x';
            }

            neededToFixLoop = true;
        } else {
            loopExists = false;
        }
    }
    if (neededToFixLoop) {
        std::cout << "fixed the isolated loop" << std::endl;
    }
}

void Maze::floodFill(std::vector<std::vector<char>> &copyMaze, unsigned int x,
                     unsigned int y, char oldChar, char newChar) {
    // Base cases
    if (x >= 0 && x < copyMaze.size() && y >= 0 && y < copyMaze[0].size() &&
        copyMaze[x][y] == oldChar) {
        // Replace the old character at (x, y) with new character
        copyMaze[x][y] = newChar;

        // Recur for all four direction
        floodFill(copyMaze, x + 1, y, oldChar, newChar);
        floodFill(copyMaze, x - 1, y, oldChar, newChar);
        floodFill(copyMaze, x, y + 1, oldChar, newChar);
        floodFill(copyMaze, x, y - 1, oldChar, newChar);
    }
}

/* Function to check for any obstacles within the maze and putting a wall there
 * ('x')*/
bool Maze::checkForObstacles() {
    bool hasObstacle = false;
    std::vector<std::vector<int>> heightsBelowGround;
    mcpp::Coordinate loc1 = mcpp::Coordinate(x, y, z);
    mcpp::Coordinate loc2 = mcpp::Coordinate(x + rows, y, z + columns);
    heightsBelowGround = mc.getHeights(loc1, loc2);
    for (unsigned int i = 0; i < heightsBelowGround.size(); i++) {
        for (unsigned int j = 0; j < heightsBelowGround[i].size(); j++) {
            /*normalise each height relative to player locationhhhh*/
            heightsBelowGround[i][j] = heightsBelowGround[i][j] - y + 1;
        }
    }
    for (unsigned int i = 0; i < heightsBelowGround.size(); i++) {
        for (unsigned int j = 0; j < heightsBelowGround[i].size(); j++) {
            /*normalise each height relative to player location*/
            if (heightsBelowGround[i][j] > 1 || heightsBelowGround[i][j] < -1) {
                hasObstacle = true;
                int k = (int)i;
                int l = (int)j;
                /* if boolean is true, place 'o' (obstacle) at that position */
                maze[k][l] = 'o';
            }
        }
    }
    return hasObstacle;
}
/*generate random maze through recursive back tracking and walling off
 * obstacles*/
void Maze::generateRandomMazeWithObstacles(void) {
    maze.clear();
    /*boolean value to hold whether wall is a consequetive wall or x x x...*/
    bool rowOfx;
    for (int i = 0; i < rows; i++) {
        if (i % 2 == 0) {
            rowOfx = true;
        } else {
            rowOfx = false;
        }
        std::vector<char> row;
        for (int j = 0; j < columns; j++) {
            if (rowOfx) {
                row.push_back('x');
            }
            /*accounts for the end of the columm to make sure there isn't an
               additional space after x */
            else if (!rowOfx && j == columns - 1) {
                row.push_back('x');
                /* otherwise, create wall of 'x x x...' */
            } else {
                row.push_back('x');
                row.push_back(' ');
                j++;
            }
        }
        maze.push_back(row);
    }
    checkForObstacles();

    /* std;;default_random_engine engine(10); seed*/
    std::random_device engine;
    /* 0 horizontal 1 vertical */
    bool avaliableEntrance = false;
    int entrance;
    while (!avaliableEntrance) {
        /* Generate a random even number*/
        std::uniform_int_distribution<int> split(1, rows / 2);
        int random = split(engine);
        entrance = random * 2 - 1;
        if (maze[entrance][1] == ' ') {
            maze[entrance][0] = '.';
            avaliableEntrance = true;
        }
    }
    /* use recursive back tracking to form maze starting from position
     * (extrance, 1) */
    recursiveBackTrackingAlgorithm(entrance, 1);
}

/* recursive back tracking algorithm function for e1*/
void Maze::recursiveBackTrackingAlgorithm(int startrow, int startcolumn) {
    maze[startrow][startcolumn] = '.';
    std::vector<std::string> clear;
    /* creating directions that a passage can be cleared */
    if (startrow - 2 > 0 && maze[startrow - 2][startcolumn] == ' ') {
        clear.push_back("up");
    }
    if (startcolumn + 2 < columns - 1 &&
        maze[startrow][startcolumn + 2] == ' ') {
        clear.push_back("right");
    }
    if (startrow + 2 < rows - 1 && maze[startrow + 2][startcolumn] == ' ') {
        clear.push_back("down");
    }

    for (const auto &direction : clear) {
        if (direction == "up" && maze[startrow - 2][startcolumn] == ' ') {
            maze[startrow - 1][startcolumn] = '.';
            maze[startrow - 2][startcolumn] = '.';
            recursiveBackTrackingAlgorithm(startrow - 2, startcolumn);
        } else if (direction == "right" &&
                   maze[startrow][startcolumn + 2] == ' ') {
            maze[startrow][startcolumn + 1] = '.';
            maze[startrow][startcolumn + 2] = '.';
            recursiveBackTrackingAlgorithm(startrow, startcolumn + 2);
        } else if (direction == "down" &&
                   maze[startrow + 2][startcolumn] == ' ') {
            maze[startrow + 1][startcolumn] = '.';
            maze[startrow + 2][startcolumn] = '.';
            recursiveBackTrackingAlgorithm(startrow + 2, startcolumn);
        }
    }

    if (startcolumn - 2 > 0 && maze[startrow][startcolumn - 2] == ' ') {
        maze[startrow][startcolumn - 1] = '.';
        maze[startrow][startcolumn - 2] = '.';
        recursiveBackTrackingAlgorithm(startrow, startcolumn - 2);
    }

    return;
}

void Maze::buildWithoutClearing() {
    /*clear world and reset vectors for usage*/
    cleanWorld();

    /*clears the wall list if a maze has already been made*/
    wallList.clear();

    /*place blue carpet at exit*/
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < columns; j++) {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            blockAndCoord.addToEnd(BlockCoord(
                mcpp::Blocks::AIR, mcpp::Coordinate(x + i, y, z + j)));
            mc.setBlock(mcpp::Coordinate(x + i, y, z + j),
                        mc.getBlock(mcpp::Coordinate(x + i, y, z + j)));

            /*push back walls into the vector to store the coordinates of
             * walls*/
            if (maze[i][j] == WALL) {
                wallList.addToEnd(
                    BlockCoord(mcpp::Blocks::ACACIA_WOOD_PLANK,
                               mcpp::Coordinate(x + i, y, z + j)));
            }

            /*places blue carpet at exit/entrance and places air block one
            top of blue carpet and solid block below blue carpet if it is
            air and stores the coordinates and blocks replaced*/

            /*if the block is on the border of the maze and equals a
             * PASSAGE*/
            if (maze[i][j] == PASSAGE &&
                (i == rows - 1 || j == columns - 1 || i == 0 || j == 0)) {
                if (i == 0) {
                    AddCarpet(x + i - 1, y, z + j);
                } else if (j == 0) {
                    AddCarpet(x + i, y, z + j - 1);
                } else if (j == columns - 1) {
                    AddCarpet(x + i, y, z + j + 1);
                } else if (i == rows - 1) {
                    AddCarpet(x + i + 1, y, z + j);
                }
            }
        }
    }

    /*place walls with height of 3 and store in total walls for all walls to
     * get cleared when calling*/
    for (int i = 0; i < wallList.getSize(); i++) {
        mcpp::Coordinate wall = wallList.at(i).getCoord2();
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        mc.setBlock(wall, mcpp::Blocks::ACACIA_WOOD_PLANK);
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        mc.setBlock(mcpp::Coordinate(wall.x, wall.y + 1, wall.z),
                    mcpp::Blocks::ACACIA_WOOD_PLANK);
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        mc.setBlock(mcpp::Coordinate(wall.x, wall.y + 2, wall.z),
                    mcpp::Blocks::ACACIA_WOOD_PLANK);
    }
}
