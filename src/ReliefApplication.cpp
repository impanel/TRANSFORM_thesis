#include "ReliefApplication.h"

//--------------------------------------------------------------
void ReliefApplication::setup(){

    //ofSetLogLevel(OF_LOG_VERBOSE);
    
    ofSetFrameRate(30);
    ofSetWindowShape(1280, 800); // sized for 13 inch macbook retnia
    ofSetCircleResolution(64); // nice circle for table simulation

    mImageWarper = new ImageWarper(0,0,RELIEF_PROJECTOR_SIZE_X,RELIEF_PROJECTOR_SIZE_Y);
    mImageWarper->loadSettings("settings_warp_points.xml");
    
    
    // setup kinect if using
    // @todo we only want to setup if connected
    // @note currently if you change the kinect setting you must restart
#ifdef KINECT
    mKinectTracker.setup();

    
    // module for saving recordings from kinect
    // @todo matt refactor to class
    movieExporter.setup(KINECT_X * 2, KINECT_Y);
    movieExporter.setPixelSource(mKinectTracker.getRecordingPixels(), KINECT_X * 2, KINECT_Y);
    
    kinectVideoPlayer.setup("kinect_videos");
#endif
	// initialize communication with the pin display
	mIOManager = new ShapeIOManager();
    
    // table simulator
    tableSimulation = new TableSimulator(mIOManager);
    
    // setup default valus for pins
    // @todo move to config file?
    gain_P = 0.5;
    gain_I = 0;
    max_I = 60;
    deadZone = 0;
    maxSpeed = 220;
    
    // allocate general images
    // @note RELIEF_PHYSICAL vs RELIEF_PROJECTOR
    pinDisplayImage.allocate(RELIEF_PROJECTOR_SIZE_X, RELIEF_PROJECTOR_SIZE_Y, GL_RGB);
    pinHeightMapImage.allocate(RELIEF_PROJECTOR_SIZE_X, RELIEF_PROJECTOR_SIZE_Y, GL_RGBA);
    pinHeightMapImageSmall.allocate(RELIEF_PHYSICAL_SIZE_X, RELIEF_PHYSICAL_SIZE_Y, GL_RGBA);
    pinHeightMapImageSmall.getTextureReference().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
    
    //#phil
    pinHeightMapImage2.allocate(RELIEF_PROJECTOR_SIZE_X, RELIEF_PROJECTOR_SIZE_Y, GL_RGBA);
    
    // allocate warping images
    // @todo matt do we need this since warping is seperate class?
    cvColorImage.allocate(KINECT_X, KINECT_Y);
    cvWarpedImage.allocate(1020, 240);
    
    // initalizes all shape objects and adds them to vectors
    initalizeShapeObjects();
    registerShapeObjectNamesForGui();
    
    // setup guis
    setupEasyGui();
    setupVideosDropdown();
    setupImagesDropdown();
    setupKinectRecordingsDropdown();
    //setupTableGui();
    
    // load gui settings
    easyGui->loadSettings("kinect_settings.xml");
    
    // set default shape object
    // @todo matt refactor this because it requires setting gui
    // options, variables, and more and needs to be centralized
    mCurrentShapeObject = mCalmShapeObject;
}


//--------------------------------------------------------------
//
// Initalize Shape objects, passking kinect and warper if needed
// then add to shape object vector.
//
// @todo matt refactor this so shape objects register themselves
//
//--------------------------------------------------------------
void ReliefApplication::initalizeShapeObjects() {
    
    // render depth map from kinect
    mKinectShapeObject = new KinectShapeObject();
    mKinectShapeObject->setImageWarper(mImageWarper);
    mKinectShapeObject->setKinectTracker(&mKinectTracker);
    
    // machine animations
    mMachineAnimationShapeObject = new MachineAnimationShapeObject();
    
    // still, no motion. Default shape object
    // can also be used to "stop" other shape objects
    mCalmShapeObject = new CalmShapeObject();
    
    mImageShapeObject = new ImageShapeObject();
    mImageShapeObject->setup("images");
    
    mTCPShapeObject = new TCPClient();
    mTCPShapeObject->setup();
    
    mHandShapeObject = new HandShapeObject();
    mHandShapeObject->setImageWarper(mImageWarper);
    mHandShapeObject->setKinectTracker(&mKinectTracker);
    
    // push all shape objects to a buffer
    allShapeObjects.push_back(mMachineAnimationShapeObject);
    allShapeObjects.push_back(mKinectShapeObject);
    allShapeObjects.push_back(mCalmShapeObject);
    allShapeObjects.push_back(mImageShapeObject);
    allShapeObjects.push_back(mHandShapeObject);
    allShapeObjects.push_back(mTCPShapeObject);
}


