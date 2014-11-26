//
//  WavyShapeObject.h
//  basicExample
//
//  Created by Tangible Media Group on 3/21/14.
//
//

#ifndef __basicExample__WavyShapeObject__
#define __basicExample__WavyShapeObject__

#include "ShapeObject.h"
#include "KinectTracker.h"
#include "ImageWarper.h"
#include "ofxOpenCv.h"
#include <iostream>

class WavyShapeObject : public ShapeObject {

public:
    
    void setKinectTracker(KinectTracker * pKinectTracker){mKinectTracker = pKinectTracker;};
    void setImageWarper(ImageWarper * pImageWarper) {mImageWarper = pImageWarper;};
    
    ofImage surface;
    float **velocity;
    float **density;
    
    // array of velocity derivatives
    // allows for nth order approximation of the velocity field
    float ***taylorSeries;
    int interpolate;
    long lastInterp;
    
    int solveCount = 1;
    
    float friction;
    float speed;
    
    int cols, rows;
    
    int getCols();
    int getRows();
    
    float seaLevel; // Resting height for the liquid. 0 - 255
    float getSeaLevel();
    void  setSeaLevel(float level);
    
    float waveAmplitude; // The wave powers
    float getWaveAmplitude();
    void  setWaveAmplitude(float amplitude);
    
    float waveScalar; // Scales both sea level and wave amplitude 0 - 1 inclusive
    float destWaveScalar;
    
    float idleWaveAmplitude = 75;
    float idleWaveFrequency = 10;

    long waveScalarSetTime;
    void setWaveScalar(float scalar);
    float getWaveScalar();
    
    float idleWaveScalar;
    void setIdleWaveScalar(float scalar);
    
    //unsigned char * getHeightMapPixels();
    ofImage depthImageInput;
    ofPixels depthMap;
    ofFbo depthData;
    ofFbo depthOutputFBO;
    
    WavyShapeObject();
    WavyShapeObject(float width, float height, float cellSize);
    //WavyShapeObject(float width, float height, float cellSize);
    WavyShapeObject(int numCols, int numRows);
    
    void interpolateSurface();
    //void drawHeightMap(int x, int y, int w, int h);
    //void drawGraphics(int x, int y, int w, int h);
    
    void update(float dt);
    void renderGraphics(int x, int y, int w, int h);
    void renderShape();
    void drawGuiScreen(int x, int y, int w, int h);
    
    void updateDepthMapFromPixels(unsigned char * pixels, int w, int h, ofImageType imageType);
    
    void solveFluid();
    float getAdjacentDensitySum(int x, int y);
    void getDepthMapKinect();
    void setHeartbeat(bool _useHeartbeat);
    
    long previousMS;
    long currentMS; // in ms
    int fixedDeltaMS;
    int leftOverMS;
    

    string get_shape_name() {return shape_name; };
    void setTableValuesForShape(ShapeIOManager *pIOManager);
    
    ofFbo editor;
    void beginEdit();
    void endEdit();
    unsigned char* getPixels();
    
private:
    KinectTracker * mKinectTracker;
    ImageWarper * mImageWarper;
    void init(int numCols, int numRows);

    void idleWaves();
    
    string shape_name = "Wavy";
    
    //tracking closest blob and map to heartbeat
    ofVec2f * people;
    ofVec2f closestBlob;
    float dist;
    //float time;
    unsigned char* allPixels;
    
    int minX, minY, maxX, maxY;
};

#endif /* defined(__basicExample__WavyShapeObject__) */
