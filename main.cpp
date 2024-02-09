#include <SFML/Graphics.hpp>
#include <iostream>
#include "Element.h"

const int gridX = 300;
const int gridY = 150;
const int cellSize = 5;
const int defaultBrushSize = 30;
const int woodBrushSize = 3;
const int eraserBrushSize = 10;
const int waterFlowRate = 5;

const int AIR = 0;
const int SAND = 1;
const int WATER = 2;
const int WOOD = 3;

int flowSwapPosition;
Element grid[gridY][gridX];

void drawGrid(sf::RenderWindow& window){
    window.clear();

    sf::RectangleShape cell(sf::Vector2f(cellSize, cellSize));

    for(int i = gridY - 1; i >= 0; i--){
        for(int j = 0; j < gridX; j++){
            cell.setPosition(j * cellSize, i * cellSize);
            Element& currentCell = grid[i][j];
            if(currentCell.isElement(SAND)){
                cell.setFillColor(sf::Color::Yellow);
            }
            else if(currentCell.isElement(WATER)){
                cell.setFillColor(sf::Color::Blue);
            }
            else if(currentCell.isElement(AIR)){
                cell.setFillColor(sf::Color::Black);
            }
            else if(currentCell.isElement(WOOD)){
                cell.setFillColor(sf::Color{139, 69, 19}); // Brown
            }
            window.draw(cell);
        }
    }
    window.display();
}

void clearGrid(){
    for(int i = 0; i < gridY; i++){
        for(int j = 0; j < gridX; j++){
            grid[i][j].setElement(AIR);
        }
    }
}


void swapElements(int i1, int j1, int i2, int j2){
    if(i1 == i2 && j1 == j2){
        return;
    }
    Element temp = grid[i1][j1];
    grid[i1][j1] = grid[i2][j2];
    grid[i2][j2] = temp;
}


void moveDown(int i, int j){
    swapElements(i, j, i + 1, j);
}

void moveDownRight(int i, int j){
    swapElements(i, j, i + 1, j + 1);
}

void moveDownLeft(int i, int j){
    swapElements(i, j, i + 1, j - 1);
}


void updateSand(Element currentCell, int i, int j){
    if(grid[i + 1][j].isElement(AIR) || grid[i + 1][j].isElement(WATER)){
        moveDown(i, j);
    }
    else if(grid[i + 1][j].isSolid()){
        if(j + 1 < gridX && (grid[i + 1][j + 1].isElement(AIR) || grid[i + 1][j + 1].isElement(WATER))){
            moveDownRight(i, j);
        }
        else if(j - 1 >= 0 && (grid[i + 1][j - 1].isElement(AIR) || grid[i + 1][j - 1].isElement(WATER))){
            moveDownLeft(i, j);
        }
    }
}


void spreadRight(int i, int j){
    flowSwapPosition = 0;
    for(int k = 1; k < waterFlowRate; k++){
        if(j + k >= gridX || grid[i][j + k].isSolid()){
            break;
        }
        flowSwapPosition++;
    }
    swapElements(i, j, i, j + flowSwapPosition);
}

void spreadLeft(int i, int j){
    flowSwapPosition = 0;
    for(int k = 1; k < waterFlowRate; k++){
        if(j - k < 0 || grid[i][j - k].isSolid()){
            break;
        }
        flowSwapPosition++;
    }
    swapElements(i, j, i, j - flowSwapPosition);
}


void updateWater(Element currentCell, int i, int j){
    if(grid[i + 1][j].isElement(AIR)){
        moveDown(i, j);
    }
    else{
        if(j + 1 < gridX && grid[i + 1][j + 1].isElement(AIR)){
            moveDownRight(i, j);
        }
        else if(j - 1 >= 0 && grid[i + 1][j - 1].isElement(AIR)){
            moveDownLeft(i, j);
        }

        if(rand() % 2 == 0){ // Random chance to flow left or right
            spreadLeft(i, j);
        }
        else{
            spreadRight(i, j);
        }
    } 
}


