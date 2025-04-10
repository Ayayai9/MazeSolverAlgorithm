## **IMPORTANT:**

**COMPILING OPTIONS**

_OPTION 1 (USING CMAKE AND IN MAIN DIRECTORY)_

STEP 1:

```bash
MAKE CLEAN
```

STEP 2:

```bash
MAKE
```

STEP 3:

```bash
./build/mazeRunner
```

_OPTION 2 (WITHOUT CMAKE AND IN MAIN DIRECTORY)_

STEP 1:

```bash
g++ -Wall -Werror -std=c++17 -g -O -o build/mazeRunner src/Agent.cpp src/mazeRunner.cpp src/Maze.cpp -lmcpp

```

STEP 2:

```bash
./build/mazeRunner
```

## Assignment Overview

This assignment is designed to enhance skills in developing software around a high-level API and apply the C++ programming skills acquired in Bootcamp 2 and Studio 2 to a comprehensive, team-based project. The code contains an “Expansion Pack” for Minecraft that enables users to solve mazes. The code utilises Minecraft's C++ API, "mcpp", to achieve this.

</br>
Group Members:

Devon Katsuk S3844510
Ishita . s4020999
Yunbin Cho s4001725
