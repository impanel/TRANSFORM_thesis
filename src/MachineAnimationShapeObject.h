//
//  shapeMachineAnimation.h
//  basicExample
//
//  Created by Tangible Media Group on 3/20/14.
//
//

#ifndef __basicExample__MachineAnimationShapeObject__
#define __basicExample__MachineAnimationShapeObject__

#include "ShapeObject.h"
#include "MachineAnimationVideoPlayer.h"
#include "Constants.h"
#include <iostream>

class MachineAnimationShapeObject : public ShapeObject {
public:
    MachineAnimationShapeObject();
    void update(float dt);
    void renderShape();
    void renderGraphics(int x, int y, int w, int h);
    vector <string> getLoadedVideoPaths();
    vector <string> getLoadedVideoFilenames();
    void drawGuiScreen(int x, int y, int w, int h);
    void playMovieByPath(string path);
    void playMovieByFilename(string filename);
    void stopNowPlaying();
    void pause();
    void resume();
    void reset();
    void setTableValuesForShape(ShapeIOManager *pIOManager);
    unsigned char* getPixels();
    void setNowPlaying(ofVideoPlayer &videoPlayer);
    void setLooping(bool _set);
    
    MachineAnimationVideoPlayer     machineAnimationVideoPlayer;

    string get_shape_name() {return shape_name; };
    
private:
    string shape_name = "Machine Animation";
};

#endif /* defined(__basicExample__MachineAnimationShapeObject__) */