//--------------------------------------------------------------
//
// For each registered shape object, add name to vector for use by GUI
// which needs a vector of strings to work.
//
//--------------------------------------------------------------
void ReliefApplication::registerShapeObjectNamesForGui() {
    
    for (vector<ShapeObject*>::iterator it = allShapeObjects.begin() ; it != allShapeObjects.end(); ++it) {
        string name = (*it)->get_shape_name();
        allsShapeObjectsNames.push_back(name);
    }
}

//--------------------------------------------------------------
//
// Setup Easy Gui
//
//--------------------------------------------------------------
void ReliefApplication::setupEasyGui() {
    easyGui = new ofxUICanvas;
    easyGui->setPosition(ofGetWidth() - 210, 0);
    easyGui->setHeight(500);
    easyGui->setName("Easy Control");
    easyGui->addLabel("Easy Control");
    
    //Toggle Buttons
    easyGui->addToggle("Toggle Use Table", true);
    easyGui->addToggle("Use Kinect Mask", &mKinectTracker.useMask);

    
    //Mode Buttons
    vector <string> modes;
    modes.push_back("none");
    modes.push_back("videos");
    modes.push_back("images");
    modes.push_back("TCP");
    
    ofxUIRadio *easyRadio = easyGui->addRadio("MODES", modes);
    easyRadio->activateToggle("none");
    
    //Kinect modes
    vector <string> kinectModes;
    kinectModes.push_back("Live Kinect");
    kinectModes.push_back("Recorded Kinect");
    
    ofxUIRadio *kinectRadio = easyGui->addRadio("KINECT", kinectModes);
    easyRadio->activateToggle("Live Kinect");
    
    //Sliders
    easyGui->addIntSlider("Near Threshold", 0, 255, &mKinectTracker.mNearThreshold);
    easyGui->addIntSlider("Far Threshold", 0, 255, &mKinectTracker.mFarThreshold);
    easyGui->addIntSlider("Contour Min Size", 0, 15000, &mKinectTracker.mContourMinimumSize);
    
    easyGui->addSlider("P TERM", 0, 3, &gain_P);
    easyGui->addSlider("I TERM", 0, 1, &gain_I);
    easyGui->addIntSlider("Max I", 0, 200, &max_I);
    easyGui->addIntSlider("deadzone", 0, 20, &deadZone);
    easyGui->addIntSlider("Max Speed", 0, 220, &maxSpeed);
    
    ofAddListener(easyGui->newGUIEvent, this, &ReliefApplication::guiEvent);
    easyGui->autoSizeToFitWidgets();
    //tableGui->loadSettings("table_settings.xml");
}

//--------------------------------------------------------------

void ReliefApplication::setupVideosDropdown()
{
    videosDropdown = new ofxUICanvas;
    videosDropdown->setPosition(ofGetWidth() - 4 * 210, 0);
    videosDropdown->setName("Videos");
    videosDropdown->addLabel("Videos");
   
    //Dropdown Menu for Videos
    ofxUIDropDownList *ddl = videosDropdown->addDropDownList("video list", mMachineAnimationShapeObject->getLoadedVideoFilenames());
    ddl->setAllowMultiple(false);
    ddl->setAutoClose(true);
    
    ofAddListener(videosDropdown->newGUIEvent, this, &ReliefApplication::guiEvent);
    videosDropdown->autoSizeToFitWidgets();
}

//--------------------------------------------------------------

