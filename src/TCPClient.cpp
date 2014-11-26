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
    
    //ofSetFrameRate(3);
	//ofSetVerticalSync(true);
    
    //TRANSFORM dimensions
    width = 3 * 16;
    height = 24;
    
	size = width * height;
	pixels = new unsigned char[size];
	
    //setup the server to listen on 11999
	TCP.setup(11996);
	TCP.setMessageDelimiter("\n");
    
    //fill the vector<>
	for(int i = 0; i < size; i++)
	{
		rects.push_back(ofRectangle());
		rects.at(i).setHeight(10);
		rects.at(i).setWidth(10);
	}
	cout << rects.size() << endl;
    
    bStop = true;
    bPause = false;
    sequenceFPS = 30;
}

//--------------------------------------------------------------
void TCPClient::update(){
	ofBackground(0, 0, 0);
}

//--------------------------------------------------------------
void TCPClient::drawDebug(){
    ofBackground(0);
    //ofLog(OF_LOG_VERBOSE, "draw bebug");
	//ofSetHexColor(0xDDDDDD);
    ofSetColor(100,100,100);
    string message = "TCP SERVER EasyPin 005 TRANSFORM \n\nconnect on port: "+ofToString(TCP.getPort());
	ofDrawBitmapString(message, 80, 80);
    
    //for each connected client lets get the data being sent and lets print it to the screen
	for(unsigned int i = 0; i < (unsigned int)TCP.getLastID(); i++) {
        
		if(!TCP.isClientConnected(i) )continue;
        
		ofSetColor(255, 255, 255);
        
		//calculate where to draw the text
		int xPos = 15;
		int yPos = 140 + (10 * i);
        
		//get the ip and port of the client
		string port = ofToString( TCP.getClientPort(i) );
		string ip   = TCP.getClientIP(i);
		string info = "client "+ofToString(i)+" -connected from "+ip+" on port: "+port;
        string storeInfo = "stored frames: " + ofToString(storeText.size()) + " fIdx: " + ofToString(frameIndex);
        
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
        
		//draw the info text and the received text bellow it
		ofDrawBitmapString(info, xPos, yPos);
        ofDrawBitmapString(storeInfo, xPos + 500, yPos);
	}
    //ofDrawBitmapString( ofToString(ofGetFrameRate()), ofGetWidth() - 50, 20);
	
    
    
    //if (bStop)
    //    drawPins(pixels);
	//else
    //    playBack(storeText, frameIndex, sequenceFPS, bPause, bStop);
}

//--------------------------------------------------------------
void TCPClient::draw(){
    
	ofSetHexColor(0xDDDDDD);
    
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
    //ofDrawBitmapString( ofToString(ofGetFrameRate()), ofGetWidth() - 50, 20);
	
    
    if (bStop)
        drawPins(pixels);
	else
        playBack(storeText, frameIndex, sequenceFPS, bPause, bStop);
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

void TCPClient::drawPinsDebug(unsigned char * _theColors)
{
    glPushMatrix();
	//ofTranslate(50, 200);
    
    int islandSpacer = 350;
    int islandSize = size / 3;
    int islands = 3;
    int rows = 16;
    int columns = 24;
    
    ofNoFill();
    ofSetColor(255, 255, 255);
    ofRect(-10, -10, 195, 282);
    ofRect( islandSpacer - 10, -10, 195, 282);
    ofRect( 2 * islandSpacer - 10, -10, 195, 282);
    ofFill();
    
    for(int k = 0; k < islands; k++) // islands
	{
        for(int i = 0; i < columns; i++) // colums
        {
            for(int j = 0; j < rows; j++) // rows
            {
                rects.at((i * rows) + (k * islandSize) + j).x = (j * 11 + k * islandSpacer);
				rects.at((i * rows) + (k * islandSize) + j).y = (i * 11);
                ofSetColor(_theColors[(i * rows) + (k * islandSize) + j]);
                //ofSetColor(128);
				ofRect(rects.at((i * rows) + (k * islandSize) + j));
                //ofSetColor(255,0,0);
                //ofDrawBitmapString(ofToString((i * rows) + (k * islandSize) + j), rects.at((i * rows) + (k * islandSize) + j).x, rects.at((i * rows) + (k * islandSize) + j).y);
            }
        }
    }
    
  	glPopMatrix();
}

void TCPClient::drawPins(unsigned char * _theColors)
{
    glPushMatrix();
	//ofTranslate(50, 200);
    
    int xOffset = 13;
    int islandSpacer = 30; //350;
    int islandSize = size / 3;
    int islands = 3;
    int rows = 16;
    int columns = 24;
    
    ofNoFill();
    ofSetColor(255, 255, 255);
    //ofRect(-10, -10, 195, 282);
    //ofRect( islandSpacer - 10, -10, 195, 282);
    //ofRect( 2 * islandSpacer - 10, -10, 195, 282);
    ofFill();
    
    for(int k = 0; k < islands; k++) // islands
	{
        for(int i = 0; i < columns; i++) // columns
        {
            for(int j = 0; j < rows; j++) // rows
            {
                rects.at((i * rows) + (k * islandSize) + j).x = (xOffset + j + k * islandSpacer);
				rects.at((i * rows) + (k * islandSize) + j).y = (i);
                ofSetColor(_theColors[(i * rows) + (k * islandSize) + j]);
                //ofSetColor(128);
                //ofPoint(j + k*islandSpacer, i);
				
                
                ofRect(rects.at((i * rows) + (k * islandSize) + j));
                
                //ofSetColor(255,0,0);
                //ofDrawBitmapString(ofToString((i * rows) + (k * islandSize) + j), rects.at((i * rows) + (k * islandSize) + j).x, rects.at((i * rows) + (k * islandSize) + j).y);
            }
        }
    }
    
  	glPopMatrix();
}

//--------------------------------------------------------------
void TCPClient::keyPressed(int key)
{
    if (key == 'c')
        storeText.clear();
    if (key == 'p') //toggle pause
    {
        bPause = !bPause;
        bStop = false;
        if(bPause)
            pauseTime = ofGetElapsedTimef() - elapsedTime;
        else
            elapsedTime = ofGetElapsedTimef() - pauseTime;
    }
    if (key == 'o') //toggle stop
    {
        //stop
        bStop = !bStop;
        bPause = false;
        elapsedTime = ofGetElapsedTimef();
    }
}

//--------------------------------------------------------------
void TCPClient::keyReleased(int key){
    
}

//--------------------------------------------------------------
void TCPClient::mouseMoved(int x, int y ){
    
}

//--------------------------------------------------------------
void TCPClient::mouseDragged(int x, int y, int button){
    
}

//--------------------------------------------------------------
void TCPClient::mousePressed(int x, int y, int button){
    
}

//--------------------------------------------------------------
void TCPClient::mouseReleased(int x, int y, int button){
    
}

//--------------------------------------------------------------
void TCPClient::windowResized(int w, int h){
    
}

//--------------------------------------------------------------
void TCPClient::gotMessage(ofMessage msg){
    
}

//--------------------------------------------------------------
void TCPClient::dragEvent(ofDragInfo dragInfo){ 
    
}