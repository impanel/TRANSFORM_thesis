//
//  HandShapeObject.cpp
//  basicExample
//
//  Created by Philipp Schoessler on 7/9/14.
//
//

#include "HandShapeObject.h"


HandShapeObject::HandShapeObject()
{
    mKinectHeightImage.allocate(KINECT_X, KINECT_Y);
    mOutputShapeImage.allocate(RELIEF_PROJECTOR_SIZE_X, RELIEF_PROJECTOR_SIZE_Y);
    allPixels = new unsigned char[RELIEF_SIZE];
};

void HandShapeObject::setup()
{
    
}

//----------------------------------------------------

void HandShapeObject::update(float dt)
{
    unsigned char * pixels;
    pixels = mKinectTracker->depthThreshedPixels();
    
    mKinectHeightImage.setFromPixels(pixels, KINECT_X, KINECT_Y);
    mImageWarper->warpIntoImage(mKinectHeightImage, mOutputShapeImage);
    
    //ofxCvGrayscaleImage smallerImage;
    smallerImage.allocate(RELIEF_PHYSICAL_SIZE_X, RELIEF_PHYSICAL_SIZE_Y);
    this->mImageWarper->warpIntoImage(mKinectHeightImage, smallerImage);
    allPixels = smallerImage.getPixels();
}

//----------------------------------------------------

void HandShapeObject::renderShape()
{
    mOutputShapeImage.draw(0,0, RELIEF_PROJECTOR_SIZE_X, RELIEF_PROJECTOR_SIZE_Y);
    //smallerImage.draw(0, 0, RELIEF_PHYSICAL_SIZE_X, RELIEF_PHYSICAL_SIZE_Y);
}

//----------------------------------------------------

void HandShapeObject::renderGraphics(int x, int y, int w, int h)
{
    mOutputShapeImage.draw(0,0, RELIEF_PROJECTOR_SIZE_X, RELIEF_PROJECTOR_SIZE_Y);
}

//----------------------------------------------------

void HandShapeObject::drawGuiScreen(int x, int y, int w, int h)
{
    
}

//----------------------------------------------------

void HandShapeObject::setTableValuesForShape(ShapeIOManager *pIOManager)
{
    pIOManager->set_max_speed(200);
    pIOManager->set_gain_p(1.5f);
    pIOManager->set_gain_i(0.045f);
    pIOManager->set_max_i(25);
    pIOManager->set_deadzone(2);
};

//----------------------------------------------------

unsigned char* HandShapeObject::getPixels()
{
    return allPixels;
}