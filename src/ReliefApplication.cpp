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
    mKinectTracker.setup();
    
    // module for saving recordings from kinect
    // @todo matt refactor to class
    movieExporter.setup(KINECT_X * 2, KINECT_Y);
    movieExporter.setPixelSource(mKinectTracker.getRecordingPixels(), KINECT_X * 2, KINECT_Y);
    
    kinectVideoPlayer.setup("kinect_videos");
  
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
    setupKinectGui();
    setupTableGui();
    setupCoolGui();
    
    // load gui settings
    kinectGui->loadSettings("kinect_settings.xml");
    coolGui_1->loadSettings("cool_1_settings.xml");
    coolGui_2->loadSettings("cool_2_settings.xml");
    coolGui_3->loadSettings("cool_3_settings.xml");
    coolGui_4->loadSettings("cool_4_settings.xml");
    
    // add al guis to tab bar
    guiTabBar = new ofxUITabBar();
    guiTabBar->addCanvas(kinectGui);
    guiTabBar->addCanvas(tableGui);
    guiTabBar->addCanvas(coolGui_3);
    guiTabBar->addCanvas(coolGui_1);
    guiTabBar->addCanvas(coolGui_2);
    guiTabBar->addCanvas(coolGui_4);
    guiTabBar->addCanvas(coolGui_5);
    guiTabBar->autoSizeToFitWidgets();
    //(1020*0.6+20, 0, KINECT_X * 0.5, KINECT_Y * 0.5
    guiTabBar->setPosition(1020 * 0.6 + 20, KINECT_Y * 0.5 + 20);
    guiTabBar->setVisible(false);
    
    // setup timeline
    setupTimeline();
    
    // set default shape object
    // @todo matt refactor this because it requires setting gui
    // options, variables, and more and needs to be centralized
    mCurrentShapeObjects.push_back(mCalmShapeObject);
    
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
    
    // wavy shape object
    mWavyShapeObject = new WavyShapeObject(KINECT_X, KINECT_Y, 11);
    mWavyShapeObject->setImageWarper(mImageWarper);
    mWavyShapeObject->setKinectTracker(&mKinectTracker);
    
    // flocks move towards and away from users tracked by kinect
    mFlockShapeObject = new FlockShapeObject(mWavyShapeObject, 9);
    mFlockShapeObject->setKinectTracker(&mKinectTracker);
    mFlockShapeObject->setImageWarper(mImageWarper);
    
    // machine animations
    mMachineAnimationShapeObject = new MachineAnimationShapeObject();
    
    // fade from 0 to 255 over time and repeat
    mFadeLoopShapeObject = new FadeLoopShapeObject();

    // still, no motion. Default shape object
    // can also be used to "stop" other shape objects
    mCalmShapeObject = new CalmShapeObject();
    
    mImageShapeObject = new ImageShapeObject();
    mImageShapeObject->setup("images");
    
    mEscherShapeObject = new EscherShapeObject();
    
    mHandShapeObject = new HandShapeObject();
    mHandShapeObject->setImageWarper(mImageWarper);
    mHandShapeObject->setKinectTracker(&mKinectTracker);
    
    // push all shape objects to a buffer
    allShapeObjects.push_back(mWavyShapeObject);
    allShapeObjects.push_back(mFlockShapeObject);
    allShapeObjects.push_back(mMachineAnimationShapeObject);
    allShapeObjects.push_back(mKinectShapeObject);
    allShapeObjects.push_back(mFadeLoopShapeObject);
    allShapeObjects.push_back(mCalmShapeObject);
    allShapeObjects.push_back(mImageShapeObject);
    allShapeObjects.push_back(mEscherShapeObject);
    allShapeObjects.push_back(mHandShapeObject);
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
    easyGui->setPosition(1090, 560);
    easyGui->setHeight(500);
    easyGui->setName("Easy Control");
    easyGui->addLabel("Easy Control");
    vector <string> modes;
    
    modes.push_back("none");
    modes.push_back("scenario standard");
    modes.push_back("scenario w/o fish");
    modes.push_back("scenario w/o machine");
    modes.push_back("fish loop");
    //modes.push_back("fish loop not shy"); //TODO
    modes.push_back("wave loop");
    modes.push_back("machine loop");
    modes.push_back("escher standard");
    modes.push_back("escher drop/pickup");
    //modes.push_back("kinect hand");
    
    ofxUIRadio *easyRadio = easyGui->addRadio("MODES", modes);
    easyRadio->activateToggle("none");
    ofAddListener(easyGui->newGUIEvent, this, &ReliefApplication::guiEvent);
    
}

