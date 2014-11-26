/*
 *  ReliefIOManager.cpp
 *  Relief
 *
 *  Created by Daniel Leithinger on 3/3/10.
 *  Copyright 2010 MIT Media Lab. All rights reserved.
 *
 */

#include "ShapeIOManager.h"

//--------------------------------------------------------------
//
// Create new shapeIOManager class
//
// loads settings, but you must manually connect / disconnect
// from the table.
//
//--------------------------------------------------------------
ShapeIOManager::ShapeIOManager() {
    setBoardConfiguration();
    mUseSerial = false;
}

//--------------------------------------------------------------
//
// Destroy
//
//--------------------------------------------------------------
ShapeIOManager::~ShapeIOManager() {
    disconnectFromTable();
}

//--------------------------------------------------------------
//
// Connects to table, which opens serial connection to boards
// and sets global variable, which effects many
// other functions within class
//
//--------------------------------------------------------------
void ShapeIOManager::connectToTable() {
    if(mUseSerial == true) return;
    openSerialConnections();
    mUseSerial = true;
}

//--------------------------------------------------------------
//
// Disconnects from table the proper way
//
// resets pins heights to 0,
// sets boards to "off",
// and then closes the serial connection.
//
// @note should be called when exiting the app
//
//--------------------------------------------------------------
void ShapeIOManager::disconnectFromTable() {
    if(mUseSerial == false) return;
    resetPinsToValue(0);
    ofSleepMillis(1000);
    sendValueToAllBoards(TERM_ID_MAXSPEED, (unsigned char) 0); // effectively "turns off" pins by setting speed to 0
    mUseSerial = false; // stops commands to serial from writing, effectively stops app from overriding manager
    ofSleepMillis(1000);
    closeSerialConnections();
}

void ShapeIOManager::disconnectFromTableWithoutPinReset(){
    if(mUseSerial == false) return;
    ofSleepMillis(1000);
    sendValueToAllBoards(TERM_ID_MAXSPEED, (unsigned char) 0); // effectively "turns off" pins by setting speed to 0
    mUseSerial = false; // stops commands to serial from writing, effectively stops app from overriding manager
    ofSleepMillis(1000);
    closeSerialConnections();
}

//--------------------------------------------------------------
//
// Opens serial connection and stores that connection in array
// delete the array value to later close the connection
//
//--------------------------------------------------------------
void ShapeIOManager::openSerialConnections() {
    mSerialConnections[0] = new ShapeSerial(SERIAL_PORT_0, SERIAL_BAUD_RATE);
    mSerialConnections[1] = new ShapeSerial(SERIAL_PORT_1, SERIAL_BAUD_RATE);
    mSerialConnections[2] = new ShapeSerial(SERIAL_PORT_2, SERIAL_BAUD_RATE);
    mSerialConnections[3] = new ShapeSerial(SERIAL_PORT_3, SERIAL_BAUD_RATE);
    mSerialConnections[4] = new ShapeSerial(SERIAL_PORT_4, SERIAL_BAUD_RATE);
    mSerialConnections[5] = new ShapeSerial(SERIAL_PORT_5, SERIAL_BAUD_RATE);
}

//--------------------------------------------------------------
//
// Close all serial connections to the table
//
//--------------------------------------------------------------
void ShapeIOManager::closeSerialConnections() {
    for (int i = 0; i < NUM_SERIAL_CONNECTIONS; i++) {
        delete mSerialConnections[i];
    }
}

//--------------------------------------------------------------
//
// Sets all pins to given value
//
// @note pin heights will be adjusted to be within clipping range
// regardless of value set
//
//--------------------------------------------------------------
void ShapeIOManager::resetPinsToValue(int value) {
    for (int i = 0; i < RELIEF_SIZE_X; i ++) {
		for (int j = 0; j < RELIEF_SIZE_Y; j ++) {
			pinHeightToRelief[i][j] = value;
		}
    }
    sendPinHeightToRelief();
}


//--------------------------------------------------------------
//
// Clips all values to be within range
//
//--------------------------------------------------------------
void ShapeIOManager::clipAllValuesToBeWithinRange() {
    for (int i = 0; i < RELIEF_SIZE_X; i ++) {
		for (int j = 0; j < RELIEF_SIZE_Y; j ++) {
			if (pinHeightToRelief[i][j] <= LOW_THRESHOLD) {
				pinHeightToRelief[i][j] = (unsigned char) LOW_THRESHOLD;
			}
			else if (pinHeightToRelief[i][j] >= HIGH_THRESHOLD) {
				pinHeightToRelief[i][j] = (unsigned char) HIGH_THRESHOLD;
			}
		}
    }
}


