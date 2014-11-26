//
//  HandShapeObject.h
//  basicExample
//
//  Created by Philipp Schoessler on 7/9/14.
//
//

#ifndef __basicExample__HandShapeObject__
#define __basicExample__HandShapeObject__

#include <iostream>
#include "ShapeObject.h"
#include "Constants.h"
#include "KinectTracker.h"
#include "ImageWarper.h"
#include "ofxOpenCv.h"

class HandShapeObject : public ShapeObject {

public:
    HandShapeObject();
    void setup();
    void update(float dt);
    void renderShape();
    void renderGraphics(int x, int y, int w, int h);
    void drawGuiScreen(int x, int y, int w, int h);
    void setKinectTracker(KinectTracker * pKinectTracker){mKinectTracker = pKinectTracker;};
    void setImageWarper(ImageWarper * pImageWarper) {mImageWarper = pImageWarper;};
    void setTableValuesForShape(ShapeIOManager *pIOManager);
    unsigned char* getPixels();
    
    string get_shape_name() {return shape_name; };
    string shape_name = "Hand";
    
    
private:
    KinectTracker * mKinectTracker;
    ImageWarper * mImageWarper;
    ofxCvGrayscaleImage mKinectHeightImage, mOutputShapeImage, smallerImage;
    
    unsigned char* allPixels;
};

#endif /* defined(__basicExample__HandShapeObject__) */
