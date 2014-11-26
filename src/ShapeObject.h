//
//  ShapeObject.h
//  basicExample
//
//  Created by Daniel Leithinger on 3/20/14.
//
//

#ifndef __basicExample__ShapeObject__
#define __basicExample__ShapeObject__

#include "ofMain.h"
#include "ShapeIOManager.h"
#include <iostream>

class ShapeObject {
public:
    virtual void update(float dt){};
    virtual void renderGraphics(){};
    virtual void renderGraphics(int x, int y, int w, int h){};
    virtual void renderShape(){};
    virtual void mouseMoved(int x, int y){};
    virtual void mousePressed(int x, int y, int button){};
    virtual void drawGuiScreen(int x, int y, int w, int h){};
    virtual unsigned char* getPixels(){};

    virtual string get_shape_name() {return shape_name; };
    virtual void setTableValuesForShape(ShapeIOManager *pIOManager){};
    
private:
    string shape_name = "Base Shape Object";
    

};

#endif /* defined(__basicExample__ShapeObject__) */
