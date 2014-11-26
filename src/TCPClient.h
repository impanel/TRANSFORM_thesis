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

// @todo: not a shape object right now

#include "ofMain.h"
#include "ofxNetwork.h"

class TCPClient {
    
public:
    
    void setup();
    void update();
    void drawDebug();
    void draw();
    
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    void playBack(vector <string> & _strFrames, int & _frameIndex, int _sequenceFPS, bool _pause, bool _play);
    
    void drawPinsDebug(unsigned char * _theColors);
    void drawPins(unsigned char * _theColors);
    
    ofxTCPServer TCP;
    
    vector <string> storeText;
    vector <ofRectangle> rects;
    int size;
    int width;
    int height;
    int sequenceFPS;
    bool bFrameIndependent;
    bool bPause;
    bool bStop;
    int frameIndex;
    float elapsedTime;
    float pauseTime;
    
    unsigned char * pixels;
    string receivedText;
};



#endif /* defined(__basicExample__TCPClient__) */