//--------------------------------------------------------------
//
// Setup Kinect Gui
//
//--------------------------------------------------------------
void ReliefApplication::setupKinectGui() {
    kinectGui = new ofxUICanvas;
    kinectGui->setName("Kinect Setup");
    kinectGui->addLabel("Kinect Setup");
    vector <string> sources;
    sources.push_back("Live Kinect");
    sources.push_back("Recorded Kinect");
    ofxUIRadio *kinectRadio = kinectGui->addRadio("SOURCE", sources);
    kinectRadio->activateToggle("Recorded Kinect");
    
    kinectGui->addIntSlider("Near Threshold", 0, 255, &mKinectTracker.mNearThreshold);
    kinectGui->addIntSlider("Far Threshold", 0, 255, &mKinectTracker.mFarThreshold);
    kinectGui->addIntSlider("Contour Min Size", 0, 15000, &mKinectTracker.mContourMinimumSize);
    kinectGui->addToggle("Use Mask", &mKinectTracker.useMask);
    ofxUIDropDownList *ddl = kinectGui->addDropDownList("RECORDING SOURCE", kinectVideoPlayer.loadedVideoFilenames);
    ddl->setAllowMultiple(false);
    ddl->setAutoClose(true);
    ofAddListener(kinectGui->newGUIEvent, this, &ReliefApplication::guiEvent);
    kinectGui->autoSizeToFitWidgets();
}


//--------------------------------------------------------------
//
// Setup Table Gui
//
//--------------------------------------------------------------
void ReliefApplication::setupTableGui() {
    tableGui = new ofxUICanvas;
    tableGui->setName("Default table values");
    tableGui->addLabel("Default table values");
    //tableGui->addTextArea("DESC", "These values will be overridden if current shape object has custom table terms");
    tableGui->addSlider("P TERM", 0, 3, &gain_P);
    tableGui->addSlider("I TERM", 0, 1, &gain_I);
    tableGui->addIntSlider("Max I", 0, 200, &max_I);
    tableGui->addIntSlider("deadzone", 0, 20, &deadZone);
    tableGui->addIntSlider("Max Speed", 0, 220, &maxSpeed);
    ofAddListener(tableGui->newGUIEvent, this, &ReliefApplication::tableGuiEvent);
    tableGui->loadSettings("table_settings.xml");
}


//--------------------------------------------------------------
//
// Setup Cool Gui
//
//--------------------------------------------------------------
void ReliefApplication::setupCoolGui() {
    
    coolGui_1 = new ofxUICanvas;
    coolGui_2 = new ofxUICanvas;
    coolGui_3 = new ofxUICanvas;
    coolGui_4 = new ofxUICanvas;
    coolGui_5 = new ofxUICanvas;
    
    //machine animation dropdown
    coolGui_1->setName("Animation Config");
    coolGui_1->addLabel("Animation Config");
    ofxUIDropDownList *mddl = coolGui_1->addDropDownList("Animation", mMachineAnimationShapeObject->getLoadedVideoFilenames());
    mddl->setAllowMultiple(false);
    mddl->setAutoClose(true);
    
    //still images dropdown
    coolGui_2->setName("Images Config");
    coolGui_2->addLabel("Images Config");
    ofxUIDropDownList *iddl = coolGui_2->addDropDownList("Still Images", mImageShapeObject->getLoadedImagesFilenames());
    iddl->setAllowMultiple(false);
    iddl->setAutoClose(true);

    coolGui_3->setName("Basic Config");
    coolGui_3->addLabel("Basic Config");
    coolGui_3->addToggle("FULLSCREEN", false);
    coolGui_3->addToggle("PAUSE APP", &isPaused);
    coolGui_3->addSpacer();
    coolGui_3->addToggle("Use Table", &useTable);
    coolGui_3->addSpacer();
    coolGui_3->addToggle("MANUAL CONTROL", controlManual);
    coolGui_3->addSpacer();
    coolGui_3->addToggle("Loop Animations", &bAnimationLooping);
    coolGui_3->addSpacer();
    coolGui_3->addLabel("Shape Object Config");
    ofxUIRadio *radio = coolGui_3->addRadio("ACTIVE SHAPE OBJECT", allsShapeObjectsNames);
    radio->activateToggle("Calm");
    
    coolGui_4->setName("Wavy Config");
    coolGui_4->addLabel("Wavy Config");
    // temp here until we factor into settings per shape object
    coolGui_4->addIntSlider("Wave Interpolation", 0, 12, &mWavyShapeObject->interpolate);
    coolGui_4->addIntSlider("Wave Speed", 0, 10, &mWavyShapeObject->solveCount);
    coolGui_4->addIntSlider("Wave Timestep (ms)", 0, 1000, &mWavyShapeObject->fixedDeltaMS);
    coolGui_4->addSlider("Sea Level", 0, 255, &mWavyShapeObject->seaLevel);
    coolGui_4->addSlider("Wave Amplitude", 0, 12, &mWavyShapeObject->waveAmplitude);
    coolGui_4->addSlider("Idle Wave Amplitude", 0, 1000, &mWavyShapeObject->idleWaveAmplitude);
    coolGui_4->addSlider("Idle Wave Frequency", 0, 50, &mWavyShapeObject->idleWaveFrequency);
    
    
    coolGui_5->setName("Escher Config");
    coolGui_5->addLabel("Escher Config");
    allEscherModeNames.push_back("Escher Start");
    allEscherModeNames.push_back("Escher Run");
    allEscherModeNames.push_back("Escher Stop");
    ofxUIRadio *radioEscher = coolGui_5->addRadio("Escher Mode", allEscherModeNames);
    
    ofAddListener(coolGui_1->newGUIEvent, this, &ReliefApplication::guiEvent);
    ofAddListener(coolGui_2->newGUIEvent, this, &ReliefApplication::guiEvent);
    ofAddListener(coolGui_3->newGUIEvent, this, &ReliefApplication::guiEvent);
    ofAddListener(coolGui_4->newGUIEvent, this, &ReliefApplication::guiEvent);
    ofAddListener(coolGui_5->newGUIEvent, this, &ReliefApplication::guiEvent);
    
    coolGui_1->autoSizeToFitWidgets();
    coolGui_2->autoSizeToFitWidgets();
    coolGui_3->autoSizeToFitWidgets();
    coolGui_4->autoSizeToFitWidgets();
    coolGui_5->autoSizeToFitWidgets();
}


