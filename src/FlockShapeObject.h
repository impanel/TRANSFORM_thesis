//
//  FlockShapeObject.h
//  basicExample
//
//  Created by Matt on 3/21/14.
//
//

#ifndef __basicExample__FlockShapeObject__
#define __basicExample__FlockShapeObject__

#include "KinectShapeObject.h"
#include "Boid.h"
#include "WavyShapeObject.h"
#include <iostream>

class FlockShapeObject : public ShapeObject {
public:
    FlockShapeObject(WavyShapeObject * wavy);
    FlockShapeObject(WavyShapeObject * wavy, int boidCount);
    
    std::vector<Boid *> boids;
    
    bool fishStayOn;
    
    long previousMS;
    long currentMS; // in ms
    int fixedDeltaMS;
    float fixedDeltaSec;
    int leftOverMS;
    
    WavyShapeObject * wavy;
    ofFbo aquarium; // our "kinect space" object for putting wave and flock together
    
    void renderShape();
    void drawGuiScreen(int x, int y, int w, int h);
    
    void reset(int boidCount);
    void update(float dt);

    void setKinectTracker(KinectTracker * pKinectTracker) { mKinectTracker = pKinectTracker; };
    void setImageWarper(ImageWarper * pImageWarper);
    void setTableValuesForShape(ShapeIOManager *pIOManager);
    
    // setters
    void setFishStay(bool stay);
    bool doFishStay();
    
    void setCohesion(float scalar);
    void setAlignment(float scalar);
    void setSeparation(float scalar);
    void setCuriosity(float scalar);
    void setFear(float scalar);
    void setStrutFreq(float freq);
    void setStrutScalar(float scalar);
    void setMinDist(float dist);
    void setMaxDist(float dist);

    string get_shape_name() {return shape_name; };
    unsigned char* getPixels();
    
private:
    KinectTracker * mKinectTracker;
    ImageWarper * mImageWarper;
    
    ofVec2f * people;
    int numPeople;
    
    string shape_name = "Flock";
    unsigned char* allPixels;
};


#endif /* defined(__basicExample__FlockShapeObject__) */
