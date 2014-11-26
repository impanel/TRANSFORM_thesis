//
//  Boid.cpp
//  basicExample
//
//  Created by Jared Counts on 3/19/14.
//
//

#include "Boid.h"
#include "Constants.h"
Boid::Boid(ofVec2f pos, ofVec2f vel) {
    this->pos = pos;
    this->vel = vel;
    
    strutVel = ofVec2f(0,0);
    
    tailLen = 30;
    radius = 16;
    minX = -20;
    minY = 80;
    maxX = minX + RELIEF_PROJECTOR_SIZE_X;
    maxY = minY + RELIEF_PROJECTOR_SIZE_Y;
    maxDistFromTable = 300;
    
    cohesionScalar = 0.0003; // 0.0003
    separationScalar = 0.03; // 0.025
    alignmentScalar = 0.0025; // 0.0025
    
    curiosityScalar = 1.5; // (unused) // 0.00005
    
    fearScalar = ofRandom(1, 1.5);
    strutFreq = ofRandom(1,3);
    strutScalar = ofRandom(3,4);
    minDist = 25;
    maxDist = 200;
    
    lastPos = new ofVec2f[tailLen];
    for (int i = 0; i < tailLen; i++)
        lastPos[i] = ofVec2f(pos.x, pos.y);
    
    nearestPerson = ofVec2f(0,0);
    
    runAwayVec = nearestPerson;
    
    bool personNearby = false;
}
void Boid::draw() {
    ofEnableDepthTest();
    //ofSetColor(MIN(255, 60.f + 195.f * (pos.z + 20.f) / 40.f));
    //if (personNearby)
    int tailLenTemp = max(tailLen - (int)(vel.length() * 3), 5);
    for (int i = 0; i < tailLenTemp; i++) { //tailLen - 1; i >= 0; i--) {
        int r = radius - (radius-2) * i / tailLenTemp;
        ofSetColor(155);
        ofEllipse(lastPos[i].x, lastPos[i].y, 0, r, r);
        
        ofSetColor(185);
        ofEllipse(lastPos[i].x, lastPos[i].y, 0.1, r - r/4.f, r - r/4.f);
        
        ofSetColor(225);
        ofEllipse(lastPos[i].x, lastPos[i].y, 0.2, r - r/2.f, r - r/2.f);
        
        ofSetColor(255);
        ofEllipse(lastPos[i].x, lastPos[i].y, 0.3, r - 3.f*r/4.f, r - 3.f*r/4.f);
    }
    ofEllipse(pos.x, pos.y, 0, 14,14);
    ofDisableDepthTest();
}
void Boid::drawWave(float w, float h) {
    ofSetColor(vel.length() * 25);
    //ofPoint rePos = mImageWarper->warpPoint(pos);
    float xScalar = (float)w / (float)KINECT_X;
    float yScalar = (float)h / (float)KINECT_Y;
    ofEllipse(pos.x * xScalar, pos.y * yScalar, 2,2);
}

void Boid::update(std::vector<Boid *> *boids, ofVec2f * people, int numPeople) {
    // track positions
    if (tailLen > 0) {
        for (int i = tailLen - 1; i > 0; i--)
            lastPos[i] = lastPos[i-1];
        lastPos[0] = pos;
    }
    
    neighbors.clear();
    
    for (int i = 0; i < boids->size(); i++) {
        Boid *other = (*boids)[i];
        if (other != this && pos.squareDistance(other->pos) < maxDist*maxDist) {
            neighbors.push_back(other);
        }
    }
    int neighborCount = neighbors.size();

    if (neighborCount > 0) {
        ofVec2f v1, v2, v3;
        if (neighborCount < 4) {
            if (!personNearby && stayOn)
                v1 = cohesion(cohesionScalar);
            else
                v1 = cohesion(-cohesionScalar*2);
            v2 = alignment(alignmentScalar);
        }
        v3 = separation(separationScalar, minDist);
        
        vel += v1 + v2 + v3;
    }
    
    if (numPeople > 0) {
        if (!personNearby && stayOn) {
            runAwayVec = people[0];
            //cout << "run away vec " << runAwayVec.x << ", " << runAwayVec << "\n\n\n";
            personNearby = true;
        }
        
        ofVec2f v4 = fear(fearScalar, runAwayVec);
        vel += v4;
    }
    else
        personNearby = false;
    
    if (!stayOn) {
        ofVec2f v5 = stayOff(tableCenter);
        vel += v5;
    }
    
    strutVel = strut(strutScalar * vel.length(), strutFreq);
    //ofLog(OF_LOG_NOTICE, "%f %f is strutvel", strutVel.x, strutVel.y);
    if (!personNearby && stayOn) {
        if (pos.x > maxX - 1.1 * radius)
            vel.x -= (pos.x - (maxX - 1.1 * radius)) * 0.09;
        if (pos.x < minX + 1.1 * radius)
            vel.x += ((minX + 1.1 * radius) - pos.x) * 0.09;
        
        if (pos.y > maxY - 1.1 * radius)
            vel.y -= (pos.y - (maxY - 1.1 * radius)) * 0.09;
        if (pos.y < minY + 1.1 * radius)
            vel.y += ((minY + 1.1 * radius) - pos.y) * 0.09;
        
        // maintain a max speed
        vel.limit(0.8f);
    }
    else
        vel.limit(2.5f);
    
    if (pos.distanceSquared(tableCenter) > maxDistFromTable*maxDistFromTable) {
        ofVec2f difference = pos - tableCenter;
        difference.rescale(maxDistFromTable);
        pos = tableCenter + difference;
    }
}

