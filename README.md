# Sand Cellular Automaton

A real-time falling sand simulation built with **C++** and **SFML**. This project uses cellular automata logic to simulate the physics of granular materials.

![sand](./screenshot/sand.png)

## 🚀 Getting Started

### Prerequisites

You will need a C++ compiler and the SFML library installed.
* **Linux (Ubuntu/Debian):** `sudo apt-get install libsfml-dev`
* **macOS:** `brew install sfml`

### Compilation

To compile the project, run the following command in your terminal. Note that the order of the flags is important for the linker to work correctly:

```bash
g++ main.cpp -o sand -lsfml-graphics -lsfml-window -lsfml-audio -lsfml-system