//
//  KinectShapeObject.h
//  basicExample
//
//  Created by Daniel Leithinger on 3/20/14.
//
//

#ifndef __basicExample__KinectShapeObject__
#define __basicExample__KinectShapeObject__

#include "ShapeObject.h"
#include "KinectTracker.h"
#include "ImageWarper.h"
#include "ofxOpenCv.h"
#include <iostream>

class KinectShapeObject : public ShapeObject {
public:
    KinectShapeObject();
    void update(float dt);
    void renderGraphics();
    void renderShape();
    void setKinectTracker(KinectTracker * pKinectTracker){mKinectTracker = pKinectTracker;};
    void setImageWarper(ImageWarper * pImageWarper) {mImageWarper = pImageWarper;};
    void setTableValuesForShape(ShapeIOManager *pIOManager);
    
    string get_shape_name() {return shape_name; };
    unsigned char* getPixels();
    
private:
    KinectTracker * mKinectTracker;
    ImageWarper * mImageWarper;
    
    ofxCvGrayscaleImage mKinectHeightImage, mOutputShapeImage;
    
    string shape_name = "Kinect Shape";
    unsigned char* allPixels;
};

#endif /* defined(__basicExample__KinectShapeObject__) */
