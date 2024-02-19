#include <SFML/Graphics.hpp>
#include <iostream>
#include "Element.h"

const int gridY = 150;
const int gridX = 280;
const int cellSize = 4;
const int defaultBrushSize = 20;
const int staticBrushSize = 3;
const int eraserBrushSize = 15;
const int waterFlowRate = 20;

// smaller number -> faster rate of generator cells
const int faucetSpeed = 1;     
const int voidSpeed = 1;


const int AIR = 0;
const int SAND = 1;
const int WATER = 2;
const int WOOD = 3;
const int VIRUS = 4;
const int VOID = 5;
const int FAUCET = 6;
const int DINOPILL = 7;
const int DINOFLESH = 8;

Element grid[gridY][gridX];
const int neighbors[8][2] = {{0, 1}, {1, 0}, {1, 1}, {0, -1}, {-1, 0}, {-1, -1}, {1, -1}, {-1, 1}};

void drawGrid(sf::RenderWindow &window)
{
    window.clear();

    sf::RectangleShape cell(sf::Vector2f(cellSize, cellSize));

    for (int i = gridY; i >= 0; i--)
    {
        for (int j = 0; j < gridX; j++)
        {
            cell.setPosition(j * cellSize, i * cellSize);
            int randColorOffset;
            Element currentCell = grid[i][j];
            sf::Color color = currentCell.getColor();
            cell.setFillColor(color);

            window.draw(cell);
        }
    }
    window.display();
}

void clearGrid()
{
    for (int i = 0; i < gridY; i++)
    {
        for (int j = 0; j < gridX; j++)
        {
            grid[i][j].setElement(AIR);
        }
    }
}

void swapElements(int i1, int j1, int i2, int j2)
{
    if (i1 == i2 && j1 == j2)
    {
        return;
    }
    Element temp = grid[i1][j1];
    grid[i1][j1] = grid[i2][j2];
    grid[i2][j2] = temp;
}

bool inBounds(int i, int j)
{
    return i >= 0 && i < gridY && j < gridX && j >= 0;
}
bool inBoundsX(int j)
{
    return j < gridX && j >= 0;
}

void moveDown(int i, int j)
{
    if(i < gridY)
    {
        swapElements(i, j, i + 1, j);
    }
}

void fallAndScatter(int i, int j)
{
    int randScatterChance = rand() % 5 == 1;
    int randScatterDir = (rand() % 3) - 1;
    Element target = grid[i + 1][j + randScatterDir];

    if(randScatterChance && inBoundsX(j + randScatterDir) && target.isElement(AIR))
    {
        swapElements(i, j, i + 1, j + randScatterDir);
    }
    else if(grid[i + 1][j].isElement(AIR))
    {
        moveDown(i, j);
    }
}

void moveDownRight(int i, int j)
{
    swapElements(i, j, i + 1, j + 1);
}

void moveDownLeft(int i, int j)
{
    swapElements(i, j, i + 1, j - 1);
}


void updateSand(int i, int j)
{
    if(i >= gridY || i + 1 >= gridY){
        return;
    }
    fallAndScatter(i, j);

    if(grid[i + 1][j].isElement(WATER)){
        moveDown(i, j);
    }

    int randDir = (rand() % 2 == 0) ? -1 : 1;
    if (inBoundsX(j + randDir) && grid[i + 1][j + randDir].isElement(AIR)){
        swapElements(i, j, i + 1, j + randDir);
    }
}

// Water flow
void flowRight(int i, int j)
{
    for(int k = 1; k < waterFlowRate; k++)
    {
        if (j + k >= gridX || k == waterFlowRate - 1 || grid[i][j + k].isSolid() || grid[i + 1][j + k].isElement(AIR))
        {
            return;
        }
        swapElements(i, j, i, j + k);
    }
}
void flowLeft(int i, int j)
{
    for(int k = 1; k < waterFlowRate; k++)
    {
        if (j - k <= 0 || k == waterFlowRate - 1 || grid[i][j - k].isSolid() || grid[i + 1][j - k].isElement(AIR))
        {
            return;
        }
        swapElements(i, j, i, j - k);
    }
}

void updateWater(int i, int j)
{
    if(i + 1 >= gridY)
    {
        return;
    }

    if(grid[i + 1][j].isElement(AIR)){
        fallAndScatter(i, j);
    }
    else{
        if (j + 1 < gridX && grid[i + 1][j + 1].isElement(AIR))
        {
            moveDownRight(i, j);
        }
        else if (j - 1 >= 0 && grid[i + 1][j - 1].isElement(AIR))
        {
            moveDownLeft(i, j);
        }

        if (j + 1 < gridX && rand() % 2 == 0)
        { // Random chance to flow left or right
            flowLeft(i, j);
        }
        else if (j - 1 >= 0)
        {
            flowRight(i, j);
        }
    }
}

