//
//  FadeLoopShapeObject.cpp
//  basicExample
//
//  Created by Daniel Leithinger on 3/20/14.
//
//

#include "FadeLoopShapeObject.h"

FadeLoopShapeObject::FadeLoopShapeObject() {
    
}

void FadeLoopShapeObject::update(float dt) {

    currentBrightness++;
    currentBrightness = (currentBrightness > 255) ? 0 : currentBrightness;
    
}

void FadeLoopShapeObject::renderGraphics() {
    
}

void FadeLoopShapeObject::renderShape() {
    ofSetColor(currentBrightness);
    ofRect(0, 0, 1020, 240);
}