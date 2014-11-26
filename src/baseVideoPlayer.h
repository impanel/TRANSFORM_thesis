//
//  baseVideoPlayer.h
//  basicExample
//
//  Created by Tangible Media Group on 3/20/14.
//
//

#ifndef __basicExample__baseVideoPlayer__
#define __basicExample__baseVideoPlayer__

#include "ofMain.h"
#include "Constants.h"

class BaseVideoPlayer {
    
public:
    void setup(string path);
    void update();
    void listAllVideos();
    
    void playByPath(string path);
    void playByFilename(string filename);
    void stopNowPlaying();
    void drawNowPlaying(int x, int y, int w, int h);
    void pause();
    void resume();
    void reset();
    void setLooping(bool _set);
    
    bool isPlaying();
    unsigned char* getColorPixels();
    unsigned char* getPixels();

    ofDirectory dir;
    vector<ofVideoPlayer> videos;
    vector<string> loadedVideoPaths;
    vector<string> loadedVideoFilenames;
    ofVideoPlayer nowPlaying;
    
    string filePath;
 
};

#endif /* defined(__basicExample__baseVideoPlayer__) */
