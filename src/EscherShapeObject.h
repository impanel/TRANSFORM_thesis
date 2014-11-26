//
//  EscherShapeObject.h
//  basicExample
//
//  Created by Tangible Media Group on 4/5/14.
//
//

#ifndef __basicExample__EscherShapeObject__
#define __basicExample__EscherShapeObject__

#include "ShapeObject.h"
#include "MachineAnimationVideoPlayer.h"
#include "Constants.h"
#include <iostream>

class EscherShapeObject : public ShapeObject {
public:
    EscherShapeObject();
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
    void setTableValuesForShape(ShapeIOManager *pIOManager);
    unsigned char* getPixels();
    void setNowPlaying(ofVideoPlayer &videoPlayer);
    void setLooping(bool _set);
    
    MachineAnimationVideoPlayer     machineAnimationVideoPlayer;
    
    string get_shape_name() {return shape_name; };
    
    bool bEscherStart = false;
    bool bEscherRun = false;
    bool bEscherStop = false;
    int currentMode = 0;
    
private:
    string shape_name = "Escher_Mode";
    int oldMode = 0;
};

#endif /* defined(__basicExample__EscherShapeObject__) */