void updateGrid(){
    for(int i = gridY - 2; i >= 0; i--){
        for(int j = 0; j < gridX; j++){
            if(grid[i][j].isElement(AIR)){
                continue;
            }
            Element currentCell = grid[i][j];

            if(currentCell.isElement(SAND)){
                updateSand(currentCell, i, j);
            }
            else if(currentCell.isElement(WATER)){
                updateWater(currentCell, i, j);
            }
        }
    }
}


void mousePositionToCell(sf::RenderWindow& window, int elementValue, int brushSize) {
    sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
    if (mousePosition.x >= 0 && mousePosition.x < window.getSize().x &&
        mousePosition.y >= 0 && mousePosition.y < window.getSize().y) {
        
        int cellX = (mousePosition.x / cellSize);
        int cellY = (mousePosition.y / cellSize);
        
        // Define the radius of the brush
        int brushRadiusSquared = (brushSize / 2) * (brushSize / 2);
        
        // Iterate over cells within the brush radius
        for (int dy = -brushSize / 2; dy <= brushSize / 2; ++dy) {
            for (int dx = -brushSize / 2; dx <= brushSize / 2; ++dx) {
                int neighborX = cellX + dx;
                int neighborY = cellY + dy;
                
                // Calculate distance from current cell to neighbor
                int distanceSquared = dx * dx + dy * dy;
                
                // Update cell if it's within the brush radius
                if (distanceSquared <= brushRadiusSquared && 
                    neighborX >= 0 && neighborX < gridX && 
                    neighborY >= 0 && neighborY < gridY) {
                    grid[neighborY][neighborX].setElement(elementValue);
                }
            }
        }
    }
}


int main(){
    sf::RenderWindow window(sf::VideoMode(cellSize * gridX, cellSize * gridY), "Falling Sand");
    sf::Cursor cursor;
    if (cursor.loadFromSystem(sf::Cursor::Cross)){
        window.setMouseCursor(cursor);
    }

    window.setFramerateLimit(60);
    bool isLeftMousePressed = false;
    bool isRightMousePressed = false;
    bool isFrontMousePressed = false;
    bool isMiddleMousePressed = false;

    drawGrid(window);
    while (window.isOpen()){
        sf::Event event;
        while (window.pollEvent(event)){
            if(event.type == sf::Event::Closed){
                window.close();
            }
            
            if(event.type == sf::Event::MouseButtonPressed){

                if(sf::Mouse::isButtonPressed(sf::Mouse::Left)){
                    isLeftMousePressed = true;
                }
                else if(sf::Mouse::isButtonPressed(sf::Mouse::Right)){
                    isRightMousePressed = true;
                }
                else if(sf::Mouse::isButtonPressed(sf::Mouse::Middle)){
                    isMiddleMousePressed = true;
                }
                else if(sf::Mouse::isButtonPressed(sf::Mouse::XButton1)){
                    clearGrid();
                }
                else if(sf::Mouse::isButtonPressed(sf::Mouse::XButton2)){
                    isFrontMousePressed = true;
                }

            }

            if(event.type == sf::Event::MouseButtonReleased){
                isLeftMousePressed = false;
                isRightMousePressed = false;
                isFrontMousePressed = false;
                isMiddleMousePressed = false;
            }
        }

        
        if(isLeftMousePressed){
            mousePositionToCell(window, SAND, defaultBrushSize);
        }
        else if(isRightMousePressed){
            mousePositionToCell(window, WATER, defaultBrushSize);
        }
        else if(isMiddleMousePressed){
            mousePositionToCell(window, WOOD, woodBrushSize);
        }
        else if(isFrontMousePressed){
            mousePositionToCell(window, AIR, eraserBrushSize);
        }


        updateGrid();
        drawGrid(window);
    }

    return 0;
}