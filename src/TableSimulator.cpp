//
//  TableSimulator.cpp
//  basicExample
//
//  Created by Tangible Media Group on 3/20/14.
//
//

#include "TableSimulator.h"


TableSimulator::TableSimulator(ShapeIOManager * shapeManager) {
    mIOManager = shapeManager;
    mHeightMapShader.load("shaders/heightMapShader");
}

//--------------------------------------------------------------
//
// Setup interaction area for easyCam
//
// Uses the viewport as container. To replace with smaller area
// change value of vp
//
//--------------------------------------------------------------
void TableSimulator::drawInteractionArea(int px, int py, int w, int h){
	//ofRectangle vp = ofGetCurrentViewport();
    ofRectangle vp(px, py, w, h);
	float r = MIN(vp.width, vp.height) * 0.5f;
	float x = vp.width * 0.5f;
	float y = vp.height * 0.5f;
    
	ofPushStyle();
	ofSetLineWidth(2);
	ofSetColor(255, 255, 0);
	ofNoFill();
	glDepthMask(false);
	ofCircle(px+x, py+y, r);
	glDepthMask(true);
	ofPopStyle();
}

//--------------------------------------------------------------
//
// Draw instructions for easy cam
//
//--------------------------------------------------------------
void TableSimulator::drawCamViewMessage() {
    
	ofPushStyle();
    ofSetColor(255);
	string msg = string("Using mouse inputs to navigate (press 'c' to toggle): ") + (cam.getMouseInputEnabled() ? "YES" : "NO");
    msg += "\n\nLEFT MOUSE BUTTON DRAG:\nStart dragging INSIDE the yellow circle -> camera XY rotation .\nStart dragging OUTSIDE the yellow circle -> camera Z rotation (roll).\n\n";
    msg += "LEFT MOUSE BUTTON DRAG + TRANSLATION KEY (" + ofToString(cam.getTranslationKey()) + ") PRESSED\n";
    msg += "OR MIDDLE MOUSE BUTTON (if available):\n";
    msg += "move over XY axes (truck and boom).\n\n";
    msg += "RIGHT MOUSE BUTTON:\n";
    msg += "move over Z axis (dolly)";
	ofDrawBitmapStringHighlight(msg, 10, 20);
    ofPopStyle();
}

//--------------------------------------------------------------
//
// Program to setup and draw the pins on map.
// The pin simulation itself is drawn using drawPinDisplaySimulation.
// This function mainly handles openGL setup
//
// @todo matt, refactor setup out of this
//
//--------------------------------------------------------------
void TableSimulator::drawTableCamView(int px, int py, int w, int h, float zoom) {
    
    ofRectangle vp(px, py, w, h);
    
    //cam.disableMouseInput();
    
    // easyCam area is rotated around by mouse movement
    // in combination with keyboard commands
    cam.begin(vp);
    
    ofSetColor(255);
    
    // set GL properties and effects
    GLfloat mat_ambient[] = { 3.0, 3.0, 3.0, 2.0 };
    GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat mat_shininess[] = { 100.0 };
    GLfloat light_position[] = { -50.0, -50.0, 50.0, 0.0 };
    glShadeModel (GL_FLAT);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, mat_ambient);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_DEPTH_TEST);
    
    // set scale here. We dont need this inside the draw simulation method
    // anymore. This gives us more consistancy in scaling based on other factors
    // @note removing this in favor of cam.setDistance(100); causes the shading on the
    // table to dissappear!
    ofScale(zoom,zoom,zoom);
    
    // set initial rotation to be sudo ortho view
    ofRotateX(-70); // simulate height looking down on table, -90 is shorter and -20 is taller
    ofRotateZ(25); // simulate walk around table
    
    // draw the simulation
    drawPinDisplaySimulation();
    
    // turn off GL lighting, saving performance
    // and avoid GL rendering effecting other parts of the app
    glDisable(GL_LIGHTING);
    
    cam.end();
}

//--------------------------------------------------------------
//
// After setup, run this to draw the table on pins.
//
// gets pin height
//
//--------------------------------------------------------------
void TableSimulator::drawPinDisplaySimulation() {
    
    // define 3d table and pin boxes.
    ofBoxPrimitive pin = ofBoxPrimitive(0.95, 0.95, 5);
    ofBoxPrimitive table = ofBoxPrimitive(RELIEF_PHYSICAL_SIZE_X, RELIEF_PHYSICAL_SIZE_Y, 20);
    
    // enable more GL stuff
    // @todo this is added in the setup, right?
    glEnable(GL_DEPTH_TEST);
    
    // save current matric
    ofPushMatrix();
    
    // draw the table
    table.draw();
    
    // account for pin width, depth, height
    // @todo move pin width definition to constant
    // black is all the way down, white is all the up
    // gray is in the middle
    ofTranslate(0.95/2, 0.95/2, -5/2);
    
    // move to area where we want to draw the pins.
    ofTranslate(-RELIEF_PHYSICAL_SIZE_X/2, -RELIEF_PHYSICAL_SIZE_Y/2, 10);
    
    // for each section, draw the pins mapped to height.
    for (int y = 0; y < 24; y++) {
        for (int x = 0; x < PINBLOCK_0_WIDTH; x++) {
            int height = mIOManager->pinHeightToRelief[x][y];
            //ofSetColor(height);
            ofPushMatrix();
            ofTranslate(x + PINBLOCK_0_X_OFFSET, y, height/51.f);
            pin.draw();
            ofPopMatrix();
        }
        
        for (int x = 0; x < PINBLOCK_1_WIDTH; x++) {
            int height = mIOManager->pinHeightToRelief[x + PINBLOCK_0_WIDTH][y];
            //ofSetColor(height);
            ofPushMatrix();
            ofTranslate(x + PINBLOCK_1_X_OFFSET, y, height/51.f);
            pin.draw();
            ofPopMatrix();
        }
        
        for (int x = 0; x < PINBLOCK_2_WIDTH; x++) {
            int height = mIOManager->pinHeightToRelief[x + PINBLOCK_0_WIDTH + PINBLOCK_1_WIDTH][y];
            //ofSetColor(height);
            ofPushMatrix();
            ofTranslate(x + PINBLOCK_2_X_OFFSET, y, height/51.f);
            pin.draw();
            ofPopMatrix();
        }
    }
    ofPopMatrix();
    glDisable(GL_DEPTH_TEST);
}


//--------------------------------------------------------------
//
// Draws representation of actual pin heights as reported by boards
//
//--------------------------------------------------------------
void TableSimulator::drawActualPinHeightImageFromTable(int x, int y, int w, int h) {
    int xSize = w / RELIEF_SIZE_X;
    int ySize = h / RELIEF_SIZE_Y;
    for (int i = 0; i < RELIEF_SIZE_X; i++) {
        for (int j = 0; j < RELIEF_SIZE_Y; j++) {
            ofSetColor(pinsFrom[i][j]);
            ofRect(x+i*xSize, y+j*ySize, xSize, ySize);
        }
    }
}


//--------------------------------------------------------------
//
// Updates local pin array with shape manager pin array
//
// @todo Matt replace with memcpy or something more effecient!
//
//--------------------------------------------------------------
void TableSimulator::update() {
    memcpy(pinsTo, mIOManager->pinHeightToRelief, sizeof(unsigned char) * RELIEF_SIZE_X * RELIEF_SIZE_Y);
    memcpy(pinsFrom, mIOManager->pinHeightFromRelief, sizeof(unsigned char) * RELIEF_SIZE_X * RELIEF_SIZE_Y);
}



