//
//  GUIShapeObject.h
//  basicExample
//
//  Created by Daniel Windham on 2/5/15.
//
//

#ifndef __basicExample__GUIShapeObject__
#define __basicExample__GUIShapeObject__

#include <iostream>
#include "ShapeObject.h"
#include "Constants.h"
#include "ofxUI.h"

class GUIShapeObject : public ShapeObject {
public:
    GUIShapeObject();
    void setup();
    void update(float dt);
    void renderShape();
    void renderGraphics(int x, int y, int w, int h);
    void setPin(int row, int col, int height);
    void setPins(vector<ofxUIToggle *> toggles0, vector<ofxUIToggle *> toggles1, vector<ofxUIToggle *> toggles2, int height, bool add=false);
    void incrementPins(vector<ofxUIToggle *> toggles0, vector<ofxUIToggle *> toggles1, vector<ofxUIToggle *> toggles2, int height);
    void pushBackup();
    void popBackup();
    void restoreBackup();
    
    string get_shape_name() {return shape_name; };
    string shape_name = "GUI";
    
    
private:
    ofImage nowDrawing;
    vector<ofImage> backups;
};

#endif /* defined(__basicExample__GUIShapeObject__) */