//--------------------------------------------------------------
//
// Setup timeline
//
//--------------------------------------------------------------
void ReliefApplication::setupTimeline() {
    ofxTimeline::removeCocoaMenusFromGlut("BasicExample"); // @todo rename this?
    
	timeline.setup();
    
    timeline.setFrameRate(30);
	timeline.setDurationInSeconds(300);
	timeline.setLoopType(OF_LOOP_NORMAL);
    
    timeline.setPageName("Fish");
    timeline.addCurves("Fish Blend", ofRange(0, 255));
    timeline.addCurves("Fish Water Level", ofRange(0, 1.0)); //TODO
    timeline.addSwitches("Fish Wavy");
    timeline.addSwitches("Fish Flock");
    timeline.addSwitches("Fish Calm");
    timeline.addFlags("Fish Flags"); // flags for all modes
    
    timeline.addPage("Wave");
    timeline.addCurves("Wave Blend", ofRange(0, 255));
    timeline.addCurves("Wave Water Level", ofRange(0, 1.0)); //TODO
    timeline.addSwitches("Wave Wavy");
    timeline.addSwitches("Wave Flock");
    timeline.addSwitches("Wave Calm");
    timeline.addFlags("Wave Flags"); // flags for all modes
    
	timeline.addPage("Machine");
    timeline.addCurves("Machine Blend", ofRange(0, 255));
    timeline.addCurves("Machine Water Level", ofRange(0, 1.0)); //TODO
    timeline.addCurves("Machine Water Current", ofRange(1, 2.25)); //TODO
    timeline.addSwitches("Machine Wavy");
    timeline.addSwitches("Machine Machine Animation");
    timeline.addSwitches("Machine Calm");
    timeline.addFlags("Machine Flags"); // flags for all modes
    timeline.addVideoTrack("Machine Animation", "machine_animation_videos/FINAL.mov");
	
    // start timeline on fish mode
    currentPage = "Fish";
    timeline.setCurrentPage(currentPage);
    timeline.play();

    mMachineAnimationShapeObject->setNowPlaying(*timeline.getVideoTrack("Machine Animation")->getPlayer());
    
    ofAddListener(timeline.events().bangFired, this, &ReliefApplication::bangFired);
    ofAddListener(timeline.events().pageChanged, this, &ReliefApplication::pageChanged);
}


// called when page changes
void ReliefApplication::pageChanged(ofxTLPageEventArgs& args){
    currentPage = args.currentPageName;
    if(currentPage == "Machine")
    {
        mMachineAnimationShapeObject->reset();
        mMachineAnimationShapeObject->resume();
    }
};

// changes page
void ReliefApplication::setPageAndPlayFromFirstFrame(string page_name) {
    currentPage = page_name;
    timeline.stop();
    timeline.setCurrentPage(currentPage);
    timeline.setCurrentFrame(0);
    timeline.play();
    cout << "TIMELINE CHANGING PAGE -> " << currentPage << endl;
};

// repeats current frame
void ReliefApplication::repeatCurrentPage() {
    timeline.stop();
    timeline.setCurrentFrame(0);
    timeline.play();
    cout << "TIMELINE REPEATING PAGE -> " << currentPage << endl;
};

void ReliefApplication::bangFired(ofxTLBangEventArgs& args){
    cout << "Bang fired from track " + args.flag << endl;
    if(args.flag == "Machine" && currentPage == "Wave" && isSkipMachine) {
        setPageAndPlayFromFirstFrame("Wave");
    }
    else if(args.flag == "Machine" && currentPage == "Wave" && !isSkipMachine) {
        setPageAndPlayFromFirstFrame("Machine");
    }
    else if(args.flag == "Fish" && currentPage == "Machine" && isSkipFish) {
        setPageAndPlayFromFirstFrame("Wave");
        timeline.stop();
        timeline.setCurrentTimeSeconds(53);
        timeline.play();
    } else if(args.flag == "Fish" && currentPage == "Machine" && !isSkipFish) {
        setPageAndPlayFromFirstFrame("Fish");
    } else if(args.flag == "repeat" && currentPage == "Fish") {
        repeatCurrentPage();
    }
}