void ReliefApplication::setupImagesDropdown()
{
    imagesDropdown = new ofxUICanvas;
    imagesDropdown->setPosition(ofGetWidth() - 3 * 210, 0);
    imagesDropdown->setName("Images");
    imagesDropdown->addLabel("Images");
    
    //Dropwdown Menu for Images
    ofxUIDropDownList *ddl = imagesDropdown->addDropDownList("images list", mImageShapeObject->getLoadedImagesFilenames());
    ddl->setAllowMultiple(false);
    ddl->setAutoClose(true);
    
    ofAddListener(imagesDropdown->newGUIEvent, this, &ReliefApplication::guiEvent);
    imagesDropdown->autoSizeToFitWidgets();
}

//--------------------------------------------------------------

void ReliefApplication::setupKinectRecordingsDropdown()
{
    kinectRecordingsDropdown = new ofxUICanvas;
    kinectRecordingsDropdown->setPosition(ofGetWidth() - 2 * 210, 0);
    kinectRecordingsDropdown->setName("Kinect Recordings");
    kinectRecordingsDropdown->addLabel("Kinect Recordings");
    
    //Dropdown Menu for Recorded Kinect Videos
    ofxUIDropDownList *ddl = kinectRecordingsDropdown->addDropDownList("RECORDING SOURCE", kinectVideoPlayer.loadedVideoFilenames);
    ddl->setAllowMultiple(false);
    ddl->setAutoClose(true);
    
    ofAddListener(kinectRecordingsDropdown->newGUIEvent, this, &ReliefApplication::guiEvent);
    kinectRecordingsDropdown->autoSizeToFitWidgets();
}

//--------------------------------------------------------------
void ReliefApplication::update(){
    
    // if paused dont update
    if(isPaused) return;
    
    // creates warping preview image
    // @note this is not a method of the kinect tracker or warper because its
    // specific to this app. Including it in the kinect would mean importaing the
    // wavy class and vice versa. The warping is still happening - this simply
    // provides a visual que to the user
    mImageWarper->warpIntoImage(mKinectTracker.colorImg, cvWarpedImage);
    
    // support recorded kinect videos
    // @todo matt refactor
    if(useRecording && kinectVideoPlayer.isPlaying()) {
        mKinectTracker.playFromRecording = true;
        mKinectTracker.updateCurrentFrame(kinectVideoPlayer.getColorPixels(),
                                          KINECT_X*2,
                                          KINECT_Y);
        kinectVideoPlayer.update();
        mKinectTracker.update();
    } else {
        mKinectTracker.playFromRecording = false;
        mKinectTracker.update();
    }
    
    // @todo matt do we need these?
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    //app timebase, to send to all animatable objets
    
    
    // time since last frame, used by the shape objects
    float dt = 1.0f / ofGetFrameRate();
    
    mCurrentShapeObject->update(dt);
    // @note the last shape object values will win out here.
    mCurrentShapeObject->setTableValuesForShape(mIOManager);

    // buffer drawing
    // @todo matt this needs refactoring, along with all shape objects
    // what's happening here is that renderShape() method works more like update
    // it performs the calculations and draws into an allPiexls buffer
    // which is then used for the small image
    // we need to clean this up in the shape objects
    
    pinHeightMapImage.begin();
    ofBackground(0);
    ofSetColor(255);
    mCurrentShapeObject->renderShape();
    pinHeightMapImage.end();
    
    //--------------------------------------------------------------
    // draw the big heightmap image into a small heightmap image and send it off to the table
    pinHeightMapImageSmall.begin();
    ofBackground(0);
    ofSetColor(255);
    
    mCurrentShapeObject->renderGraphics(0, 0, RELIEF_PHYSICAL_SIZE_X, RELIEF_PHYSICAL_SIZE_Y);
    
    pinHeightMapImageSmall.end();
    //--------------------------------------------------------------
    
    // updates the table with the small rendered pinHeightMapImage
    // will use the last height map buffer image
    mIOManager->update(pinHeightMapImageSmall);
    
    // update the table simulation, which is the rendered table graphic
    tableSimulation->update();
}

