#ifndef ELEMENT_H
#define ELEMENT_H

sf::Color colors[] = {
    sf::Color{0, 0, 0},      // AIR,       0
    sf::Color{255,255,102},  // SAND,      1
    sf::Color{0,0,255},      // WATER,     2
    sf::Color{139,69,19},    // WOOD,      3
    sf::Color{0,255,0},      // VIRUS,     4
    sf::Color{255,250,250},  // VOID,      5
    sf::Color{135,206,250},  // FAUCET,    6
    sf::Color{184, 27, 215}, // DINOPILL,  7
    sf::Color{0,200,0}       // DINOFLESH, 8
};

class Element {
protected:
    int value;
    sf::Color color;

public:
    Element(){
        value = 0;
        color = sf::Color{0, 0, 0};
    }
    
    int getElement(){
        return value;
    }

    sf::Color getColor(){
        return color;
    }

    void setColor(sf::Color newColor){
        color = newColor;
    }

    void setElement(int elementValue){
        value = elementValue;
        setColor(colors[elementValue]);
    }

    bool isElement(int other){
        return value == other;
    }

    bool isSolid(){
        return 
        value == 1 ||  // Sand
        value == 3 ||  // Wood
        value == 4 ||  // Virus
        value == 5 ||  // Void
        value == 6 ||  // Faucet
        value == 7 ||  // Dinopill
        value == 8;    // Dinoflesh
    }
};
#endif