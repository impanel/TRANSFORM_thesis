//
//  TCPClient.cpp
//  basicExample
//
//  Created by Tangible Media Group on 3/14/14.
//
//

#include "TCPClient.h"

//--------------------------------------------------------------
void TCPClient::setup(){
   
	size = RELIEF_SIZE_X * RELIEF_SIZE_Y;
    pixels = new unsigned char[size];
	
    //setup the server to listen on 11999
	TCP.setup(11996);
	TCP.setMessageDelimiter("\n");
    
    //fill the vector<>
	for(int i = 0; i < size; i++)
	{
		rects.push_back(ofRectangle());
		rects.at(i).setHeight(1);
		rects.at(i).setWidth(1);
	}
    
    bStop = true;
    bPause = false;
    sequenceFPS = 30;
    
    outImage.allocate(RELIEF_PHYSICAL_SIZE_X, RELIEF_PHYSICAL_SIZE_Y, GL_RGB);
}

//--------------------------------------------------------------
void TCPClient::update(){
    
    //get data from host (3dsmax)
    ofSetColor(255, 255, 255);
    string message = "TCP SERVER EasyPin 006 TRANSFORM \n\nconnect on port: "+ofToString(TCP.getPort());
    ofDrawBitmapString(message, 80, 80);
    
    //for each connected client lets get the data being sent and lets print it to the screen
	for(unsigned int i = 0; i < (unsigned int)TCP.getLastID(); i++)
    {
		if(!TCP.isClientConnected(i) )continue;
        
        //we only want to update the text we have recieved there is data
		string str = TCP.receive(i);
        
		if(str.length() > 0)
        {
			storeText.push_back(str);
            unsigned char * receivedChars = new unsigned char[size];
            
            for(int i = 0; i < size; i++)
                receivedChars[i] = 255  - str[i]; //invert values here because it doesn't work in maxscript
            pixels = receivedChars;
		}
	}
    
    
    //draw info about TCP
    if(TCP.isClientConnected(0))
    {
        //calculate where to draw the text
        int xPos = 80;
        int yPos = 150;
        
        //get the ip and port of the client
        string port = ofToString( TCP.getClientPort(0) );
        string ip   = TCP.getClientIP(0);
        string info = "client "+ofToString(0)+" -connected from "+ip+" on port: "+port;
        string storeInfo = "stored frames: " + ofToString(storeText.size()) + " fIdx: " + ofToString(frameIndex);
        
        //draw the info text and the received text bellow it
        ofDrawBitmapString(info, xPos, yPos);
        ofDrawBitmapString(storeInfo, xPos + 500, yPos);
    }
    
    if (bStop)
        drawPins(pixels);
	else
        playBack(storeText, frameIndex, sequenceFPS, bPause, bStop);
}

//--------------------------------------------------------------

void TCPClient::renderShape()
{
    
}

//--------------------------------------------------------------

void TCPClient::renderGraphics(int x, int y, int w, int h)
{
    outImage.draw(x, y, w, h);
}

//--------------------------------------------------------------

void TCPClient::playBack(vector <string> & _strFrames, int & _frameIndex, int _sequenceFPS, bool _pause, bool _stop)
{
    if(_strFrames.size() != 0)
    {
        if(!bPause)
        {
            _frameIndex = (int)((ofGetElapsedTimef() - elapsedTime) * _sequenceFPS) % _strFrames.size();
        }
        
        if (!bStop) {
            unsigned char * theColors = new unsigned char[size];
            for(int i = 0; i < size; i++)
            {
                theColors[i] = 255  - _strFrames[_frameIndex][i]; //invert values here because it doesn't work in maxscript
            }
            drawPins(theColors);
        }
    }
}

//--------------------------------------------------------------

void TCPClient::drawPins(unsigned char * _theColors)
{
    ofPushMatrix();
	//ofTranslate(50, 200);
    
    int xOffset = 13;
    int islandSpacer = 30; //350;
    
    int islandSize = size / 3;
    int islands = 3;
    int rows = 24;
    int columns = 16;

    outImage.begin();
    
    ofSetColor(0);
    ofRect(0, 0, RELIEF_PHYSICAL_SIZE_X, RELIEF_PHYSICAL_SIZE_Y);
    
    //sort incoming data
    for(int k = 0; k < islands; k++) // islands
	{
        for(int i = 0; i < columns; i++) // columns
        {
            for(int j = 0; j < rows; j++) // rows
            {
                //sort linear array to the 3 islands
                int idx = (j * columns) + (k * islandSize) + i;
               
                int sortIdx = (k + i) * (rows) + j;
                
                rects.at(idx).setWidth(1);
                rects.at(idx).setHeight(1);
                
                rects.at(idx).x = ( xOffset + (i + k * islandSpacer));
				rects.at(idx).y = (j);
                
                ofSetColor(_theColors[idx]);
                //draw values in sorted order
                ofRect(rects.at(idx));
            }
        }
    }
    
    outImage.end();
    
    outImage.draw(0, 0);
    
    outImage.draw(0, 400, 102 * 5, 24 * 5);
  	ofPopMatrix();
}
//--------------------------------------------------------------

//void TCPClient::togglePlay()
//{
//    bStop = !bStop;
//    bPause = false;
//    elapsedTime = ofGetElapsedTimef();
//}
//
////--------------------------------------------------------------
//
//void TCPClient::togglePause()
//{
//    bPause = !bPause;
//    bStop = false;
//    if(bPause)
//        pauseTime = ofGetElapsedTimef() - elapsedTime;
//    else
//        elapsedTime = ofGetElapsedTimef() - pauseTime;
//}
////--------------------------------------------------------------
//
//void TCPClient::clearFrames()
//{
//    storeText.clear();
//}
//--------------------------------------------------------------

void TCPClient::keyPressed(int key)
{
    if (key == 'c')
        storeText.clear();
    if (key == 'x') //toggle pause
    {
        cout<<"pause"<<endl;
        bPause = !bPause;
        bStop = false;
        if(bPause)
            pauseTime = ofGetElapsedTimef() - elapsedTime;
        else
            elapsedTime = ofGetElapsedTimef() - pauseTime;
    }
    if (key == 'z') //toggle stop
    {
        //stop
        bStop = !bStop;
        bPause = false;
        elapsedTime = ofGetElapsedTimef();
    }
}

//--------------------------------------------------------------


