//
//  FadeLoopShapeObject.h
//  basicExample
//
//  Created by Daniel Leithinger on 3/20/14.
//
//

#ifndef __basicExample__FadeLoopShapeObject__
#define __basicExample__FadeLoopShapeObject__

#include <iostream>

#include "ShapeObject.h"
#include "KinectTracker.h"
#include "ImageWarper.h"
#include "ofxOpenCv.h"
#include <iostream>

class FadeLoopShapeObject : public ShapeObject {
public:
    FadeLoopShapeObject();
    void update(float dt);
    void renderGraphics();
    void renderShape();
    
    string get_shape_name() {return shape_name; };

private:
    string shape_name = "Fade Loop";
    
    int currentBrightness = 0;
};

#endif /* defined(__basicExample__FadeLoopShapeObject__) */
