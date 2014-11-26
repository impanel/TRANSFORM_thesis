//
//  baseVideoPlayer.cpp
//  basicExample
//
//  Created by Tangible Media Group on 3/20/14.
//
//

#include "baseVideoPlayer.h"

//--------------------------------------------------------------
//
// Setup
//
//--------------------------------------------------------------

void BaseVideoPlayer::setup(string path) {
    filePath = path;
    listAllVideos();
}


//--------------------------------------------------------------
//
// @todo this could be optimized to only load new videos
// currently it reloads all the videos each time this is called
//
//--------------------------------------------------------------

void BaseVideoPlayer::listAllVideos() {
    
    dir.listDir(filePath); // @todo move to constant
    dir.sort();
    
    // if directory is not empty, assign memory in vector
    // @todo check for dot files
    
    int newSize = (int)dir.size();
    
    if(newSize > 0) {
        videos.assign(newSize, ofVideoPlayer());
    
        // load each movie and set to first frame for display
        for(int i = 0; i < newSize; i++) {
            
            videos.resize(newSize);
            loadedVideoPaths.resize(newSize);
            loadedVideoFilenames.resize(newSize);
            
            cout << "== Loading new movie " << dir.getPath(i) << endl;
            videos[i].loadMovie(dir.getPath(i));
            loadedVideoPaths[i] = dir.getPath(i);
            loadedVideoFilenames[i] = dir.getName(i);
            
        }
    }
}

//--------------------------------------------------------------
//
// Update
//
//--------------------------------------------------------------

void BaseVideoPlayer::update() {
    
    // videos
    // update all videos
    for(int i = 0; i < (int)videos.size(); i++) {
        videos[i].update();
    }
    
    // update our main video, if set
    nowPlaying.update();
    
}

//--------------------------------------------------------------
//
// Plays video by path. You must pass the full path, including
// folder and filename for this to work.
//
//--------------------------------------------------------------

void BaseVideoPlayer::playByPath(string path) {
    //
    for (vector<ofVideoPlayer>::iterator it = videos.begin() ; it != videos.end(); ++it) {
        if(path == (*it).getMoviePath()) {
            nowPlaying = *it;
            nowPlaying.play();
            cout << " now playing video by path " << path;
        }
    }
}

//--------------------------------------------------------------
//
// Play video by filename, automatically prefixes the filePath
// for this video player.
//
//--------------------------------------------------------------

void BaseVideoPlayer::playByFilename(string filename) {
    filename = filePath + "/" + filename; // prefix this folder path
    playByPath(filename);
    cout<< "play: " << filename << endl;

}

//--------------------------------------------------------------
//
// Stops currently playing video
//
//--------------------------------------------------------------

void BaseVideoPlayer::stopNowPlaying() {
    nowPlaying.stop(); //
}

//--------------------------------------------------------------
//
// Pauses currently playing video if video is playing
//
//--------------------------------------------------------------

void BaseVideoPlayer::pause() {
    //if(nowPlaying.isPlaying()) nowPlaying.stop();
    if(nowPlaying.isPlaying()) nowPlaying.setPaused(true);
    //else nowPlaying.setPaused(false);
}

//--------------------------------------------------------------
//
// Resumes currently playing video if video is paused
//
//--------------------------------------------------------------

void BaseVideoPlayer::resume() {
    if(!nowPlaying.isPlaying()) nowPlaying.play();
}

//--------------------------------------------------------------

void BaseVideoPlayer::reset() {
    nowPlaying.setFrame(0);
    pause();
}

//--------------------------------------------------------------
//
// Draws video that is currently playing
// @deprecated
//
//--------------------------------------------------------------

void BaseVideoPlayer::drawNowPlaying(int x, int y, int w, int h) {
    if(nowPlaying.isPlaying() || nowPlaying.isPaused()) {
        nowPlaying.draw(x,y,w,h);
    }
}


//--------------------------------------------------------------
//
// Gets pixels from currently playing movie as 1 channel grayscale image
//
//--------------------------------------------------------------

unsigned char * BaseVideoPlayer::getPixels() {
    
    unsigned char * returnArr = new unsigned char[RELIEF_SIZE];
    unsigned char * temp = new unsigned char[RELIEF_SIZE * 3];
    temp = nowPlaying.getPixels();
    
    //convert 3 channel to 1 channel;
    if (temp != 0) { //check if there are pixels in the array (if the video is not playing there won't be any)
        for (int i = 1; i <= 102 * 24; i++) {
            char val = (temp[i * 3 - 1] + temp[i * 3 - 2] + temp[i * 3 - 3]) / 3;
            returnArr[i -1] = val;
        }
    }
    else for (int i = 1; i <= 102 * 24; i++)returnArr[i] = 0;
    
    //return nowPlaying.getPixels();
    return returnArr;
}

//--------------------------------------------------------------
//
// Gets pixels from currently playing movie as color
//
//--------------------------------------------------------------

unsigned char * BaseVideoPlayer::getColorPixels() {
    return nowPlaying.getPixels();
}

//--------------------------------------------------------------
//
// will be true if main video is currently playing
//
//--------------------------------------------------------------

bool BaseVideoPlayer::isPlaying() {
    return nowPlaying.isPlaying();
}

//--------------------------------------------------------------

void BaseVideoPlayer::setLooping(bool _set)
{
    if(_set) nowPlaying.setLoopState(OF_LOOP_NORMAL);
    else nowPlaying.setLoopState(OF_LOOP_NONE);
}


