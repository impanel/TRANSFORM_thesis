//
//  GUIShapeObject.cpp
//  basicExample
//
//  Created by Daniel Windham on 2/5/15.
//
//

#include "GUIShapeObject.h"

GUIShapeObject::GUIShapeObject(){};

void GUIShapeObject::setup()
{
    nowDrawing.allocate(RELIEF_PHYSICAL_SIZE_X, RELIEF_PHYSICAL_SIZE_Y, OF_IMAGE_GRAYSCALE);
    
    // set all pins to 0
    nowDrawing.setColor(ofColor(0));
    nowDrawing.update();
    pushBackup();
}

//----------------------------------------------------

void GUIShapeObject::setPin(int row, int col, int height) {
    nowDrawing.getPixelsRef().setColor(col, row, ofColor(height));
    nowDrawing.update();
    return;
}

//----------------------------------------------------

void GUIShapeObject::setPins(vector<ofxUIToggle *> toggles0, vector<ofxUIToggle *> toggles1, vector<ofxUIToggle *> toggles2, int height, bool add)
{
    ofPixels &activePixels = nowDrawing.getPixelsRef();
    const ofPixels &basePixels = backups.back();
    
    for (int row = 0; row < RELIEF_PHYSICAL_SIZE_Y; row++) {
        // first block
        for (int col = 0; col < PINBLOCK_0_WIDTH; col++) {
            ofxUIToggle *toggle = toggles0[row * PINBLOCK_0_WIDTH + col];
            if (toggle->getValue()) {
                if (add) {
                    int currentHeight = basePixels.getColor(col + PINBLOCK_0_X_OFFSET, row).getBrightness();
                    activePixels.setColor(col + PINBLOCK_0_X_OFFSET, row, CLAMP(currentHeight + height, 0, 255));
                } else {
                    activePixels.setColor(col + PINBLOCK_0_X_OFFSET, row, height);
                }
            }
        }
        
        // second block
        for (int col = 0; col < PINBLOCK_1_WIDTH; col++) {
            ofxUIToggle *toggle = toggles1[row * PINBLOCK_1_WIDTH + col];
            if (toggle->getValue()) {
                if (add) {
                    int currentHeight = basePixels.getColor(col + PINBLOCK_1_X_OFFSET, row).getBrightness();
                    activePixels.setColor(col + PINBLOCK_1_X_OFFSET, row, CLAMP(currentHeight + height, 0, 255));
                } else {
                    activePixels.setColor(col + PINBLOCK_1_X_OFFSET, row, height);
                }
            }
        }
        
        // third block
        for (int col = 0; col < PINBLOCK_2_WIDTH; col++) {
            ofxUIToggle *toggle = toggles2[row * PINBLOCK_2_WIDTH + col];
            if (toggle->getValue()) {
                if (add) {
                    int currentHeight = basePixels.getColor(col + PINBLOCK_2_X_OFFSET, row).getBrightness();
                    activePixels.setColor(col + PINBLOCK_2_X_OFFSET, row, CLAMP(currentHeight + height, 0, 255));
                } else {
                    activePixels.setColor(col + PINBLOCK_2_X_OFFSET, row, height);
                }
            }
        }
    }
    
    nowDrawing.update();
    return;
}

//----------------------------------------------------
//
// Alias for setPins with add=true flag.
//
//----------------------------------------------------

void GUIShapeObject::incrementPins(vector<ofxUIToggle *> toggles0, vector<ofxUIToggle *> toggles1, vector<ofxUIToggle *> toggles2, int height)
{
    setPins(toggles0, toggles1, toggles2, height, true);
}

//----------------------------------------------------

void GUIShapeObject::pushBackup()
{
    ofImage backup;
    backup.setFromPixels(nowDrawing.getPixels(), nowDrawing.width, nowDrawing.height, OF_IMAGE_GRAYSCALE);
    backups.push_back(backup);
}

//----------------------------------------------------

void GUIShapeObject::popBackup()
{
    if (backups.size() > 1) {
        backups.pop_back();
    }
}

//----------------------------------------------------

void GUIShapeObject::restoreBackup()
{
    ofImage backup = backups.back();
    nowDrawing.setFromPixels(backup.getPixels(), backup.width, backup.height, OF_IMAGE_GRAYSCALE);
}

//----------------------------------------------------

void GUIShapeObject::update(float dt)
{
    
}

//----------------------------------------------------

void GUIShapeObject::renderShape()
{
    if(nowDrawing.bAllocated()) nowDrawing.draw(0, 0, RELIEF_PHYSICAL_SIZE_X, RELIEF_PHYSICAL_SIZE_Y);
}

//----------------------------------------------------

void GUIShapeObject::renderGraphics(int x, int y, int w, int h)
{
    if(nowDrawing.bAllocated()) nowDrawing.draw(x, y, w, h);
    
}