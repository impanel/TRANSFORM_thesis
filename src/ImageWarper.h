//
//  ImageWarper.h
//  basicExample
//
//  Created by Daniel Leithinger on 3/20/14.
//
//

#ifndef __basicExample__ImageWarper__
#define __basicExample__ImageWarper__

#include <iostream>
#include "ofMain.h"
#include "ofxOpenCv.h"
#include "Constants.h"
#include "ofxXmlSettings.h"

class ImageWarper {
public:
    ImageWarper(int x, int y, int w, int h);
    ~ImageWarper();
    
    // src positions (quad points, smaller section of src image)
    void setSrcPositions(ofPoint srcPositions[4]);
    
    
    // dest points will be size of larger image
    void setDestPositions(ofPoint warpPositions[4]);
 
    // various image warping methods
    void warpIntoImage(ofxCvColorImage &pSourceImage, ofxCvColorImage &pDestinationImage);
    void warpIntoImage(ofxCvGrayscaleImage &pSourceImage, ofxCvGrayscaleImage &pDestinationImage);
    void warpIntoImage(ofFbo &pSourceImage, ofxCvGrayscaleImage &pDestinationImage);
    void warpIntoImage(ofImage &pSourceImage, ofxCvGrayscaleImage &pDestinationImage);
    void warpIntoImage(ofPixels &pSourcePixels, ofPixels &pDestinationPixels);
    
    // warp point
    ofPoint warpPoint(ofPoint src);
    
    // gui interface for selecting the quad warp points
    void mouseMoved(int x, int y);
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void drawWarpPoints();
    
    // load and save to xml
    void saveSettings(string filename);
    void loadSettings(string filename);
    ofxXmlSettings XML;
    
    // getters
    ofPoint * getSrcPositions() { return srcPositions; };
    ofPoint * getDestPositions() { return destPositions; };
private:
    ofPoint srcPositions[4];
    ofPoint destPositions[4];
    int activePointIn;
    
};

#endif /* defined(__basicExample__ImageWarper__) */