void Boid::integrate(float timestep) {
    internalClock += timestep;
    pos += vel + strutVel; // * timestep;
}


ofVec2f Boid::cohesion(float scalar) {
    ofVec2f avg = ofVec2f(0,0);
    for (int i = 0; i < neighbors.size(); i++)
        avg += neighbors[i]->pos;
    
    avg /= neighbors.size();
    
    return (avg - pos) * scalar;
}
ofVec2f Boid::separation(float scalar, float minDist) {
    ofVec2f c = ofVec2f(0,0);
    for (int i = 0; i < neighbors.size(); i++) {
        if (pos.squareDistance(neighbors[i]->pos) < minDist * minDist)
            c -= neighbors[i]->pos - pos;
    }
    c *= scalar;
    return c;
}
ofVec2f Boid::alignment(float scalar) {
    ofVec2f avgVel = ofVec2f(0,0);
    for (int i = 0; i < neighbors.size(); i++)
        avgVel += neighbors[i]->vel;
    avgVel /= neighbors.size();
    
    return (avgVel - vel) * scalar;
}
ofVec2f Boid::curiosity(float scalar, ofVec2f location) {
    ofVec2f diff = location - pos;
    
    //if (nearestDistSq < 15*15) {
    diff.normalize();
    diff *= scalar;
    return diff;
    //}
    //return ofVec2f(0,0,0);
}
ofVec2f Boid::curiosity(float scalar, ofVec2f * people, int numPeople) {
    if (numPeople == 0)
        return ofVec2f(0,0);
    // find closest person
    float nearestDistSq = 9999*9999;
    ofVec2f closest = ofVec2f(0,0);
    for (int i = 0; i < numPeople; i++) {
        float distSq = (people[i] - pos).lengthSquared();
        if (distSq < nearestDistSq) {
            nearestDistSq = distSq;
            closest = people[i] - pos;
            nearestPerson = people[i];
        }
    }
    
    //if (nearestDistSq < 15*15) {
    closest.normalize();
    closest *= scalar;
    return closest;
    //}
    //return ofVec2f(0,0,0);
}
ofVec2f Boid::fear(float scalar, ofVec2f location) {
    return -1 * curiosity(scalar, location);
}
ofVec2f Boid::fear(float scalar, ofVec2f * people, int numPeople) {
    return -1 * curiosity(scalar, people, numPeople);
}
ofVec2f Boid::stayOff(ofVec2f fromWhere) {
    float distSq = pos.distanceSquared(tableCenter);
    //if (distSq < 15*15) {
    ofVec2f diff = pos - tableCenter;
    diff.normalize();
    diff *= 5;
    return diff;
    //}
    //return ofVec2f(0,0,0);
}
ofVec2f Boid::strut(float scalar, float frequency) {
    ofVec2f strutVec = ofVec2f(vel.x, vel.y);
    float angle = PI * sin(internalClock * frequency * 2 * PI);
    strutVec.rotate(angle);
    return scalar * (vel - strutVec);
}