//--------------------------------------------------------------
//
// Main app draw function. Call other draw functions here
// and only draw basic stuff. This is where we set different drawing groups
// based on ux mode.
//
//--------------------------------------------------------------
void ReliefApplication::draw(){
    
    ofBackground(100, 100, 100);
    
    // warping
	int xVidPos			= 0;
	int yVidPos			= 0;
    
    if(currentScreen == "kinect")
    {
        //cvColorImage.draw(xVidPos,yVidPos);
        mKinectTracker.drawColorImage(0,0, KINECT_X, KINECT_Y);
        if(mKinectTracker.useMask) mKinectTracker.mask.draw(xVidPos,yVidPos);
        drawBitmapString("Masked Kinect 8-bit Image", 10, 10);
        
        cvWarpedImage.draw(0,KINECT_Y, cvWarpedImage.width/2, cvWarpedImage.height/2);
        mKinectTracker.drawDepthThreshedDiff(KINECT_X, 0, KINECT_X/2, KINECT_Y/2);
        drawBitmapString("Threshold Depth Image", 10, KINECT_Y/2);
        
        mKinectTracker.drawThresholdImage(KINECT_X, KINECT_Y/2, KINECT_X/2, KINECT_Y/2);
        
        mImageWarper->drawWarpPoints();
    }
    
    else if(currentScreen == "shapeobject")
    {
        pinDisplayImage.draw(2, 2, 1020*0.6, 240*0.6);
        drawBitmapString("Pin Display Color Image (for projection if needed)", 10, 10);
        
        pinHeightMapImage.draw(2, 240*0.6+10, 1020*0.6, 240*0.6);
        drawBitmapString("Pin Height Map Image (the actual grey values send to the table)", 10, 240*0.6+20);
        
        mKinectTracker.drawThresholdImage(2, 240*0.6*2+20, KINECT_X * 0.5, KINECT_Y * 0.5);
        drawBitmapString("Kinect Threshold Image", 10, 240*0.6*2+30);
        
        //actual size height map image (102 * 24)
        pinHeightMapImage.draw(KINECT_Y*0.5 + 100, 240*0.6*2+30, RELIEF_PHYSICAL_SIZE_X, RELIEF_PHYSICAL_SIZE_Y);
        
        //tableSimulation->drawActualPinHeightImageFromTable(2, 240*0.6*3+20, RELIEF_PHYSICAL_SIZE_X * 6, RELIEF_PHYSICAL_SIZE_Y * 6); //costly
        drawBitmapString("Actual Pin Height Image from Table", 10, 240*0.6*3+30);
        
        // each shape object can have a seperate gui
        //mCurrentShapeObject->drawGuiScreen(1020*0.6+20, 0, KINECT_X * 0.5, KINECT_Y * 0.5);
    }
    
    else if(currentScreen == "tcp")
    {
        mTCPShapeObject->update();
        mCurrentShapeObject->renderGraphics(0, 0, RELIEF_PHYSICAL_SIZE_X, RELIEF_PHYSICAL_SIZE_Y);
    }
    
    // draw simulation in all views if we want
    // be careful as this slows performance
    if(drawTableSimulation) {
        tableSimulation->drawTableCamView(400, 320, 680, 480, 4);
        tableSimulation->drawInteractionArea(400, 320, 680, 480);
    }

    drawGeneralMessage();

    if(isPaused)
    {
        ofSetColor(255, 0, 0);
        ofRect(ofGetWidth() - 200,  ofGetHeight() -300, 30, 100);
        ofRect(ofGetWidth() - 140,  ofGetHeight() -300, 30, 100);
   }
}

//--------------------------------------------------------------
void ReliefApplication::drawGeneralMessage() {
	ofPushStyle();
    ofSetColor(255);
	string msg = "Press '=' to toggle between timeline, kinect, and shapeobject screens.";
    msg += "\nPress `t` to show / hide the table simulation and increase performance.";
    msg += "\nPress `g` to show / hide the gui.";
	msg += "\n\nfps: " + ofToString(ofGetFrameRate(), 2);
	ofDrawBitmapStringHighlight(msg, 10, ofGetWindowHeight()-100);
    ofPopStyle();
}

