//
//  TableSimulator.h
//  basicExample
//
//  Created by Tangible Media Group on 3/20/14.
//
//

#ifndef __basicExample__TableSimulator__
#define __basicExample__TableSimulator__

#include "ofMain.h"
#include "ShapeIOManager.h"
#include "Constants.h"

class TableSimulator {
    
public:
    
    TableSimulator(ShapeIOManager * shapeManager);
    ~TableSimulator();
    
    void update();
    
    // table feedback
    void drawActualPinHeightImageFromTable(int x, int y, int w, int h);
    
    // cam
    void drawCamViewMessage();
    void drawTableCamView(int px, int py, int w, int h, float zoom);
    void drawInteractionArea(int px, int py, int w, int h);
    void drawPinDisplaySimulation();
    
    ofEasyCam cam; // add mouse controls for camera movement
    
    unsigned char pinsTo [RELIEF_SIZE_X][RELIEF_SIZE_Y];
    unsigned char pinsFrom [RELIEF_SIZE_X][RELIEF_SIZE_Y];
    
    ofShader mHeightMapShader; // this shader renders the depth buffer
 
private:
    
    ShapeIOManager * mIOManager; // manages communication with the pin display
    
};

#endif 