// Update method for VIRUS
void updateVirus(int i, int j)
{
    int randomNeighborChoice = rand() % 5;
    int randomNeighborY = i + neighbors[randomNeighborChoice][0];
    int randomNeighborX = j + neighbors[randomNeighborChoice][1];
    Element virusNeighbor = grid[randomNeighborY][randomNeighborX];
    if (inBounds(randomNeighborY, randomNeighborX) && !virusNeighbor.isElement(AIR) && !virusNeighbor.isElement(VIRUS))
    {
        grid[randomNeighborY][randomNeighborX].setElement(VIRUS);
    }
}

// Loop through possible neighbors and set them to an element
void setNeighbors(int i, int j, int elementValue, int ignoreElementValue, int speed)
{
    for (auto neighbor : neighbors)
    {
        int neighborY = i + neighbor[0];
        int neighborX = j + neighbor[1];
        if (!inBounds(neighborY, neighborX) || grid[neighborY][neighborX].isElement(ignoreElementValue) || grid[neighborY][neighborX].isSolid())
        {
            continue;
        }
        if(rand() % speed == 0){
            grid[neighborY][neighborX].setElement(elementValue);
        }    
    }
}

void updateVoid(int i, int j)
{
    setNeighbors(i, j, AIR, VOID, voidSpeed);
}

void updateFaucet(int i, int j)
{
    setNeighbors(i, j, WATER, FAUCET, faucetSpeed);
}

void buildDino(int i, int j){
    bool mirrorArray = false;
    int dinoCells[80][2] = {{-11, -1}, {-11, -2}, {-11, -3}, {-10, 0}, {-10, -1}, {-10, -2}, {-10, -3}, {-10, -4}, {-10, -5}, {-9, 0}, {-9, -1}, {-9, -3}, {-9, -4}, {-9, -5}, {-9, -6}, {-8, 1}, {-8, 0}, {-8, -1}, {-8, -2}, {-8, -3}, {-8, -4}, {-8, -5}, {-8, -6}, {-7, 2}, {-7, 1}, {-7, 0}, {-7, -1}, {-7, -2}, {-7, -3}, {-7, -4}, {-7, -5}, {-6, 2}, {-6, 1}, {-6, 0}, {-6, -1}, {-6, -2}, {-5, 7}, {-5, 3}, {-5, 2}, {-5, 1}, {-5, 0}, {-5, -1}, {-5, -2}, {-4, 7}, {-4, 6}, {-4, 5}, {-4, 4}, {-4, 3}, {-4, 2}, {-4, 1}, {-4, 0}, {-4, -1}, {-4, -2}, {-4, -3}, {-4, -4}, {-3, 6}, {-3, 5}, {-3, 4}, {-3, 3}, {-3, 2}, {-3, 1}, {-3, 0}, {-3, -1}, {-3, -2}, {-2, 3}, {-2, 2}, {-2, 1}, {-2, 0}, {-2, -1}, {-1, 2}, {-1, 1}, {-1, 0}, {-1, -1}, {-1, -2}, {0, 2}, {0, 1}, {0, 0}, {0, -1}, {0, -2}, {0, -3}};
    if(rand() % 2 == 0){
        mirrorArray = true;  // Chance to mirror the array and have the dino face a differrent direction
    }
    for(auto dinoCell : dinoCells){
        int dinoY = dinoCell[0];
        int dinoX = dinoCell[1];
        if(mirrorArray){
            dinoX = -dinoX;
        }
        if(i + dinoY >= 0 && j + dinoX < gridX && j + dinoX >= 0 && (grid[i + dinoY][j + dinoX].isElement(DINOPILL) || !grid[i + dinoY][j + dinoX].isSolid())){
            grid[i + dinoY][j + dinoX].setElement(DINOFLESH);
        }

    }
}

void updateDinoPill(int i, int j)
{
    if(j < 0 || j >= gridX){
        return;
    }

    if(i + 1 < gridY && (grid[i + 1][j].isElement(AIR) || grid[i + 1][j].isElement(WATER))){
        moveDown(i, j);
    }
    else if(grid[i - 1][j].isElement(WATER) && (grid[i + 1][j].isSolid() || i <= gridY)){
        buildDino(i, j);
    }
}