//--------------------------------------------------------------
//
// Calculate when to go to next page
//
//--------------------------------------------------------------
void ReliefApplication::setPageBasedOnBlobCount() {
    
    int numBlobs = mKinectTracker.getContourFinder()->nBlobs;
    
    // if no people,
    // if current page is transition
    // go to page rest
    
    // if no people
    // if current page is awake
    // stay on awake
    
    // if people
    // if current page is rest
    // go to transition
    
    // if people
    // if current page is transition
    // stay on transition
    
    // no people
    if(numBlobs == 0) {
        if(currentPage == "Wave" && !isSkipFish) {
            setPageAndPlayFromFirstFrame("Fish");
        }
    // have people
    } else {
        if(currentPage == "Fish" && !isSkipFish) {
            setPageAndPlayFromFirstFrame("Wave");
        }
    }

}

//--------------------------------------------------------------
//
// Gets curve value for current page by prefixing with current page name
//
//--------------------------------------------------------------
float ReliefApplication::getValueForCurrentPage(string curveName) {
    return timeline.getValue(currentPage + " " + curveName);
}

//--------------------------------------------------------------
//
// Gets curve value for current page by prefixing with current page name
//
//--------------------------------------------------------------
int ReliefApplication::getIntValueForCurrentPage(string curveName) {
    return timeline.getValue(currentPage + " " + curveName);
}


//--------------------------------------------------------------
void ReliefApplication::update(){
    
    // prevent playing in manual mode
    if(!controlTimeline) timeline.stop();
    
    // if paused dont update
    if(isPaused) return;
    
    if(controlTimeline) {
        
        mWavyShapeObject->setWaveScalar(getValueForCurrentPage("Water Level"));
//<<<<<<< HEAD
//=======
        
//        if (getValueForCurrentPage("Water Current") != 0)
//            mWavyShapeObject->setIdleWaveScalar(getValueForCurrentPage("Water Current"));
//
//        //cout << "water level is: " << getValueForCurrentPage("Water Level") << endl;
//
//        if(currentPage == "Rest") {
//            
//            if(isSwitchOnForCurrentPage("Calm")) mCurrentShapeObjects.push_back(mCalmShapeObject);
//            else mCurrentShapeObjects.push_back(mFlockShapeObject);
//            
//            mFlockShapeObject->setFishStay(true);
//            
//        } else if (currentPage =="Transition") {
//            
//            mCurrentShapeObjects.push_back(mFlockShapeObject);
//            
//        } else if(currentPage == "Awake") {
//            mFlockShapeObject->setFishStay(false);
//            if(isSwitchOnForCurrentPage("Calm")) {
//                mCurrentShapeObjects.push_back(mCalmShapeObject);
//            } else {
//                if(isSwitchOnForCurrentPage("Wavy"))
//                    mCurrentShapeObjects.push_back(mFlockShapeObject);
//                mCurrentShapeObjects.push_back(mMachineAnimationShapeObject);
//            }
//            
//        }

     
        // iterate though all shape objects and check if
        // trigger is on for this shape object
        // on current page
        setCurrentShapeObjectsFromActiveTimelineSwitches();
        
        // @todo it would be great to remove this and just activate wavy or
        // flocking depending on switch
        if(currentPage == "Fish")       mFlockShapeObject->setFishStay(true);
        else if(currentPage == "Machine") mFlockShapeObject->setFishStay(false);
        
        // go to next page if we have blobs
        if(!isSkipFish)setPageBasedOnBlobCount();
    
        
    }
    
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
    
    // update all shape objects and values
    for(int i = 0; i < mCurrentShapeObjects.size(); i++) {
        mCurrentShapeObjects[i]->update(dt);
        // @note the last shape object values will win out here.
        mCurrentShapeObjects[i]->setTableValuesForShape(mIOManager);
    }

    // buffer drawing
    // @todo matt this needs refactoring, along with all shape objects
    // what's happening here is that renderShape() method works more like update
    // it performs the calculations and draws into an allPiexls buffer
    // which is then used for the small image
    // we need to clean this up in the shape objects
    pinHeightMapImage.begin();
    ofBackground(0);
    ofSetColor(255);
    for(int i = 0; i < mCurrentShapeObjects.size(); i++)
        mCurrentShapeObjects[i]->renderShape();
    pinHeightMapImage.end();
    
    // for all active shape objects
    // we blend them together
    // by taking the highest pixel value from each position
    blendCurrentShapeObjectsByHighestValue();
    
    // draw the big heightmap image into a small heightmap image and send it off to the table
    pinHeightMapImageSmall.begin();
    ofBackground(0);
    ofSetColor(255);
    
    ofImage heightImage;
    heightImage.allocate(RELIEF_PHYSICAL_SIZE_X, RELIEF_PHYSICAL_SIZE_Y, OF_IMAGE_GRAYSCALE);
    ofPixels heightPixels;
    heightPixels.allocate(RELIEF_PHYSICAL_SIZE_X, RELIEF_PHYSICAL_SIZE_Y, OF_IMAGE_GRAYSCALE);
    heightPixels.setFromExternalPixels(targetPixels, RELIEF_PHYSICAL_SIZE_X, RELIEF_PHYSICAL_SIZE_Y, 1);
    heightImage.setFromPixels(heightPixels);
    heightImage.draw(0,0);
    
    // by drawing a transparent rectangle over the entire buffer
    if(timeline.getIsPlaying() == true) {
        ofPushStyle();
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        float fadeAmnt = 255 - getIntValueForCurrentPage("Blend");
        ofSetColor(0,0,0, fadeAmnt);
        ofRect(0,0,RELIEF_PHYSICAL_SIZE_X, RELIEF_PHYSICAL_SIZE_Y);
        glDisable(GL_BLEND);
        ofPopStyle();
    }
    
    //crude but quick solution
    if (mCurrentShapeObjects[0]->get_shape_name() == "Image") {
        mCurrentShapeObjects[0]->renderGraphics(0, 0, RELIEF_PHYSICAL_SIZE_X, RELIEF_PHYSICAL_SIZE_Y);
    }
    
    pinHeightMapImageSmall.end();
    
    // updates the table with the small rendered pinHeightMapImage
    // will use the last height map buffer image
    mIOManager->update(pinHeightMapImageSmall);
    
    // update the table simulation, which is the rendered table graphic
    tableSimulation->update();
    
//    for(int i = 0; i < mCurrentShapeObjects.size(); i++) {
//        cout << mCurrentShapeObjects[i]->get_shape_name() << endl;
//    }
//    cout << "----" << endl;
}

