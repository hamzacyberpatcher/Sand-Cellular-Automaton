#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <SFML/Graphics.hpp>

const int GRIDSIZE = 5;
const int WIN_W = 1200;
const int WIN_H = 700;
const int CELLS_X = WIN_W / GRIDSIZE;
const int CELLS_Y = WIN_H / GRIDSIZE;
const int BRUSHSIZE = 5;
const int FPS = 100;

template <typename T>
T clamp(T value, T low, T high) {
    if (value < low) return low;
    if (value > high) return high;
    return value;
}

struct Color
{
    int r, g, b;
};

struct Cell
{
    Color color;
    bool isSand;
    Cell(Color color = {0, 0, 0}, bool isSand = false) : color(color), isSand(isSand) {};
};

Color hsvToRgb(float h, float s, float v)
{
    float c = v * s;
    float x = c * (1 - std::abs(std::fmod(h / 60.0, 2) - 1));
    float m = v - c;
    float r, g, b;

    if (h < 60) { r = c; g = x; b = 0; }
    else if (h < 120) { r = x; g = c; b = 0; }
    else if (h < 180) { r = 0; g = c; b = x; }
    else if (h < 240) { r = 0; g = x; b = c; }
    else if (h < 300) { r = x; g = 0; b = c; }
    else { r = c; g = 0; b = x; }

    return {
        static_cast<int>((r + m) * 255),
        static_cast<int>((g + m) * 255),
        static_cast<int>((b + m) * 255)
    };
}

class SandPalette {
private:
    float colorCounter = 0;

public:
    Color getNextColor() {
        // Increment the hue slowly (change 0.1 to 1.0 for faster shifts)
        colorCounter += 0.01f;
        if (colorCounter > 360.0f) colorCounter = 0;

        // Get base color from current hue
        Color base = hsvToRgb(colorCounter, 0.8f, 0.9f);

        // Add "Jitter" (noise) so it looks like individual grains
        int jitter = (rand() % 30) - 15; // Random value between -15 and 15
        
        base.r = clamp(base.r + jitter, 0, 255);
        base.g = clamp(base.g + jitter, 0, 255);
        base.b = clamp(base.b + jitter, 0, 255);

        return base;
    }
};

void sandFalling(std::vector<std::vector<Cell>>& grid)
{
    static bool polarityLeft = true;

    for(int i = CELLS_X - 1; i >= 0; i--)
    {
        for(int j = CELLS_Y - 2; j >= 0; j--)
        {
            if (grid[i][j].isSand)
            {
                if (!grid[i][j + 1].isSand)
                {
                    grid[i][j + 1] = grid[i][j];
                    grid[i][j] = {{0, 0, 0}, false};
                }
                else if (polarityLeft)
                {
                    if (i > 0)
                    {
                        if (!grid[i - 1][j + 1].isSand)
                        {
                            grid[i - 1][j + 1] = grid[i][j];
                            grid[i][j] = {{0, 0, 0}, false};
                        }
                        else if (i < CELLS_X - 1 && !grid[i + 1][j + 1].isSand)
                        {
                            grid[i + 1][j + 1] = grid[i][j];
                            grid[i][j] = {{0, 0, 0}, 0};
                        }
                    }
                }
                else
                {
                    if (i < CELLS_X - 1 && !grid[i + 1][j + 1].isSand)
                    {
                        grid[i + 1][j + 1] = grid[i][j];
                        grid[i][j] = {{0, 0, 0}, 0};
                    }
                    else if (i > 0 && !grid[i - 1][j + 1].isSand)
                    {
                        grid[i - 1][j + 1]= grid[i][j];
                        grid[i][j] = {{0, 0, 0}, 0};
                    }
                }
            }
        }
    }

    polarityLeft = !polarityLeft;
}

int main()
{
    sf::RenderWindow window(sf::VideoMode(WIN_W, WIN_H), "Sand Cellular Automaton");
    window.setFramerateLimit(FPS);
    std::vector<std::vector<Cell>> grid(CELLS_X, std::vector<Cell>(CELLS_Y, {{0, 0, 0}, 0}));
    SandPalette pallete;

    while(window.isOpen())
    {
        sf::Event ev;

        while(window.pollEvent(ev))
        {
            if (ev.type == sf::Event::Closed) window.close();
        }

        sf::Vector2f mouseCords(sf::Mouse::getPosition(window).x / GRIDSIZE, sf::Mouse::getPosition(window).y / GRIDSIZE);

        if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
        {
            for(int dx = - BRUSHSIZE / 2; dx <= BRUSHSIZE / 2; dx++)
            {
                for(int dy = -BRUSHSIZE / 2; dy <= BRUSHSIZE / 2; dy++)
                {
                    int x = mouseCords.x + dx;
                    int y = mouseCords.y + dy;

                    if (x < 0 || x >= CELLS_X || y < 0 || y >= CELLS_Y)
                        continue;

                    if (!grid[x][y].isSand)
                    {
                        grid[x][y].isSand = true;
                        grid[x][y].color = pallete.getNextColor();
                    }
                }
            }
        }
        

        window.clear(sf::Color(37, 217, 230));

        sf::RectangleShape curosrBg;

        curosrBg.setFillColor(sf::Color(192, 192, 192));
        curosrBg.setSize(sf::Vector2f(GRIDSIZE, GRIDSIZE));

        for(int dx = - BRUSHSIZE / 2; dx <= BRUSHSIZE / 2; dx++)
        {
            for(int dy = -BRUSHSIZE / 2; dy <= BRUSHSIZE / 2; dy++)
            {
                int x = mouseCords.x + dx;
                int y = mouseCords.y + dy;

                if (x < 0 || x >= CELLS_X || y < 0 || y >= CELLS_Y)
                    continue;

                curosrBg.setPosition(x * GRIDSIZE, y * GRIDSIZE);
                window.draw(curosrBg);
            }
        }


        sf::RectangleShape block;
        block.setSize(sf::Vector2f(GRIDSIZE, GRIDSIZE));
        block.setFillColor(sf::Color(192, 12, 12));

        for(int i = 0; i < CELLS_X; i++)
        {
            for(int j = 0; j < CELLS_Y; j++)
            {
                if (grid[i][j].isSand)
                {
                    block.setPosition(i * GRIDSIZE, j * GRIDSIZE);
                    block.setFillColor(sf::Color(grid[i][j].color.r, grid[i][j].color.g, grid[i][j].color.b));
                    window.draw(block);
                }
            }
        }

        window.display();

        sandFalling(grid);
    }

    return 0;
}