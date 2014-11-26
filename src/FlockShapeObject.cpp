//
//  FlockShapeObject.cpp
//  basicExample
//
//  Created by Matt on 3/21/14.
//
//

#include "FlockShapeObject.h"
#include "ofMain.h"
FlockShapeObject::FlockShapeObject(WavyShapeObject * wavy) {
    this->wavy = wavy;
    reset(5);
}

FlockShapeObject::FlockShapeObject(WavyShapeObject * wavy, int boidCount) {
    this->wavy = wavy;
    reset(boidCount);
    allPixels = new unsigned char[RELIEF_PHYSICAL_SIZE_X * RELIEF_PHYSICAL_SIZE_Y];
}
void FlockShapeObject::reset(int boidCount) {
    fixedDeltaMS = 20;
    fixedDeltaSec = (float)fixedDeltaMS / 1000.f;
    
    // create boids
    for (int i = 0; i < boidCount; i++) {
        Boid * boid = new Boid(ofVec3f(ofRandom(100,105),ofRandom(100,105),ofRandom(-5,5)), ofVec3f(0,0,0));
        boids.push_back(boid);
    }
    
    aquarium.allocate(KINECT_X,KINECT_Y, GL_RGB);
    
    setFishStay(true);
    
    //wavy->setWaveScalar(0.25);
}

void FlockShapeObject::update(float dt) {
    
    // update table coordinates if needed
    
    ofPoint * src = mImageWarper->getSrcPositions();
    if (boids[0]->minX != min(min(src[0].x, src[1].x), min(src[2].x, src[3].x))) {
        ofPoint tableCenter = ofVec2f(0.25 * (src[0].x + src[1].x + src[2].x + src[3].x), 0.25 * (src[0].y + src[1].y + src[2].y + src[3].y));
        
        for (int i = 0; i < boids.size(); i++) {
            boids[i]->minX = min(min(src[0].x, src[1].x), min(src[2].x, src[3].x)) - 15;
            boids[i]->minY = min(min(src[0].y, src[1].y), min(src[2].y, src[3].y)) - 15;
            
            boids[i]->maxX = max(max(src[0].x, src[1].x), max(src[2].x, src[3].x)) + 15;
            boids[i]->maxY = max(max(src[0].y, src[1].y), max(src[2].y, src[3].y)) + 15;
            
            boids[i]->tableCenter = tableCenter;
        }
    }
    
    // where are people in relation to the table?
    int blobCount = mKinectTracker->getContourFinder()->nBlobs;
    numPeople = blobCount;
    people = new ofVec2f[blobCount];
    for (int i = 0; i < blobCount; i++) {
        ofPoint loc = mKinectTracker->getContourFinder()->blobs[i].centroid;
        //ofPoint reloc = mImageWarper->warpPoint(loc);
        people[i] = ofVec2f(loc.x, loc.y);
    }
    
    // find the elapsed time and integrate based on that
    currentMS = ofGetElapsedTimeMillis();
    long deltaMS = currentMS - previousMS;
    previousMS = currentMS; // reset previousTime
    
    int timestepCount = (int)((float)(deltaMS + leftOverMS) / (float)fixedDeltaMS);
    
    leftOverMS += (int)(deltaMS - (timestepCount * fixedDeltaMS));
    
    timestepCount = ofClamp(timestepCount, 0, 5); // limit to 5 so nothing freezes
    for (int t = 0; t < timestepCount; t++) {
        
        for (int i = 0; i < boids.size(); i++) {
            boids[i]->update(&boids, people, blobCount);
            boids[i]->integrate(fixedDeltaSec);
        }
    }
    
    wavy->beginEdit();
    for (int i = 0; i < boids.size(); i++)
        boids[i]->drawWave(wavy->cols, wavy->rows);
    wavy->endEdit();
    wavy->update(dt);
    
}
void FlockShapeObject::renderShape() {
    aquarium.begin();
    ofBackground(0);
    ofSetColor(255);
    wavy->renderGraphics(0,0, KINECT_X, KINECT_Y);
    
//    ofSetColor(0,127);
//    ofRect(0,0, KINECT_X, KINECT_Y);
//    ofSetColor(255);
    
    //if(!fishStayOn)
    //{
    for (int i = 0; i < boids.size(); i++)
        boids[i]->draw();
    //}
    aquarium.end();

    ofPixels pixels;
    aquarium.readToPixels(pixels);
    ofxCvColorImage srcColorImage;
    srcColorImage.allocate(KINECT_X, KINECT_Y);
    srcColorImage.setFromPixels(pixels);
    
    ofxCvColorImage dstColorImage;
    dstColorImage.allocate(RELIEF_PROJECTOR_SIZE_X, RELIEF_PROJECTOR_SIZE_Y);
    
    this->mImageWarper->warpIntoImage(srcColorImage, dstColorImage);
    
    dstColorImage.draw(0,0);
    
    
    ofxCvColorImage smallColorImage = dstColorImage;
    smallColorImage.resize(RELIEF_PHYSICAL_SIZE_X, RELIEF_PHYSICAL_SIZE_Y);
    
    unsigned char* temp = new unsigned char[102*24*3];
    temp = smallColorImage.getPixels();
    if (temp != 0) {
        for (int i = 1; i <= 102 * 24; i++) {
            char val = (temp[i * 3 - 1] + temp[i * 3 - 2] + temp[i * 3 - 3]) / 3;
            allPixels[i -1] = val;
        }
    }
    
}
void FlockShapeObject::drawGuiScreen(int x, int y, int w, int h) {
    aquarium.begin();
    ofBackground(0);
    ofSetColor(255);
    wavy->renderGraphics(0,0, aquarium.getWidth(), aquarium.getHeight());
    ofSetColor(0,127);
    ofRect(0,0, KINECT_X, KINECT_Y);
    ofSetColor(255);
    for (int i = 0; i < boids.size(); i++) {
        boids[i]->draw();
        if (boids[i]->personNearby)
            ofLine(boids[i]->pos.x, boids[i]->pos.y, boids[i]->runAwayVec.x, boids[i]->runAwayVec.x);
            //ofLine(boids[i]->pos.x, boids[i]->pos.y, boids[i]->nearestPerson.x, boids[i]->nearestPerson.y);
    }
    
    ofSetColor(0,255,0);
    for (int i = 0; i < numPeople; i++)
        ofEllipse(people[i].x, people[i].y, 15, 15);
    
    ofSetColor(255,0,0);
    ofLine(boids[0]->minX, boids[0]->minY, boids[0]->maxX, boids[0]->minY);
    ofLine(boids[0]->maxX, boids[0]->minY, boids[0]->maxX, boids[0]->maxY);
    ofLine(boids[0]->maxX, boids[0]->maxY, boids[0]->minX, boids[0]->maxY);
    ofLine(boids[0]->minX, boids[0]->maxY, boids[0]->minX, boids[0]->minY);
    aquarium.end();
    
    ofSetColor(255);
    aquarium.draw(x,y,w,h);
    
    
    
//    ofPushMatrix();
//    wavy->renderGraphics(0,0, ofGetWidth(), ofGetHeight());
//    mKinectTracker->drawDepthImage(0,0, ofGetWidth(), ofGetHeight());
//    
//    ofTranslate(mImageWarper->getSrcPositions()[0].x, mImageWarper->getSrcPositions()[0].y);
//    
//    ofSetColor(255);
//    for (int i = 0; i < boids.size(); i++) {
//        boids[i]->draw();
//        if (boids[i]->personNearby)
//            ofLine(boids[i]->pos.x, boids[i]->pos.y, boids[i]->pos.z, boids[i]->nearestPerson.x, boids[i]->nearestPerson.y, boids[i]->nearestPerson.z);
//    }
//    
//    ofSetColor(0,255,0);
//    for (int i = 0; i < numPeople; i++)
//        ofEllipse(people[i].x, people[i].y, 10, 10);
//    ofPopMatrix();
}