//--------------------------------------------------------------
void ReliefApplication::keyPressed(int key){
	switch(key) {
		case 'r':
        case 'R':
			if (movieExporter.isRecording()) {
                movieExporter.stop();
                mKinectTracker.isCurrentlyRecording = false;
                kinectVideoPlayer.listAllVideos();
            }
			else {
                mKinectTracker.isCurrentlyRecording = true;
                movieExporter.record(ofGetTimestampString()+"-kinect-", "kinect_videos");
            }
			break;
        case 'p':
        case 'P':
            isPaused = !isPaused;
            if(isPaused) pauseApp();
            else resumeApp();
            break;
        case 't':
        case 'T':
            drawTableSimulation = !drawTableSimulation;
            break;
        case '=':
            incrementAndSetCurrentScreen();
            break;
        case 's':
        case 'S':
            mMachineAnimationShapeObject->stopNowPlaying(); // drops the video
            break;
        case 'c':
        case 'C':
            //mTCPShapeObject->clearFrames();
            //mTCPShapeObject->keyPressed('c');
            break;
        case 'x':
        case 'X':
            //mTCPShapeObject->togglePlay();
            //mTCPShapeObject->keyPressed('x');
            break;
        case 'z':
        case 'Z':
            //mTCPShapeObject->togglePause();
            //mTCPShapeObject->keyPressed('z');
            break;
    }
    
    //recorder.keyPressed(key);
}

//--------------------------------------------------------------
void ReliefApplication::mouseMoved(int x, int y){
    
}

//--------------------------------------------------------------
void ReliefApplication::mouseDragged(int x, int y, int button){
    if(currentScreen == "kinect") mImageWarper->mouseDragged(x, y, button);
}

//--------------------------------------------------------------
void ReliefApplication::mousePressed(int x, int y, int button){
    if(currentScreen == "kinect") mImageWarper->mousePressed(x, y, button);
}

//------------------------------------------------------------
void ReliefApplication::exit(){
    
    mImageShapeObject->drawImageByFileName("exit.jpg");
    //mCurrentShapeObjects.clear();
    mCurrentShapeObject = mImageShapeObject;
    update();
    draw();
    
    //mIOManager->disconnectFromTable();
    mIOManager->disconnectFromTableWithoutPinReset();
    //tableGui->saveSettings("table_settings.xml");
    easyGui->saveSettings("kinect_settings.xml");
    mImageWarper->saveSettings("settings_warp_points.xml");

    delete easyGui;
    delete imagesDropdown;
    delete videosDropdown;
    delete kinectRecordingsDropdown;
}

//------------------------------------------------------------
void ReliefApplication::tableGuiEvent(ofxUIEventArgs &e) {
    
//    if(e.getName() == "P TERM") {
//        cout << "p terms changed" << gain_P << endl;
//    }
    
    // set all values is easier then doing check for event name
    mIOManager->set_gain_p(gain_P);
    mIOManager->set_gain_i(gain_I);
    mIOManager->set_max_i(max_I);
    mIOManager->set_deadzone(deadZone);
    mIOManager->set_max_speed(maxSpeed);
    
}

//------------------------------------------------------------

void ReliefApplication::pauseApp() {
    kinectVideoPlayer.pause();
    cout << "pause" << endl;
}

//------------------------------------------------------------

void ReliefApplication::resumeApp() {
    kinectVideoPlayer.resume();
    cout << "resume" << endl;
}