void updateGrid(){
    for (int i = gridY - 1; i >= 0; i--)
    {
        for (int j = 0; j < gridX; j++)
        {
            if (grid[i][j].isElement(AIR))
            {
                continue;
            }
            Element currentCell = grid[i][j];

            if (currentCell.isElement(SAND))
            {
                updateSand(i, j);
            }
            else if (currentCell.isElement(WATER))
            {
                updateWater(i, j);   
            }
            else if (currentCell.isElement(VIRUS))
            {
                updateVirus(i, j);
            }
            else if (currentCell.isElement(VOID))
            {
                updateVoid(i, j);
            }
            else if (currentCell.isElement(FAUCET))
            {
                updateFaucet(i, j);
            }
            else if (currentCell.isElement(DINOPILL))
            {
                updateDinoPill(i, j);
            }
        }
    }
}

void mousePositionToCell(sf::RenderWindow &window, int elementValue, int brushSize)
{
    sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
    if (mousePosition.x >= 0 && mousePosition.x < window.getSize().x &&
        mousePosition.y >= 0 && mousePosition.y < window.getSize().y)
    {

        int cellX = (mousePosition.x / cellSize);
        int cellY = (mousePosition.y / cellSize);

        int brushRadius = brushSize / 2;

        // Iterate through range of brush size
        for (int i = -brushRadius; i <= brushRadius; i++)
        {
            for (int j = -brushRadius; j <= brushRadius; j++)
            {
                if (inBounds(cellY + i, cellX + j))
                {
                    if (elementValue == AIR || elementValue == WOOD || elementValue == VIRUS || elementValue == VOID || elementValue == FAUCET || elementValue == DINOPILL)
                    {
                        grid[cellY + i][cellX + j].setElement(elementValue);
                        grid[cellY + i][cellX + j].setColor(colors[elementValue]);
                    }
                    else if ((rand() % 5 == 0))
                    {
                        grid[cellY + i][cellX + j].setElement(elementValue);
                        grid[cellY + i][cellX + j].setColor(colors[elementValue]);
                    }
                }
            }
        }
    }
}

int main()
{
    sf::RenderWindow window(sf::VideoMode(cellSize * gridX, cellSize * gridY), "Falling Sand");
    sf::Cursor cursor;
    if (cursor.loadFromSystem(sf::Cursor::Cross))
    {
        window.setMouseCursor(cursor);
    }

    window.setFramerateLimit(60);
    bool isLeftMousePressed = false;
    bool isRightMousePressed = false;
    bool isFrontMousePressed = false;
    bool isMiddleMousePressed = false;

    drawGrid(window);
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }

            if (event.type == sf::Event::MouseButtonPressed)
            {

                if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
                {
                    isLeftMousePressed = true;
                }
                else if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
                {
                    isRightMousePressed = true;
                }
                else if (sf::Mouse::isButtonPressed(sf::Mouse::Middle))
                {
                    isMiddleMousePressed = true;
                }
                else if (sf::Mouse::isButtonPressed(sf::Mouse::XButton1))
                {
                    clearGrid();
                }
                else if (sf::Mouse::isButtonPressed(sf::Mouse::XButton2))
                {
                    isFrontMousePressed = true;
                }
            }

            if (event.type == sf::Event::KeyPressed)
            {
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::V))
                {
                    mousePositionToCell(window, VIRUS, 1);
                }
                else if (sf::Keyboard::isKeyPressed(sf::Keyboard::E))
                {
                    mousePositionToCell(window, VOID, staticBrushSize);
                }
                else if (sf::Keyboard::isKeyPressed(sf::Keyboard::F)){
                    mousePositionToCell(window, FAUCET, 1);
                }
                else if(sf::Keyboard::isKeyPressed(sf::Keyboard::D)){
                    mousePositionToCell(window, DINOPILL, 1);
                }
            }

            if (event.type == sf::Event::MouseButtonReleased)
            {
                isLeftMousePressed = false;
                isRightMousePressed = false;
                isFrontMousePressed = false;
                isMiddleMousePressed = false;
            }
        }

        if (isLeftMousePressed)
        {
            mousePositionToCell(window, SAND, defaultBrushSize);
        }
        else if (isRightMousePressed)
        {
            mousePositionToCell(window, WATER, defaultBrushSize);
        }
        else if (isMiddleMousePressed)
        {
            mousePositionToCell(window, WOOD, staticBrushSize);
        }
        else if (isFrontMousePressed)
        {
            mousePositionToCell(window, AIR, eraserBrushSize);
        }

        updateGrid();
        drawGrid(window);
    }

    return 0;
}