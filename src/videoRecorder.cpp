#include "videoRecorder.h"


//--------------------------------------------------------------
void VideoRecorder::setup(unsigned char * pixels, int w, int h)
{
	
    
    
    
    //movieExporter.setRecordingArea(100,100,100,100);
    
    
    testImage.loadImage("kate-1024-768.jpg");
  
	 movieExporter.setup();
    
    movieExporter.setPixelSource(pixels, w, h);
}

//--------------------------------------------------------------
void VideoRecorder::update()
{
    //movieExporter.setPixelSource(pixels, w, h);
}

//--------------------------------------------------------------
void VideoRecorder::draw()
{
//	ofSetColor(255, 255, 255);
//	testImage.draw(0, 0, ofGetWidth(), ofGetHeight());
//	ofEllipse(.5f * ofGetWidth() + 200.f * cosf(ofGetElapsedTimef()), .5f * ofGetHeight() + 200.f * sinf(ofGetElapsedTimef()), 100, 100);
//	int size = max(1000 - ofGetFrameNum(), 0);
//	ofEllipse(.5f * ofGetWidth(), .5f * ofGetHeight(), size, size);
    ofPushStyle();
	ofSetColor(0, 255, 0);
	ofDrawBitmapString(ofToString(ofGetFrameRate()), 10, 35);
	ofDrawBitmapString(ofToString(ofGetElapsedTimef()), 10, 55);
	if (movieExporter.isRecording())
	{
		ofSetColor(255, 0, 0);
		ofDrawBitmapString("Press space to stop recording", 10, 15);
	}
	else ofDrawBitmapString("Press space to start recording", 10, 15);
    ofPopStyle();
}

//--------------------------------------------------------------
void VideoRecorder::keyPressed(int key)
{
//	switch (key)
//	{
//		case ' ':
//			if (movieExporter.isRecording()) movieExporter.stop();
//			else movieExporter.record("videos/cats");
//			break;
//	}
}

//--------------------------------------------------------------
void VideoRecorder::keyReleased(int key){
    
}

//--------------------------------------------------------------
void VideoRecorder::mouseMoved(int x, int y ){
    
}

//--------------------------------------------------------------
void VideoRecorder::mouseDragged(int x, int y, int button){
    
}

//--------------------------------------------------------------
void VideoRecorder::mousePressed(int x, int y, int button){
    
}

//--------------------------------------------------------------
void VideoRecorder::mouseReleased(int x, int y, int button){
    
}

//--------------------------------------------------------------
void VideoRecorder::windowResized(int w, int h){
    
}

//--------------------------------------------------------------
void VideoRecorder::gotMessage(ofMessage msg){
    
}

//--------------------------------------------------------------
void VideoRecorder::dragEvent(ofDragInfo dragInfo){
    
}