//--------------------------------------------------------------
// Blends current shape objects together by
// taking highest pixel value from each position in pixel array
//--------------------------------------------------------------
void ReliefApplication::blendCurrentShapeObjectsByHighestValue() {
    
    targetPixels = new unsigned char[RELIEF_SIZE]; // create target matrix
    
    for(int i = 0; i < RELIEF_SIZE; i++) {
        targetPixels[i] = 0; // set all pixels to black
    }
    
    for(int i = 0; i < mCurrentShapeObjects.size(); i++) { // for each shape object
        compPixels = new unsigned char[RELIEF_SIZE];
        compPixels = mCurrentShapeObjects[i]->getPixels(); // get shape object pixels
        
        for(int i = 0; i < RELIEF_SIZE; i++) { // for each pixel
            if(compPixels[i] > targetPixels[i]) { // if pixel is greater then target pixel value
                targetPixels[i] = compPixels[i]; // replace target value with this value
            }
        }
    }
}

//--------------------------------------------------------------
//
// Checks if a switch is on across all page names
// by prefacing switch name with prefix for page
//
// assumes that on each page we preface switches with Rest, Transition, Awake
//
//--------------------------------------------------------------

bool ReliefApplication::isSwitchOnForCurrentPage(string switchName) {
    string switchNameForPage = currentPage + " " + switchName;
    //string switchNameForPage = switchName;
    if(!timeline.hasTrack(switchNameForPage)) return false;
    return timeline.isSwitchOn(switchNameForPage);
}

