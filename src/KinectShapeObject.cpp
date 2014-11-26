//
//  KinectShapeObject.cpp
//  basicExample
//
//  Created by Daniel Leithinger on 3/20/14.
//
//

#include "KinectShapeObject.h"

KinectShapeObject::KinectShapeObject() {
    mKinectHeightImage.allocate(KINECT_X, KINECT_Y);
    mOutputShapeImage.allocate(RELIEF_PROJECTOR_SIZE_X, RELIEF_PROJECTOR_SIZE_Y);
    allPixels = new unsigned char[RELIEF_SIZE];
};

void KinectShapeObject::update(float dt){
    unsigned char * pixels;
    pixels = mKinectTracker->depthThreshedPixels();
    mKinectHeightImage.setFromPixels(pixels, KINECT_X, KINECT_Y);
    mImageWarper->warpIntoImage(mKinectHeightImage, mOutputShapeImage);
    
    
    ofxCvGrayscaleImage smallerImage;
    smallerImage.allocate(RELIEF_PHYSICAL_SIZE_X, RELIEF_PHYSICAL_SIZE_Y);
    this->mImageWarper->warpIntoImage(mKinectHeightImage, smallerImage);
    allPixels = smallerImage.getPixels();
};

void KinectShapeObject::renderGraphics(){
    mOutputShapeImage.draw(0,0, RELIEF_PROJECTOR_SIZE_X, RELIEF_PROJECTOR_SIZE_Y);
};

void KinectShapeObject::renderShape(){
    mOutputShapeImage.draw(0,0, RELIEF_PROJECTOR_SIZE_X, RELIEF_PROJECTOR_SIZE_Y);
};

void KinectShapeObject::setTableValuesForShape(ShapeIOManager *pIOManager) {
    pIOManager->set_max_speed(200);
    pIOManager->set_gain_p(1.5f);
    pIOManager->set_gain_i(0.045f);
    pIOManager->set_max_i(25);
    pIOManager->set_deadzone(2);
};

unsigned char* KinectShapeObject::getPixels()
{
    return allPixels;
}