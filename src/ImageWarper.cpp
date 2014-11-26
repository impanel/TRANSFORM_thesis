//
//  ImageWarper.cpp
//  basicExample
//
//  Created by Daniel Leithinger on 3/20/14.
//
//

#include "ImageWarper.h"

//--------------------------------------------------------------
//
// Initialize new image warper
//
//--------------------------------------------------------------

ImageWarper::ImageWarper(int x, int y, int w, int h){
	destPositions[0].set(x,   y,   0);
	destPositions[1].set(x+w, y,   0);
	destPositions[2].set(x+w, y+h, 0);
	destPositions[3].set(x,   y+h, 0);
    
    // set defaults
    // @todo check for loaded xml
    srcPositions[0].set(100,100);
	srcPositions[1].set(400,100);
	srcPositions[2].set(400,300);
	srcPositions[3].set(100,300);
    
};

ImageWarper::~ImageWarper(){};


//--------------------------------------------------------------
//
// SET POSITIONS FOR WARPING
//
// srcPositions: the quad points within src image from which to get pixels
// destPositions: quad to warp src pixels into (the w/h of the destination image)
//
//--------------------------------------------------------------

void ImageWarper::setSrcPositions(ofPoint pSrcPositions [4]){
    for (int i = 0; i < 4; i++) {
        srcPositions[i] = pSrcPositions[i];
    }
};

void ImageWarper::setDestPositions(ofPoint pDestPositions[4]){
    for (int i = 0; i < 4; i++) {
        destPositions[i] = pDestPositions[i];
    }
};

//--------------------------------------------------------------
//
// IMAGE WARPING
//
// Warps src image into destination image using quad points
//
// @note destination image has to be same dimensions for points to match
// @note images must be peroperly allocated if they are not allocated
// within each method
//
//--------------------------------------------------------------

void ImageWarper::warpIntoImage(ofxCvColorImage &pSourceImage, ofxCvColorImage &pDestinationImage){
    pDestinationImage.warpIntoMe(pSourceImage, srcPositions, destPositions);
};

//--------------------------------------------------------------

void ImageWarper::warpIntoImage(ofxCvGrayscaleImage &pSourceImage, ofxCvGrayscaleImage &pDestinationImage){
    pDestinationImage.warpIntoMe(pSourceImage, srcPositions, destPositions);
};

//--------------------------------------------------------------

void ImageWarper::warpIntoImage(ofFbo &pSourceImage, ofxCvGrayscaleImage &pDestinationImage){
    // perform warp by getting pixels from source image
    // converting them to a ofxcvcolorimage, and then warping
    ofPixels pixels;
    pSourceImage.readToPixels(pixels);
    ofxCvGrayscaleImage srcColorImage;
    srcColorImage.allocate(RELIEF_PROJECTOR_SIZE_X, RELIEF_PROJECTOR_SIZE_Y);
    srcColorImage.setFromPixels(pixels);
    pDestinationImage.warpIntoMe(srcColorImage, srcPositions, destPositions);
};

//--------------------------------------------------------------

void ImageWarper::warpIntoImage(ofImage &pSourceImage, ofxCvGrayscaleImage &pDestinationImage){
    ofPixels pixels;
    pSourceImage.getPixels();
    
    ofxCvGrayscaleImage temp;
    temp.setFromPixels(pSourceImage.getPixels(), pSourceImage.getWidth(), pSourceImage.getHeight());
    
    pDestinationImage.warpIntoMe(temp, srcPositions, destPositions);
};

//--------------------------------------------------------------

void ImageWarper::warpIntoImage(ofPixels &pSourcePixels, ofPixels &pDestinationPixels) {
    ofxCvColorImage srcColorImage, pDestinationImage;
    srcColorImage.allocate(pSourcePixels.getWidth(), pSourcePixels.getHeight());
    srcColorImage.setFromPixels(pSourcePixels);
    pDestinationImage.warpIntoMe(srcColorImage, srcPositions, destPositions);
    pDestinationPixels = pDestinationImage.getPixelsRef();
};