void FlockShapeObject::setFishStay(bool stay) {
    for (int i = 0; i < boids.size(); i++)
        boids[i]->stayOn = stay;
}
bool FlockShapeObject::doFishStay() {
    return boids[0]->stayOn;
}

void FlockShapeObject::setImageWarper(ImageWarper * pImageWarper) {
    this->mImageWarper = pImageWarper;
}

void FlockShapeObject::setCohesion(float scalar) {
    for (int i = 0; i < boids.size(); i++)
        boids[i]->cohesionScalar = scalar;
}

void FlockShapeObject::setAlignment(float scalar) {
    for (int i = 0; i < boids.size(); i++)
        boids[i]->alignmentScalar = scalar;
}

void FlockShapeObject::setSeparation(float scalar) {
    for (int i = 0; i < boids.size(); i++)
        boids[i]->separationScalar = scalar;
}

void FlockShapeObject::setCuriosity(float scalar) {
    for (int i = 0; i < boids.size(); i++)
        boids[i]->curiosityScalar = scalar;
}
void FlockShapeObject::setFear(float scalar) {
    for (int i = 0; i < boids.size(); i++)
        boids[i]->fearScalar = scalar;
}
void FlockShapeObject::setStrutFreq(float freq) {
    for (int i = 0; i < boids.size(); i++)
        boids[i]->strutFreq = freq;
}
void FlockShapeObject::setStrutScalar(float scalar) {
    for (int i = 0; i < boids.size(); i++)
        boids[i]->strutScalar = scalar;
}


void FlockShapeObject::setMinDist(float dist) {
    for (int i = 0; i < boids.size(); i++)
        boids[i]->minDist = dist;
}

void FlockShapeObject::setMaxDist(float dist) {
    for (int i = 0; i < boids.size(); i++)
        boids[i]->maxDist = dist;
}

void FlockShapeObject::setTableValuesForShape(ShapeIOManager *pIOManager) {
    pIOManager->set_max_speed(200);
    pIOManager->set_gain_p(1.5f);
    pIOManager->set_gain_i(0.045f);
    pIOManager->set_max_i(25);
    pIOManager->set_deadzone(2);
};

unsigned char* FlockShapeObject::getPixels()
{
    return allPixels;
}
