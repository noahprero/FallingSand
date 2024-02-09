#ifndef ELEMENT_H
#define ELEMENT_H


class Element {
protected:
    int value;

public:
    Element(){
        value = 0;
    }
    
    int getElement(){
        return value;
    }

    void setElement(int elementValue){
        value = elementValue;
    }

    bool isElement(int other){
        return value == other;
    }

    bool isSolid(){
        return value == 3 || value == 1;  // Wood or Sand
    }

    void clear(){
        value = 0;
    }
};
#endif