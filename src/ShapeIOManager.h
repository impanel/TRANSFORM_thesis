/*
 *  ReliefIOManager.h
 *  Relief
 *
 *  Created by Daniel Leithinger on 3/3/10.
 *  Copyright 2010 MIT Media Lab. All rights reserved.
 *
 */

#ifndef _SHAPE_IO_MANAGER
#define _SHAPE_IO_MANAGER

#include "ofMain.h"
#include "ShapeSerial.h"
#include "Constants.h"
//#include "ofxXmlSettings.h"

class ReliefBoard{
public:
    unsigned char pinCoordinates[NUM_PINS_ARDUINO][2]; // what physical x and y does each pin on the board map to?
    bool invertHeight; // is it mounted upside down? if so, the height is inverted
    double rollingAverageHeight[NUM_PINS_ARDUINO];
    bool pinIsIdle[NUM_PINS_ARDUINO];
    unsigned char lastFramePinHeight[NUM_PINS_ARDUINO];
    unsigned char currentFramePinHeight[NUM_PINS_ARDUINO];
    int serialConnection; // what serial connection is it on?
    bool isBoardIdle() {
        bool boardIsIdle = true;
        for (int i = 0; i < NUM_PINS_ARDUINO; i++) {
            if (!pinIsIdle[i]) boardIsIdle = false;
        }
        return boardIsIdle;
    };
    
};

class ShapeIOManager {
private:

    float _gain_P;
    float _gain_I;
    int _max_I;
    int _deadZone;
    int _maxSpeed;
    
	ShapeSerial * mSerialConnections [NUM_SERIAL_CONNECTIONS];
	ReliefBoard reliefBoardValues [NUM_ARDUINOS];
	//ofxXmlSettings mXML;
    
    bool mUseSerial = false;
    void openSerialConnections();
    void closeSerialConnections();
    void clipAllValuesToBeWithinRange();
    void readPinHeightsFromBoards();
    void prepDepthDataForArduino();
    
    void sendAllValuesToTable(); // called evey x seconds in update fn to fix "broken" pins
    long resetInterval = PID_RESET_INTERVALL; // milliseconds to reset table
    unsigned long long startTime = ofGetElapsedTimeMillis();
	
    bool tableValuesHaveChanged = false;
    
public:

    void snycValuesToTable();
    
    // setters for table config variables
    void set_gain_p(float value);
    void set_gain_i(float value);
    void set_max_i(int value);
    void set_deadzone(int value);
    void set_max_speed(int value);
    
    // the array that stores the pin display height values
    unsigned char pinHeightToRelief [RELIEF_SIZE_X][RELIEF_SIZE_Y];
    unsigned char pinHeightFromRelief [RELIEF_SIZE_X][RELIEF_SIZE_Y];
    
    ShapeIOManager();
    ~ShapeIOManager();
    void connectToTable();
    void disconnectFromTable();
    void disconnectFromTableWithoutPinReset();
    
    void update(ofFbo buffer);
    
    // @todo standardize names
	void sendPinHeightToRelief();
	void sendAndReceivePinHeight();
    void sendHeightToBoardAndRequestFeedback(unsigned char boardId, unsigned char value[NUM_PINS_ARDUINO], int serialConnection);
	void sendValueToAllBoards(unsigned char termId, unsigned char value);
	void sendValuesToBoard(unsigned char termId, unsigned char boardId, unsigned char value[NUM_PINS_ARDUINO], int serialConnection);
    void setBoardConfiguration();
    void resetPinsToValue(int value);

};

#endif
