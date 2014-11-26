//
//  Boid.h
//  basicExample
//
//  Created by Jared Counts on 3/19/14.
//
//

#ifndef __basicExample__Boid__
#define __basicExample__Boid__

#include "ofMain.h"
#include "ImageWarper.h"
#include <vector>
class Boid {
public:
    ofVec2f pos, vel;
    ofVec2f *lastPos;
    
    ofVec2f strutVel;
    
    float radius;
    int tailLen;
    int minX, maxX, minY, maxY;
    float maxDistFromTable;
    float internalClock;
    
    
    float cohesionScalar, separationScalar, alignmentScalar, curiosityScalar, fearScalar;
    float strutFreq, strutScalar;
    float minDist, maxDist;
    
    ofVec2f nearestPerson;
    bool personNearby;
    
    ofVec2f runAwayVec; // position from which the boids will run away
    
    bool stayOn;
    ofVec2f tableCenter;
    
    std::vector<Boid *> neighbors;
    
    Boid(ofVec2f pos, ofVec2f vel);
    void draw();
    void drawWave(float w, float h);
    void update(std::vector<Boid *> *boids, ofVec2f * people, int numPeople);
    void integrate(float timestep);
    
    // steering factors
private:
    ofVec2f cohesion(float scalar);
    ofVec2f separation(float scalar, float minDist);
    ofVec2f alignment(float scalar);
    ofVec2f strut(float amplitude, float frequency);
    ofVec2f curiosity(float scalar, ofVec2f location);
    ofVec2f curiosity(float scalar, ofVec2f * people, int numPeople);
    ofVec2f fear(float scalar, ofVec2f location);
    ofVec2f fear(float scalar, ofVec2f * people, int numPeople);
    ofVec2f stayOff(ofVec2f fromWhere);
};

#endif /* defined(__basicExample__Boid__) */
