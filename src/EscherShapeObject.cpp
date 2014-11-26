//
//  EscherShapeObject.cpp
//  basicExample
//
//  Created by Tangible Media Group on 4/5/14.
//
//

#include "EscherShapeObject.h"

EscherShapeObject::EscherShapeObject() {
    machineAnimationVideoPlayer.setup("machine_animation_videos");
    machineAnimationVideoPlayer.setLooping(false);
};

void EscherShapeObject::update(float dt) {
    if(!machineAnimationVideoPlayer.nowPlaying.isPaused())
    machineAnimationVideoPlayer.update();
    
    if (currentMode != oldMode)
    {
        if (currentMode == 1)
        {
            machineAnimationVideoPlayer.reset();
            machineAnimationVideoPlayer.playByFilename("b_escher_start.mov");
            oldMode = 1;
        }
        else if (currentMode == 2)
        {
            machineAnimationVideoPlayer.reset();
            machineAnimationVideoPlayer.playByFilename("b_escher_run.mov");
            oldMode = 2;
        }
        else if (currentMode == 3)
        {
            machineAnimationVideoPlayer.reset();
            machineAnimationVideoPlayer.playByFilename("b_escher_stop.mov");
            oldMode = 3;
        }
    }
    
    machineAnimationVideoPlayer.setLooping(false);
    if(machineAnimationVideoPlayer.nowPlaying.getCurrentFrame() >= machineAnimationVideoPlayer.nowPlaying.getTotalNumFrames() - 1)
    {
        machineAnimationVideoPlayer.nowPlaying.setPaused(true);
    }
}

void EscherShapeObject::playMovieByPath(string path) {
    machineAnimationVideoPlayer.playByPath(path);
}

void EscherShapeObject::playMovieByFilename(string filename) {
    machineAnimationVideoPlayer.playByFilename(filename);
}

vector <string> EscherShapeObject::getLoadedVideoPaths() {
    return machineAnimationVideoPlayer.loadedVideoPaths;
}

vector <string> EscherShapeObject::getLoadedVideoFilenames() {
    return machineAnimationVideoPlayer.loadedVideoFilenames;
}

void EscherShapeObject::stopNowPlaying() {
    machineAnimationVideoPlayer.stopNowPlaying();
}

void EscherShapeObject::resume() {
    machineAnimationVideoPlayer.resume();
}

void EscherShapeObject::pause() {
    machineAnimationVideoPlayer.pause();
}

void EscherShapeObject::renderGraphics(int x, int y, int w, int h) {
    machineAnimationVideoPlayer.drawNowPlaying(x,y,w,h);
}

void EscherShapeObject::renderShape() {
    machineAnimationVideoPlayer.drawNowPlaying(0,0,RELIEF_PROJECTOR_SIZE_X,RELIEF_PROJECTOR_SIZE_Y);
}

void EscherShapeObject::drawGuiScreen(int x, int y, int w, int h) {
    //machineAnimationVideoPlayer.drawAllVideos();
}

void EscherShapeObject::setNowPlaying(ofVideoPlayer &videoPlayer) {
    machineAnimationVideoPlayer.nowPlaying = videoPlayer;
    cout << "setting nowPlaying" << endl;
}

void EscherShapeObject::setTableValuesForShape(ShapeIOManager *pIOManager) {
    pIOManager->set_max_speed(200);
    pIOManager->set_gain_p(1.5f);
    pIOManager->set_gain_i(0.045f);
    pIOManager->set_max_i(25);
    pIOManager->set_deadzone(2);
}

unsigned char* EscherShapeObject::getPixels()
{
    return machineAnimationVideoPlayer.getPixels();
}

void EscherShapeObject::setLooping(bool _set){
    machineAnimationVideoPlayer.setLooping(_set);
}