//------------------------------------------------------------
void ReliefApplication::guiEvent(ofxUIEventArgs &e)
{
    //cout << "Event fired: " << e.getName() << endl;
    if(e.getName() == "video list")
    {
        ofxUIDropDownList *ddlist = (ofxUIDropDownList *) e.widget;
        vector<ofxUIWidget *> &selected = ddlist->getSelected();
        for(int i = 0; i < selected.size(); i++)
        {
            cout << "SELECTED VIDEO: " << selected[i]->getName() << endl;
            //mCurrentShapeObject = mMachineAnimationShapeObject;
            mMachineAnimationShapeObject->playMovieByFilename(selected[i]->getName());
            mMachineAnimationShapeObject->reset();
            mMachineAnimationShapeObject->setLooping(true);
            mMachineAnimationShapeObject->resume();
            cout<<"play"<<endl;
        }
    }
    
    else if(e.getName() == "images list")
    {
        ofxUIDropDownList *ddlist = (ofxUIDropDownList *) e.widget;
        vector<ofxUIWidget *> &selected = ddlist->getSelected();
        for(int i = 0; i < selected.size(); i++)
        {
           cout << "SELECTED IMAGE: " << selected[i]->getName() << endl;
           mImageShapeObject->drawImageByFileName(selected[i]->getName());
        }
    }
    
    else if(e.getName() == "RECORDING SOURCE")
    {
        ofxUIDropDownList *ddlist = (ofxUIDropDownList *) e.widget;
        vector<ofxUIWidget *> &selected = ddlist->getSelected();
        for(int i = 0; i < selected.size(); i++)
        {
            cout << "SELECTED: " << selected[i]->getName() << endl;
            kinectVideoPlayer.playByFilename(selected[i]->getName());
        }
    }
    
    else if(e.getName() == "Toggle Use Table")
    {
        ofxUIToggle *toggle = e.getToggle();
        if(toggle->getValue()  == true)
        {
            connectTable();
        }
        else
        {
            cout << "NOT Using table, closing connection" << endl;
            // close connection
            mIOManager->disconnectFromTable();
        }
    }

    
    //MODES
    if(e.getName() == "MODES")
    {
        ofxUIRadio *radio = (ofxUIRadio *) e.widget;
        
        if(radio->getActiveName() == "none")
        {
            cout << "MODE - none" << endl;
            //mCurrentShapeObjects.clear();
            mCurrentShapeObject = mCalmShapeObject;
        }
        
        else if(radio->getActiveName() == "videos")
        {
            cout<<"video"<<endl;
            mCurrentShapeObject = mMachineAnimationShapeObject;
        }
        
        else if(radio->getActiveName() == "images")
        {
            cout << "images" << endl;
            mCurrentShapeObject = mImageShapeObject;
        }
        
        else if(radio->getActiveName() == "TCP")
        {
            cout << "TCP" << endl;
            mCurrentShapeObject = mTCPShapeObject;
        }
    }
    
    //KINECT Modes
    if(e.getName() == "KINECT")
    {
        ofxUIRadio *radio = (ofxUIRadio *) e.widget;
        
        if(radio->getActiveName() == "Live Kinect")
        {
            cout << "SOURCE - live kinect" << endl;
            useLiveKinect = true; //  @todo fix this when refactoring how we get kinect
            useRecording = false;
        }
        else if(radio->getActiveName() == "Recorded Kinect")
        {
            cout << "SOURCE - recorded kinect" << endl;
            useLiveKinect = true;
            useRecording = true;
        }
    }
};

//------------------------------------------------------------
//
// Increment current screen by 1, and set currentScreen to string
// name representing the screen. To reorder screens,
// change array order in header file.
//
//------------------------------------------------------------
void ReliefApplication::incrementAndSetCurrentScreen()
{
    screenIndex ++;
    if(screenIndex > maxScreens-1) screenIndex = 0;
    currentScreen = screens[screenIndex];
}

//------------------------------------------------------------
void ReliefApplication::drawBitmapString(string _message, int _x, int _y)
{
    ofPushStyle();
    ofSetColor(255, 0, 0);
    ofDrawBitmapString(_message, _x, _y);
    ofPopStyle();
}
//------------------------------------------------------------

void ReliefApplication::connectTable()
{
    cout << "Using table, trying to connect" << endl;
    
    // start table connection
    mIOManager->connectToTable();
    
    mIOManager->set_gain_p(gain_P);
    mIOManager->set_gain_i(gain_I);
    mIOManager->set_max_i(max_I);
    mIOManager->set_deadzone(deadZone);
    mIOManager->set_max_speed(maxSpeed);
}

//------------------------------------------------------------

