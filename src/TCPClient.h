//
//  TCPClient.h
//  basicExample
//
//  Created by Tangible Media Group on 3/14/14.
//
//

#ifndef __basicExample__TCPClient__
#define __basicExample__TCPClient__

#include <iostream>

//#include "ofMain.h"
#include "ofxNetwork.h"
#include <iostream>
#include "ShapeObject.h"
#include "Constants.h"
#include "ofxOpenCv.h"


class TCPClient : public ShapeObject
{    
public:
    TCPClient();
    void setup();
    void update();
    void drawDebug();
    void draw();
    void exit();
    
    void keyPressed(int key);
    void drawPinsDebug(unsigned char * _theColors);
    void drawPins(unsigned char * _theColors);
    void renderShape();
    void renderGraphics(int x, int y, int w, int h);
    
    
    void setTableValuesForShape(ShapeIOManager *pIOManager);
    //unsigned char* getPixels();
    
    ofxTCPServer TCP;
    
    string get_shape_name() {return shape_name; };
    string shape_name = "tcp";

    void togglePlay();
    void togglePause();
    void clearFrames();

private:
    void playBack(vector <string> & _strFrames, int & _frameIndex, int _sequenceFPS, bool _pause, bool _play);
    bool checkForErrors();
    vector <string> storeText;
    vector <ofRectangle> rects;

    int size;
    int width;
    int height;
    int sequenceFPS;
    bool bFrameIndependent;
    bool bPause;
    bool bStop;
    bool bErrorDetected;
    bool bMentionError;
    int frameIndex;
    int oldCheckSum;

    float elapsedTime;
    float pauseTime;
    
    unsigned char * pixels;
    unsigned char * sortedPixels;
    string receivedText;
    
    ofFbo outImage;
};



#endif /* defined(__basicExample__TCPClient__) */
