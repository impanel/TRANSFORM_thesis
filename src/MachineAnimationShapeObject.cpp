//
//  shapeMachineAnimation.cpp
//  basicExample
//
//  Created by Tangible Media Group on 3/20/14.
//
//

#include "MachineAnimationShapeObject.h"

MachineAnimationShapeObject::MachineAnimationShapeObject() {
    machineAnimationVideoPlayer.setup("machine_animation_videos");
};

void MachineAnimationShapeObject::update(float dt) {
    machineAnimationVideoPlayer.update();
}

void MachineAnimationShapeObject::playMovieByPath(string path) {
    machineAnimationVideoPlayer.playByPath(path);
}

void MachineAnimationShapeObject::playMovieByFilename(string filename) {
    machineAnimationVideoPlayer.playByFilename(filename);
}

vector <string> MachineAnimationShapeObject::getLoadedVideoPaths() {
    return machineAnimationVideoPlayer.loadedVideoPaths;
}

vector <string> MachineAnimationShapeObject::getLoadedVideoFilenames() {
    return machineAnimationVideoPlayer.loadedVideoFilenames;
}

void MachineAnimationShapeObject::stopNowPlaying() {
    machineAnimationVideoPlayer.stopNowPlaying();
}

void MachineAnimationShapeObject::resume() {
    machineAnimationVideoPlayer.resume();
}

void MachineAnimationShapeObject::reset() {
    machineAnimationVideoPlayer.reset();
}

void MachineAnimationShapeObject::pause() {
    machineAnimationVideoPlayer.pause();
}

void MachineAnimationShapeObject::renderGraphics(int x, int y, int w, int h) {
    machineAnimationVideoPlayer.drawNowPlaying(x,y,w,h);
}

void MachineAnimationShapeObject::renderShape() {
    machineAnimationVideoPlayer.drawNowPlaying(0,0,RELIEF_PROJECTOR_SIZE_X,RELIEF_PROJECTOR_SIZE_Y);
}

void MachineAnimationShapeObject::drawGuiScreen(int x, int y, int w, int h) {
    //machineAnimationVideoPlayer.drawAllVideos();
}

void MachineAnimationShapeObject::setNowPlaying(ofVideoPlayer &videoPlayer) {
    machineAnimationVideoPlayer.nowPlaying = videoPlayer;
    cout << "setting nowPlaying" << endl;
}

void MachineAnimationShapeObject::setTableValuesForShape(ShapeIOManager *pIOManager) {
    pIOManager->set_max_speed(200);
    pIOManager->set_gain_p(1.5f);
    pIOManager->set_gain_i(0.045f);
    pIOManager->set_max_i(25);
    pIOManager->set_deadzone(2);
}

unsigned char* MachineAnimationShapeObject::getPixels()
{
    return machineAnimationVideoPlayer.getPixels();
}

void MachineAnimationShapeObject::setLooping(bool _set){
    machineAnimationVideoPlayer.setLooping(_set);
}