//--------------------------------------------------------------
//
// POINT WARPING
//
// Given an ofPoint with the destination image space (the big area)
// adjusts this point to be within the src image area (the small quad)
//
// @experimental
//
//--------------------------------------------------------------

ofPoint ImageWarper::warpPoint(ofPoint src) {
    CvPoint pDst;

    CvScalar p1, p2;
    
    cv::Point2f cvsrc[4];
    cv::Point2f cvdst[4];
    vector<cv::Point2f> cvSrcVec;
    
    CvMat* translate = cvCreateMat( 3, 3, CV_32FC1 );
    cvSetZero( translate );
    for (int i = 0; i < 4; i++ ) {
        cvsrc[i].x = srcPositions[i].x;
        cvsrc[i].y = srcPositions[i].y;
        cvdst[i].x = destPositions[i].x;
        cvdst[i].y = destPositions[i].y;
    }
    
    cvSrcVec.push_back(cv::Point2f(src.x, src.y));
    
    cv::Mat warp_matrix = cv::getPerspectiveTransform(cvsrc, cvdst);
    
    vector<cv::Point2f> dst2;
    cv::perspectiveTransform(cvSrcVec, dst2, warp_matrix);

    //cout << dst2.size() << endl;
    
    return ofPoint(dst2[0].x, dst2[0].y);
}


//--------------------------------------------------------------
//
// GUI
//
// interface for selecting points
//
//--------------------------------------------------------------

void ImageWarper::mouseDragged(int x, int y, int button) {
    if (activePointIn > -1){
        srcPositions[activePointIn].x = x;
        srcPositions[activePointIn].y = y;
    }
};

void ImageWarper::mousePressed(int x, int y, int button) {
    
     //this is not the best way
     activePointIn       = -1;
     
     float smallestDist  = 999999;
     float clickRadius = 10;
     
     for (int j = 0; j < 4; j++){
         ofPoint inputPt;
         inputPt.x = srcPositions[j].x;
         inputPt.y = srcPositions[j].y;
         inputPt.z = 0;
         float len = sqrt( (inputPt.x - x) * (inputPt.x - x) + (inputPt.y - y) * (inputPt.y - y));
         if (len < clickRadius && len < smallestDist){
             activePointIn  = j;
             smallestDist = len;
         }
     }
    
};

void ImageWarper::drawWarpPoints() {
    ofPushStyle();
    ofSetColor(ofColor::magenta);
    ofNoFill();
    ofBeginShape();
    for (int j = 0; j < 4; j++) {
        ofVertex(srcPositions[j].x, srcPositions[j].y);
    }
    ofEndShape(true);
    
    ofFill();
    ofSetColor(ofColor::magenta);
    for (int j = 0; j < 4; j++){
        ofCircle(srcPositions[j].x, srcPositions[j].y, 3);
        string message = "p"+ofToString(j);
        ofDrawBitmapString(message, srcPositions[j].x-10, srcPositions[j].y-10);
    }
    ofPopStyle();
};


//--------------------------------------------------------------
//
// LOAD AND SAVE TO XML
//
//--------------------------------------------------------------

void ImageWarper::loadSettings(string filename) {
    
    XML.loadFile(filename);
    
    for(int i = 0; i < 4; i++) {
        srcPositions[i].x = XML.getValue("WARP_POINTS:P"+ofToString(i)+":X", srcPositions[i].x);
        srcPositions[i].y = XML.getValue("WARP_POINTS:P"+ofToString(i)+":Y", srcPositions[i].y);
    }
    
};

void ImageWarper::saveSettings(string filename) {
    
    for(int i = 0; i < 4; i++) {
        XML.setValue("WARP_POINTS:P"+ofToString(i)+":X", srcPositions[i].x);
        XML.setValue("WARP_POINTS:P"+ofToString(i)+":Y", srcPositions[i].y);
    }
    
    XML.save(filename);
    
};