//--------------------------------------------------------------
//
// Given multiarray of values, sends all values to the table
//
//--------------------------------------------------------------
void ShapeIOManager::sendPinHeightToRelief() {
    if(mUseSerial == false) return;
    clipAllValuesToBeWithinRange();
    prepDepthDataForArduino();
    for (unsigned char i = 0; i < NUM_ARDUINOS; i++) {
        sendValuesToBoard(TERM_ID_HEIGHT, i + 1, reliefBoardValues[i].currentFramePinHeight, reliefBoardValues[i].serialConnection);
    }
}

//--------------------------------------------------------------
//
// Given multiarray of values, sends all values to the table AND recieves
// communication about current pin height
//
//--------------------------------------------------------------
void ShapeIOManager::sendAndReceivePinHeight() {
    
    if(mUseSerial == false) return;
    clipAllValuesToBeWithinRange();
    prepDepthDataForArduino();
    for (unsigned char i = 0; i < NUM_ARDUINOS; i++) {
        sendHeightToBoardAndRequestFeedback(i + 1, reliefBoardValues[i].currentFramePinHeight, reliefBoardValues[i].serialConnection);
    }
    readPinHeightsFromBoards(); // gets actual pin heights from arduino boards
    
    // temp way to store these values
    // since sendAndReceivePinHeight gets called in update method
    // this will save the values locally
    //memcpy(pinHeightToRelief, pinHeightToRelief, sizeof(unsigned char) * RELIEF_SIZE_X * RELIEF_SIZE_Y);
    //memcpy(pinHeightFromRelief, pinHeightFromRelief, sizeof(unsigned char) * RELIEF_SIZE_X * RELIEF_SIZE_Y);
}

//-----------------------------------------------------------
//
// gets pin height
//
//-----------------------------------------------------------
void ShapeIOManager::update(ofFbo buffer){
    
    unsigned char* pixels;
    ofPixels tempPixels;
    buffer.readToPixels(tempPixels);
    //Rotate to align all coordinate systems
    //tempPixels.rotate90(1);
    //tempPixels.mirror(true, false);
    tempPixels.mirror(false, true);
    pixels = tempPixels.getPixels();
    
    // it is necessary to remap the pin size from the physical area to the actual number of pins,
    // as the table has 3 separate blocks of actuators, which are spaced apart from each other
    for (int j = 0; j < RELIEF_PHYSICAL_SIZE_Y; j ++) {
        
        // first block
        for (int i = 0; i < PINBLOCK_0_WIDTH; i ++) {
            int y = j;
            int x = RELIEF_PHYSICAL_SIZE_X - 1 - i - PINBLOCK_0_X_OFFSET;
            int heightMapValue = pixels[((x + (y * RELIEF_PHYSICAL_SIZE_X)) * 4)];
            pinHeightToRelief[i][j] = heightMapValue;
        }
        
        // second block
        for (int i = 0; i < PINBLOCK_1_WIDTH; i ++) {
            int y = j;
            int x = RELIEF_PHYSICAL_SIZE_X - 1 - i - PINBLOCK_1_X_OFFSET;
            int heightMapValue = pixels[((x + (y * RELIEF_PHYSICAL_SIZE_X)) * 4)];
            pinHeightToRelief[i + PINBLOCK_0_WIDTH][j] = heightMapValue;
        }
        
        // third block
        for (int i = 0; i < PINBLOCK_2_WIDTH; i ++) {
            int y = j;
            int x = RELIEF_PHYSICAL_SIZE_X - 1 - i - PINBLOCK_2_X_OFFSET;
            int heightMapValue = pixels[((x + (y * RELIEF_PHYSICAL_SIZE_X)) * 4)];
            pinHeightToRelief[i + PINBLOCK_0_WIDTH + PINBLOCK_1_WIDTH][j] = heightMapValue;
        }
    }
    
    // send the height map to the hardware interface
	//mIOManager->sendPinHeightToRelief(mPinHeightToRelief);
    sendAndReceivePinHeight();
    
    if(ofGetElapsedTimeMillis() >= startTime+resetInterval) {
        //cout << "RESETTING TABLE VALUES" << endl;
        startTime = ofGetElapsedTimeMillis();
        sendAllValuesToTable();
    } else {
        if(tableValuesHaveChanged) {
            sendAllValuesToTable();
            tableValuesHaveChanged = false;
            cout << "VALUES HAVE CHANGED, updating table" << endl;
        }
    }
}