//--------------------------------------------------------------
// If switch is active add each shape object to current vector
//--------------------------------------------------------------
void ReliefApplication::setCurrentShapeObjectsFromActiveTimelineSwitches() {
    
    mCurrentShapeObjects.clear();
    
    for (vector<ShapeObject*>::iterator it = allShapeObjects.begin() ; it != allShapeObjects.end(); ++it) {
        string name = (*it)->get_shape_name();
        string name_for_current_page = currentPage + " " + name;
        //cout << "checking is on? " << name << endl;
        
        // if switch is on, add to current shape objects vector
        if(isSwitchOnForCurrentPage(name)) {
            //cout << currentPage << " : " << name << endl;
            mCurrentShapeObjects.push_back(*it);
        }
        
        //mCurrentShapeObjects.push_back(mFlockShapeObject);
        //cout << "vector size is " << mCurrentShapeObjects.size() << "\n========" << endl;
    }
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
    
    // default the buffered image
    // will be different depending on kinect or wave enabled
    if(currentScreen == "timeline") {
        
        mKinectTracker.drawThresholdImage(0, 400, KINECT_X/2, KINECT_Y/2);
        
        int blobCount = mKinectTracker.getContourFinder()->nBlobs;
        string message = "Current blob count: " + ofToString(blobCount);
        ofDrawBitmapString(message, 0, 400+(KINECT_Y/2)+10);
        
        timeline.draw();
        
        if(guiTabBar->isVisible()) timeline.hide();
        else timeline.show();
        
        mCurrentShapeObjects[0]->drawGuiScreen(KINECT_X/2+10, 400, KINECT_X * 0.2, KINECT_Y * 0.2);
        
        // draw a small preview of the table surface
        ofImage heightImage;
        heightImage.allocate(RELIEF_PHYSICAL_SIZE_X, RELIEF_PHYSICAL_SIZE_Y, OF_IMAGE_GRAYSCALE);
        ofPixels heightPixels;
        heightPixels.allocate(RELIEF_PHYSICAL_SIZE_X, RELIEF_PHYSICAL_SIZE_Y, OF_IMAGE_GRAYSCALE);
        heightPixels.setFromExternalPixels(targetPixels, RELIEF_PHYSICAL_SIZE_X, RELIEF_PHYSICAL_SIZE_Y, 1);
        heightImage.setFromPixels(heightPixels);
        //heightImage.draw(0,0);
        //heightImage.draw(ofGetWidth() - RELIEF_PHYSICAL_SIZE_X - 10, ofGetHeight() - RELIEF_PHYSICAL_SIZE_Y - 10);
        
    } if(currentScreen == "kinect") {
        
        //cvColorImage.draw(xVidPos,yVidPos);
        mKinectTracker.drawColorImage(0,0, KINECT_X, KINECT_Y);
        if(mKinectTracker.useMask) mKinectTracker.mask.draw(xVidPos,yVidPos);
        
        cvWarpedImage.draw(0,KINECT_Y, cvWarpedImage.width/2, cvWarpedImage.height/2);
        mKinectTracker.drawDepthThreshedDiff(KINECT_X, 0, KINECT_X/2, KINECT_Y/2);
        
        mKinectTracker.drawThresholdImage(KINECT_X, KINECT_Y/2, KINECT_X/2, KINECT_Y/2);
        
        mImageWarper->drawWarpPoints();
    
        timeline.hide();
        
    } else if(currentScreen == "shapeobject") {

        pinDisplayImage.draw(2, 2, 1020*0.6, 240*0.6);
        pinHeightMapImage.draw(2, 240*0.6+10, 1020*0.6, 240*0.6);
        mKinectTracker.drawThresholdImage(2, 240*0.6*2+20, KINECT_X * 0.5, KINECT_Y * 0.5);
        pinHeightMapImage.draw(KINECT_Y*0.5 + 100, 240*0.6*2+20, RELIEF_PHYSICAL_SIZE_X, RELIEF_PHYSICAL_SIZE_Y);
        
        tableSimulation->drawActualPinHeightImageFromTable(2, 240*0.6*3+20, RELIEF_PHYSICAL_SIZE_X * 6, RELIEF_PHYSICAL_SIZE_Y * 6);
        
        // each shape object can have a seperate gui
        mCurrentShapeObjects[0]->drawGuiScreen(1020*0.6+20, 0, KINECT_X * 0.5, KINECT_Y * 0.5);
        
        timeline.hide();
        
    }
    
    // draw simulation in all views if we want
    // be careful as this slows performance
    if(drawTableSimulation && !useTable) {
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
        case 'g':
        case 'G':
            if(guiTabBar->isVisible()) {
                guiTabBar->setVisible(false);
                coolGui_1->setVisible(false);
                coolGui_2->setVisible(false);
                coolGui_3->setVisible(false);
                coolGui_4->setVisible(false);
                coolGui_5->setVisible(false);
                tableGui->setVisible(false);
                kinectGui->setVisible(false);
            }
            else
                guiTabBar->setVisible(true);
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
        case 'o':
            timeline.stop();
            timeline.setCurrentFrame(0);
            timeline.play();
            cout << 0 << endl;
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
    
    controlTimeline = false;
    controlManual = true;
    mImageShapeObject->drawImageByFileName("exit.jpg");
    mCurrentShapeObjects.clear();
    mCurrentShapeObjects.push_back(mImageShapeObject);
    update();
    draw();
    
    //mIOManager->disconnectFromTable();
    mIOManager->disconnectFromTableWithoutPinReset();
    coolGui_1->saveSettings("cool_1_settings.xml");
    coolGui_2->saveSettings("cool_2_settings.xml");
    coolGui_3->saveSettings("cool_3_settings.xml");
    coolGui_4->saveSettings("cool_4_settings.xml");
    tableGui->saveSettings("table_settings.xml");
    kinectGui->saveSettings("kinect_settings.xml");
    mImageWarper->saveSettings("settings_warp_points.xml");
    delete coolGui_1;
    delete coolGui_2;
    delete coolGui_3;
    delete coolGui_4;
    delete guiTabBar;
    delete tableGui;
    delete kinectGui;
    timeline.reset();
}


//mIOManager->set_gain_p(value);
//mIOManager->set_gain_i(value);
//mIOManager->set_max_i(value);
//mIOManager->set_deadzone(value);
//mIOManager->set_max_speed(value);

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

void ReliefApplication::pauseApp() {
    if(!controlTimeline) mMachineAnimationShapeObject->pause();
    else if(controlTimeline && timeline.getIsPlaying()) {
        didPauseTimeline = true;
        timeline.stop();
    }
    kinectVideoPlayer.pause();
    cout << "pause" << endl;
}

void ReliefApplication::resumeApp() {
    if(!controlTimeline) mMachineAnimationShapeObject->resume();
    else if(controlTimeline && !timeline.getIsPlaying() && didPauseTimeline) {
        didPauseTimeline = false;
        timeline.play();
    }
    kinectVideoPlayer.resume();
    cout << "resume" << endl;

}


//------------------------------------------------------------
void ReliefApplication::guiEvent(ofxUIEventArgs &e) {
    
    cout << "Event fired: " << e.getName() << endl;
    
    if(e.getName() == "PAUSE APP") {
        if(isPaused) pauseApp();
        else resumeApp();
    } else if(e.getName() == "Animation") {
        
        ofxUIDropDownList *ddlist = (ofxUIDropDownList *) e.widget;
        vector<ofxUIWidget *> &selected = ddlist->getSelected();
        for(int i = 0; i < selected.size(); i++)
        {
            cout << "SELECTED VIDEO: " << selected[i]->getName() << endl;
            mMachineAnimationShapeObject->playMovieByFilename(selected[i]->getName());
        }
    } else if(e.getName() == "Still Images"){
        
        ofxUIDropDownList *ddlist = (ofxUIDropDownList *) e.widget;
        vector<ofxUIWidget *> &selected = ddlist->getSelected();
        for(int i = 0; i < selected.size(); i++)
        {
           cout << "SELECTED IMAGE: " << selected[i]->getName() << endl;
           mImageShapeObject->drawImageByFileName(selected[i]->getName());
        }
        
    } else if(e.getName() == "RECORDING SOURCE") {
      
        ofxUIDropDownList *ddlist = (ofxUIDropDownList *) e.widget;
        vector<ofxUIWidget *> &selected = ddlist->getSelected();
        for(int i = 0; i < selected.size(); i++)
        {
            cout << "SELECTED: " << selected[i]->getName() << endl;
            kinectVideoPlayer.playByFilename(selected[i]->getName());
        }
        
    } if(e.getName() == "SOURCE") {
        
        ofxUIRadio *radio = (ofxUIRadio *) e.widget;
        if(radio->getActiveName() == "Live Kinect") {
            cout << "SOURCE - live kinect" << endl;
            useLiveKinect = true; //  @todo fix this when refactoring how we get kinect
            useRecording = false;
        } else if(radio->getActiveName() == "Recorded Kinect") {
            cout << "SOURCE - recorded kinect" << endl;
            useLiveKinect = true;
            useRecording = true;
        }
        
    } else if(e.getName() == "Use Table") {
        
        ofxUIToggle *toggle = e.getToggle();
        if(toggle->getValue()  == true) {
            
            cout << "Using table, trying to connect" << endl;
            
            // start table connection
            mIOManager->connectToTable();
            
            mIOManager->set_gain_p(gain_P);
            mIOManager->set_gain_i(gain_I);
            mIOManager->set_max_i(max_I);
            mIOManager->set_deadzone(deadZone);
            mIOManager->set_max_speed(maxSpeed);
            
        } else if(e.getName() == "Loop Animations"){
            ofxUIToggle *toggle = e.getToggle();
            if(toggle->getValue() == true) bAnimationLooping = true;
            else bAnimationLooping = false;
            mMachineAnimationShapeObject->setLooping(bAnimationLooping);
        } else {
            
            cout << "NOT Using table, closing connection" << endl;
            
            // close connection
            mIOManager->disconnectFromTable();
        }
        
    } else if(e.getName() == "BACKGROUND") {
        
        ofxUISlider *slider = e.getSlider();
        ofBackground(slider->getScaledValue());
     
    } else if (e.getName() == "MANUAL CONTROL") {
        
        ofxUIToggle *toggle = e.getToggle();
        if(toggle->getValue() == true) {
        
            controlTimeline = false;
            controlManual = true;
            
            // make sure waveScalar is correct
            mWavyShapeObject->waveScalar = 1;
            
            // trigger the active shape object button to ensure shape object is set initially
            ofxUIWidget *widget = coolGui_3->getWidget("ACTIVE SHAPE OBJECT");
            widget->triggerSelf();
            
        } else {
            controlTimeline = true;
            controlManual = false;
            
            
            mMachineAnimationShapeObject->playMovieByFilename("FINAL.mov");
            mMachineAnimationShapeObject->pause();
            
            // ensure machine animation is loaded again
            // in case user was overriding with manual control
            //mMachineAnimationShapeObject->setNowPlaying(*timeline.getVideoTrack("Machine Animation")->getPlayer());
            
        }
    
    } else if (e.getName() == "FULLSCREEN") {
        
        ofxUIToggle *toggle = e.getToggle();
        ofSetFullscreen(toggle->getValue());
        
    } else if(e.getName() == "ACTIVE SHAPE OBJECT") {
        
        ofxUIRadio *radio = (ofxUIRadio *) e.widget;
        cout << radio->getName() << " value: " << radio->getValue() << " active name: " << radio->getActiveName() << endl;
        
        mCurrentShapeObjects.clear();
        mCurrentShapeObjects.push_back(allShapeObjects.at(radio->getValue()));
        
    } else if(e.getName() == "Escher Mode") {
        ofxUIRadio *radioEscher = (ofxUIRadio *) e.widget;
        cout << radioEscher->getName() << " value: " << radioEscher->getValue() << " active name: " << radioEscher->getActiveName() << endl;
        if (radioEscher->getActiveName() == "Escher Start")
            mEscherShapeObject->currentMode = 1;
        else if (radioEscher->getActiveName() == "Escher Run")
            mEscherShapeObject->currentMode = 2;
        else if (radioEscher->getActiveName() == "Escher Stop")
            mEscherShapeObject->currentMode = 3;
    }
    
    //Easy Gui
    
    if(e.getName() == "MODES")
    {
        isSkipFish = false;
        isSkipMachine = false;
        isEasyModeFishNotShyToggled = false;
        ofxUIRadio *radio = (ofxUIRadio *) e.widget;
        
        if(radio->getActiveName() == "none")
        {
            cout << "MODE - none" << endl;
            controlTimeline = false;
            controlManual = true;
            timeline.stop();
            
            mCurrentShapeObjects.clear();
            mCurrentShapeObjects.push_back(mCalmShapeObject);
        }
        
        else if(radio->getActiveName() == "scenario standard")
        {
            cout << "MODE - scenario standard" << endl;
            mCurrentShapeObjects.clear();
            controlTimeline = true;
            controlManual = false;
            
            mMachineAnimationShapeObject->playMovieByPath("FINAL.mov");
            mMachineAnimationShapeObject->pause();
            timeline.stop();
            timeline.setCurrentPage(0);
            timeline.setCurrentFrame(0);
            timeline.play();
        }
        
        else if(radio->getActiveName() == "scenario w/o fish")
        {
            cout << "MODE - scenario w/o fish" << endl;
            mCurrentShapeObjects.clear();
            controlTimeline = true;
            controlManual = false;
            
            mMachineAnimationShapeObject->playMovieByFilename("FINAL.mov");
            mMachineAnimationShapeObject->pause();
            timeline.stop();
            timeline.setCurrentPage(1);
            timeline.setCurrentFrame(0);
            timeline.play();
            isSkipFish = true;
        }
        
        else if(radio->getActiveName() == "scenario w/o machine")
        {
            cout << "MODE - scenario w/o machine" << endl;
            mCurrentShapeObjects.clear();
            controlTimeline = true;
            controlManual = false;
            
            //mMachineAnimationShapeObject->playMovieByFilename("FINAL.mov");
            //mMachineAnimationShapeObject->pause();
            timeline.stop();
            timeline.setCurrentPage(1);
            timeline.setInPointAtFrame(5); // this avoids an empty frame when the wave timeline loops
            isSkipMachine = true;
        }

        
        else if(radio->getActiveName() == "fish loop")
        {
            cout << "MODE - fish loop" << endl;
            controlTimeline = false;
            controlManual = true;
            mCurrentShapeObjects.clear();
            mFlockShapeObject->reset(0);
            mCurrentShapeObjects.push_back(mFlockShapeObject);
        }
        
        else if(radio->getActiveName() == "fish loop not shy")
        {
            cout << "MODE - fish loop not shy" << endl;
            controlTimeline = false;
            controlManual = true;
            mCurrentShapeObjects.clear();
            mCurrentShapeObjects.push_back(mFlockShapeObject);
            isEasyModeFishNotShyToggled = true;
           //TODO
        }

        
        else if(radio->getActiveName() == "wave loop")
        {
            cout << "MODE - wave loop" << endl;
            controlTimeline = false;
            controlManual = true;
            mCurrentShapeObjects.clear();
            mWavyShapeObject->setWaveScalar(1.0f);
            mCurrentShapeObjects.push_back(mWavyShapeObject);
        }
        
        else if(radio->getActiveName() == "machine loop")
        {
            cout << "MODE - machine loop" << endl;
            isSkipFish =true;
            mCurrentShapeObjects.clear();
            controlTimeline = true;
            controlManual = false;
            
            mMachineAnimationShapeObject->playMovieByFilename("FINAL.mov");
            mMachineAnimationShapeObject->pause();
            timeline.stop();
            timeline.setCurrentPage("Wave");
            timeline.setCurrentTimeSeconds(53);
            timeline.play();
        }
        
        else if(radio->getActiveName() == "escher standard")
        {
            cout << "MODE - escher standard" << endl;
            controlTimeline = false;
            controlManual = true;
            
            mCurrentShapeObjects.clear();
            mMachineAnimationShapeObject->playMovieByFilename("escher_3_slow_5.mov");

            mCurrentShapeObjects.push_back(mMachineAnimationShapeObject);
            mMachineAnimationShapeObject->reset();
            mMachineAnimationShapeObject->setLooping(true);
            mMachineAnimationShapeObject->resume();
            
        }
        
        else if(radio->getActiveName() == "escher drop/pickup")
        {
            cout << "escher drop/pickup" << endl;
            controlTimeline = false;
            controlManual = true;
            
            mCurrentShapeObjects.clear();
            mCurrentShapeObjects.push_back(mMachineAnimationShapeObject);
            mMachineAnimationShapeObject->playMovieByFilename("escher_3_slow_3.mov");
            mMachineAnimationShapeObject->reset();
            mMachineAnimationShapeObject->setLooping(true);
            mMachineAnimationShapeObject->resume();
        }
        
        else if(radio->getActiveName() == "kinect hand")
        {
            cout << "kinect hand" << endl;
            controlTimeline = false;
            controlManual = true;
            
            mCurrentShapeObjects.clear();
            mCurrentShapeObjects.push_back(mHandShapeObject);
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
void ReliefApplication::incrementAndSetCurrentScreen() {
    screenIndex ++;
    if(screenIndex > maxScreens-1) screenIndex = 0;
    currentScreen = screens[screenIndex];
}