//--------------------------------------------------------------
//
// Copy data from data structure of image (2 dimensional array)
// to that of ardunios AND then flip the values where needed
// to match arduinos configuration
//
//--------------------------------------------------------------
void ShapeIOManager::prepDepthDataForArduino() {
    for (unsigned char i = 0; i < NUM_ARDUINOS; i++) { // iterate through each board
        for (int j = 0; j < NUM_PINS_ARDUINO; j++) {
            
            // copy the height values from the current frame values to the arduino pins
            reliefBoardValues[i].currentFramePinHeight[j] = pinHeightToRelief[reliefBoardValues[i].pinCoordinates[j][0]][reliefBoardValues[i].pinCoordinates[j][1]];
            
            // invert the values if needed (this affects boards mounted upside down)
            if (reliefBoardValues[i].invertHeight) reliefBoardValues[i].currentFramePinHeight[j] = 255 - reliefBoardValues[i].currentFramePinHeight[j];
        }
    }
}


//--------------------------------------------------------------
//
// Reads actual pin heights from the boards
//
//--------------------------------------------------------------
void ShapeIOManager::readPinHeightsFromBoards() {
    // receive the pin heights from the table
    for (int i= 0; i< NUM_SERIAL_CONNECTIONS; i++) {
        while(mSerialConnections[i]->hasNewMessage()) {
            unsigned char messageContent[MSGS_SIZE_RECEIVE];
            mSerialConnections[i]->readMessage(messageContent);
            if(messageContent[0] == 253) {
                int boardAddress = messageContent[1] - 1;
                if (boardAddress >= 0 && boardAddress <= NUM_ARDUINOS) {
                    for (int j = 0; j < 6; j++) {
                        int height = messageContent[j+2];
                        if (reliefBoardValues[boardAddress].invertHeight) height = 255 - height;
                        
                        if (height >= 0 && height <= 255) {
                            int x = reliefBoardValues[boardAddress].pinCoordinates[j][0];
                            int y = reliefBoardValues[boardAddress].pinCoordinates[j][1];
                            pinHeightFromRelief[x][y] = height;
                        }
                    }
                }
            }
        }
    }
}


//--------------------------------------------------------------
//
// 8 byte and 10 byte versions of the same thing
// when recieving information from the kinect, we need a 10 byte
// message, compared to 8 when sending only.
//
//--------------------------------------------------------------

// 10 byte
void ShapeIOManager::sendHeightToBoardAndRequestFeedback(unsigned char boardId, unsigned char value[NUM_PINS_ARDUINO], int serialConnection) {
    unsigned char messageContents[10];
    messageContents[0] = TERM_ID_HEIGHT_FEEDBACK;
    messageContents[1] = boardId;
    for (int j = 0; j < NUM_PINS_ARDUINO; j++) {
        messageContents[j + 2] = value[j];
    }
    // xtra bytes!
    messageContents[8] = 255;
    messageContents[9] = 255;
    
    if(mUseSerial) {
        mSerialConnections[serialConnection]->writeMessageRequestFeedback(messageContents);
    }
}

// 8 byte
void ShapeIOManager::sendValuesToBoard(unsigned char termId, unsigned char boardId, unsigned char value[NUM_PINS_ARDUINO], int serialConnection) {
    unsigned char messageContents[8];
    messageContents[0] = termId;
    messageContents[1] = boardId;
    for (int j = 0; j < NUM_PINS_ARDUINO; j++) {
        messageContents[j + 2] = value[j];
    }
    if(mUseSerial) {
        mSerialConnections[serialConnection]->writeMessage(messageContents);
    }
}


//--------------------------------------------------------------
//
// Send a value to all boards, givin term id and term value
//
//--------------------------------------------------------------
void ShapeIOManager::sendValueToAllBoards(unsigned char termId, unsigned char value) {
    
    unsigned char messageContents[8];
    messageContents[0] = (unsigned char) termId;
    messageContents[1] = (unsigned char) 0;
    for (int i = 0; i < NUM_PINS_ARDUINO; i++) {
        messageContents[i + 2] = (unsigned char) value;
    }
    
    if(mUseSerial) {
        for (int i = 0; i < NUM_SERIAL_CONNECTIONS; i ++) {
            mSerialConnections[i]->writeMessage(messageContents);
        }
    }
}


//--------------------------------------------------------------
//
// Setters for table config
//
//--------------------------------------------------------------
void ShapeIOManager::set_gain_p(float value){
    if(value != _gain_P) tableValuesHaveChanged = true;
    _gain_P = value;
    //sendValueToAllBoards(TERM_ID_GAIN_P, (unsigned char) (_gain_P * 25));
}

void ShapeIOManager::set_gain_i(float value){
    if(value != _gain_I) tableValuesHaveChanged = true;
    _gain_I = value;
    //sendValueToAllBoards(TERM_ID_GAIN_I, (unsigned char) (_gain_I * 100));
}

void ShapeIOManager::set_max_i(int value){
    if(value != _max_I) tableValuesHaveChanged = true;
    _max_I = value;
    //sendValueToAllBoards(TERM_ID_MAX_I, (unsigned char)_max_I);
}

void ShapeIOManager::set_deadzone(int value){
    if(value != _deadZone) tableValuesHaveChanged = true;
    _deadZone = value;
    //sendValueToAllBoards(TERM_ID_DEADZONE, (unsigned char)_deadZone);
}

void ShapeIOManager::set_max_speed(int value){
    if(value != _maxSpeed) tableValuesHaveChanged = true;
    _maxSpeed = value;
    //sendValueToAllBoards(TERM_ID_MAXSPEED, (unsigned char)(_maxSpeed/2));
}

//--------------------------------------------------------------
//
// Sends all current table config vars to the table
//
// this method should be called periodically to reset the table and fix
// pins that appear broken (but really have invalid values that have
// changed over time because of firmware issues or connection noise)
//
//--------------------------------------------------------------
void ShapeIOManager::sendAllValuesToTable() {
    sendValueToAllBoards(TERM_ID_GAIN_P, (unsigned char) (_gain_P * 25));
    sendValueToAllBoards(TERM_ID_GAIN_I, (unsigned char) (_gain_I * 100));
    sendValueToAllBoards(TERM_ID_MAX_I, (unsigned char)_max_I);
    sendValueToAllBoards(TERM_ID_DEADZONE, (unsigned char)_deadZone);
    sendValueToAllBoards(TERM_ID_MAXSPEED, (unsigned char)(_maxSpeed/2));
}

void ShapeIOManager::snycValuesToTable() {
    sendAllValuesToTable();
}


//--------------------------------------------------------------
//
// Custom function per installation to setup table.
//
// @todo this should be within the reliefApp???
//
//--------------------------------------------------------------
void ShapeIOManager::setBoardConfiguration() {
    
    // set up coordinates for 
    for (int i = 0; i < NUM_ARDUINOS; i++) {
        
        // determine which serial connection each board is on:
        // every 3rd and 4th board is on the second
        if(i < 64) {
            reliefBoardValues[i].serialConnection = ((i/2)%2 == 0) ? 0 : 1;
        }
        else if(i < 128) {
            reliefBoardValues[i].serialConnection = ((i/2)%2 == 0) ? 2 : 3;
        }
        else {
            reliefBoardValues[i].serialConnection = ((i/2)%2 == 0) ? 4 : 5;
        }
        
        // every 5th to 8th board is mounted upside down, so invert the height
        reliefBoardValues[i].invertHeight = (((i/4)%2) == 0) ? false : true;
        
        for (int j = 0; j < NUM_PINS_ARDUINO; j++) {
            int currentRow = (int)(i / 4);
            int currentColumn = j + (i%4 * 6);
            reliefBoardValues[i].lastFramePinHeight[j] = 0;
            reliefBoardValues[i].currentFramePinHeight[j] = 0;
            reliefBoardValues[i].rollingAverageHeight[j] = 0;
            reliefBoardValues[i].pinIsIdle[j] = true;
            reliefBoardValues[i].pinCoordinates[j][0] = currentRow;
            reliefBoardValues[i].pinCoordinates[j][1] = currentColumn;
        }
        
        if(((i/2)%2 == 0)) {
            int pinCoordinateRows[NUM_PINS_ARDUINO];
            
            // invert pin order if the boards are mounted rotated
            for (int count = 0; count < NUM_PINS_ARDUINO; count++){
                pinCoordinateRows[NUM_PINS_ARDUINO - count - 1] = reliefBoardValues[i].pinCoordinates[count][1];
            }
            for (int count = 0; count < NUM_PINS_ARDUINO; count++){
                reliefBoardValues[i].pinCoordinates[count][1] = pinCoordinateRows[count];
            }
            
            // also invert the pin height again if they are:
            reliefBoardValues[i].invertHeight = !reliefBoardValues[i].invertHeight;
        }
    }
    
    // print out the mapping
    
    for (int i = 0; i < NUM_ARDUINOS; i++) {
        for (int j = 0; j < NUM_PINS_ARDUINO; j++) {
            printf("%d,%d(%d); ", reliefBoardValues[i].pinCoordinates[j][0], reliefBoardValues[i].pinCoordinates[j][1], reliefBoardValues[i].invertHeight);
        }
        printf("\n");
    }
}