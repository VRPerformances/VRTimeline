#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetFrameRate(30);
    ofSetVerticalSync(true);
    ofEnableSmoothing();
    
    glEnable(GL_DEPTH_TEST);
    
    logger = new Logger();
    logger->setup();
    
    ofxTimeline::removeCocoaMenusFromGlut("ShowControl");
    timeline = new ofxTimeline();
    timeline->setup();
    timeline->setFrameRate(24);
    timeline->setLoopType(OF_LOOP_NONE);
    timeline->toggleSnapToOtherKeyframes();
    timeline->enableUndo(true);
    timeline->setAutosave(false);
    ofVec2f v;
    v.x = 0.0f;
    v.y = 300.0f;
    timeline->setOffset(v);
    timeline->setHeight(500.0f);
    
    settings = new ofxXmlSettings();
    if(settings->loadFile("settings.xml")){
        loadConfig();
    }
    
    //Flags are little markers that you can attach text to
    //They are only useful when listening to bangFired() events
    //so that you know when one has passed
    timeline->setFrameBased(false);
    
    gui = new ofxUICanvas();
    saveButton = new ofxUIImageButton(0.0f, 0.0f, 32.0f, 32.0f, false, "GUI/save.png", "SaveButton");
    gui->addWidgetPosition(saveButton, OFX_UI_WIDGET_POSITION_DOWN);
    saveAsButton = new ofxUIImageButton(32, 32, false, "GUI/saveas.png", "SaveAsButton");
    saveAsButton->setLabelVisible(false);
    gui->addWidgetEastOf(saveAsButton, "SaveButton");
    loadButton = new ofxUIImageButton(32, 32, false, "GUI/load.png", "LoadButton");
    loadButton->setLabelVisible(false);
    gui->addWidgetEastOf(loadButton, "SaveAsButton");
    
    playButton = new ofxUIImageToggle(0.0f, 32.0f, 32.0f, 32.0f, false, "GUI/play.png", "PlayButton");
    //playButton->setLabelVisible(false);
    gui->addWidgetPosition(playButton, OFX_UI_WIDGET_POSITION_DOWN);
    //gui->addWidget(playButton);
    pauseButton = new ofxUIImageToggle(32, 32, false, "GUI/pause.png", "PauseButton");
    pauseButton->setLabelVisible(false);
    gui->addWidgetEastOf(pauseButton, "PlayButton");
    stopButton = new ofxUIImageButton(32, 32, false, "GUI/stop.png", "StopButton");
    stopButton->setLabelVisible(false);
    gui->addWidgetEastOf(stopButton, "PauseButton");
    recordButton = new ofxUIImageToggle(32, 32, false, "GUI/record_on.png", "RecordButton");
    recordButton->setLabelVisible(false);
    gui->addWidgetEastOf(recordButton, "StopButton");

    gui->addTextInput("Timecode", timeline->getCurrentTimecode(), 145.0f);
    timestampInput = (ofxUITextInput*)gui->getWidget("Timecode");
    (timestampInput)->setInputTriggerType(OFX_UI_TEXTINPUT_ON_ENTER);

    gui->autoSizeToFitWidgets();
    ofAddListener(gui->newGUIEvent, this, &ofApp::guiEvent);
    gui->loadSettings("GUI/guiSettings.xml");
    gui->setVisible(true);

    showIsPaused = false;
    toggleLines = false;
    timeline->setSpacebarTogglePlay(false);

}

void ofApp::loadConfig(){
    settings->pushTag("settings");
    timeline->setDurationInSeconds(settings->getAttribute("project_settings", "duration", 60));
    // load commouts
    settings->pushTag("commouts");
    int commOutCount = settings->getNumTags("commout");
    for (int i = 0; i < commOutCount; i++){
        string commOutType = settings->getAttribute("commout", "type", "", i);
        string commOutName = settings->getAttribute("commout", "name", "CommOutName"+ofToString(i), i);
        if (commOutType == "OSC"){
            OSCManager* o = new OSCManager(commOutName);
            o->setup();
            o->start();
            commouts[commOutName] = (CommOutManager*)o;
        }
    }
    settings->popTag();
    // load tracks
    settings->pushTag("tracks");
    int trackCount = settings->getNumTags("track");
    timeline->setPageName("Tracks");
    ofAddListener(timeline->events().bangFired, this, &ofApp::receivedEvent);
    ofAddListener(timeline->events().switched, this, &ofApp::receivedSwitchEvent);
    for (int i = 0; i < trackCount; i++){
        string trackType = settings->getAttribute("track", "type", "", i);
        string trackName = settings->getAttribute("track", "name", "TrackName"+ofToString(i), i);
        string commOutForTrack = settings->getAttribute("track", "commout", "", i);
        this->tracksToCommOut[trackName] = commOutForTrack;
        if (trackType == "OSC"){
            ofxTLOSCTrack* t2 = new ofxTLOSCTrack();
            t2->addLogger(logger);
            timeline->addTrack(trackName, t2);
        }
        else if (trackType == "Label"){
            ofxTLFlags* t3 = new ofxTLFlags();
            timeline->addFlags(trackName);
        }
        else if (trackType == "Audio"){
            string audioFile = settings->getAttribute("track", "file", "", i);
            bool setTimeControlTrack = ofToBool(settings->getAttribute("track", "setTimeControlTrack", "false", i));
            
            ofxTLAudioTrack* t4 = new ofxTLAudioTrack();
            t4->loadSoundfile(audioFile);
            timeline->addTrack(trackName, t4);
            
            if(setTimeControlTrack){
                timeline->setTimecontrolTrack(trackName);
            }
            
        }
    }
    settings->popTag();
    settings->popTag();
}


void ofApp::receivedEvent(ofxTLBangEventArgs& bang){
    //cout<<"llegó"<<endl;
    if (bang.track->getTrackType() == "OSCTrack"){
        ofxTLOSCTrackEventArgs* oscC = (ofxTLOSCTrackEventArgs*)(&bang);
        string managerName = this->tracksToCommOut[bang.track->getName()];
        OSCManager* manager = (OSCManager*)this->commouts[managerName];
        ofxOscMessage* p = new ofxOscMessage();
        p->operator=(oscC->m);
        OSCCommMessage* message = new OSCCommMessage(p);
        manager->storeMessage(message);
    }
}

void ofApp::receivedSwitchEvent(ofxTLSwitchEventArgs &tlSwitch){

}

void ofApp::guiEvent(ofxUIEventArgs &e){
    string name = e.getName();
    int kind = e.getKind();
    if(e.getName() == "PlayButton")
    {
        ofxUIToggle *toggle = e.getToggle();
        doPlay();
        cout<<"toggle pressed! "<<name<<" "<<toggle->getValue()<<endl;
    }
    if(e.getName() == "PauseButton")
    {
        ofxUIToggle *toggle = e.getToggle();
        doPause();
        cout<<"toggle pressed! "<<name<<" "<<toggle->getValue()<<endl;
    }
    if(e.getName() == "StopButton")
    {
        ofxUIImageButton* b = (ofxUIImageButton*) e.widget;
        if (b->getValue() == 0){
            doStop();
        }
        cout<<"button pressed! "<<name<<" "<<b->getValue()<<endl;
    }
    if(e.getName() == "RecordButton")
    {
        ofxUIToggle *toggle = e.getToggle();
        cout<<"toggle pressed! "<<name<<" "<<toggle->getValue()<<endl;
    }
    if(e.getName() == "Timecode"){
        //check input is correct
        string oldTimecode = timeline->getCurrentTimecode();
        ofxUITextInput* tinput = (ofxUITextInput*) e.widget;
        string newTimecode = tinput->getTextString();
        //ofxTimecode* t = new ofxTimecode();
        unsigned long long newTimeInMillis = timeline->getTimecode().millisForTimecode(newTimecode);
        if ((newTimeInMillis >= 0) && (newTimeInMillis <= timeline->getDurationInMilliseconds())){
            goTo(newTimeInMillis);
            cout<<"changed time from "<<oldTimecode<<" to "<<newTimecode<<endl;
        }
    }
    //Same as ctrl+s
    if(e.getName() == "SaveButton"){
        ofxUIImageButton* b = (ofxUIImageButton*) e.widget;
        if (b->getValue() == 0){
            timeline->saveTracksToFolder(timeline->getWorkingFolder());
            logger->addMessageToLog(success, "Data saved!");
        }
    }
    if(e.getName() == "SaveAsButton"){
        ofxUIImageButton* b = (ofxUIImageButton*) e.widget;
        if (b->getValue() == 0){
            ofFileDialogResult res = ofSystemLoadDialog("Choose a folder to save data", true);
            if (res.filePath != ""){
                cout<<"selected folder is "<<res.filePath<<endl;
                timeline->setWorkingFolder(res.filePath);
                timeline->saveTracksToFolder(ofFilePath::addTrailingSlash(res.filePath));
                logger->addMessageToLog(success, "Data saved to " + res.filePath);
                ofSetWindowTitle(res.filePath);
            }
        }
    }
    if(e.getName() == "LoadButton"){
        ofxUIImageButton* b = (ofxUIImageButton*) e.widget;
        if (b->getValue() == 0){
            ofFileDialogResult res = ofSystemLoadDialog("Choose a folder to load data", true);
            if (res.filePath != ""){
                cout<<"selected folder is "<<res.filePath<<endl;
                timeline->setWorkingFolder(res.filePath);
                timeline->loadTracksFromFolder(ofFilePath::addTrailingSlash(res.filePath));
                logger->addMessageToLog(success, "Data loaded from " + res.filePath);
                ofSetWindowTitle(res.filePath);
            }
        }
    }
}

void ofApp::goTo(long long millis){
    timeline->setCurrentTimeMillis(millis);
    logger->addMessageToLog(info, "Jumped to " + timeline->getCurrentTimecode());
}

//--------------------------------------------------------------
void ofApp::update(){
    for (map<string, CommOutManager*>::iterator it=commouts.begin(); it!=commouts.end(); ++it){
        if (!(it->second->getIsThreaded())){
            it->second->singleThreadedUpdate();
        }
    }
    
    if(!timestampInput->isFocused() || (timestampInput->isFocused() && timeline->getIsPlaying())){
        ((ofxUITextInput*)gui->getWidget("Timecode"))->setTextString(timeline->getCurrentTimecode());
    }
    
    logger->update();
    
    if (timeline->isDone()){
        doStop();
    }

}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackgroundGradient(ofColor(80),ofColor(250), OF_GRADIENT_BAR);
    ofSetColor(0,0,0);
    ofSetLineWidth(1.5f);
    ofRect(0.0f, 300.0f, 0.0f, ofGetWidth(), ofGetHeight() - 300.0f);
    ofPushMatrix();
    //ofTranslate(0.0f, 300.0f);
    timeline->draw();
    ofPopMatrix();

}

void ofApp::doPlay(){
    if(!timeline->getIsPlaying()){
        logger->addMessageToLog(info, "Starting play routine");
        timeline->play();
        logger->addMessageToLog(info, "Playing");
        
    }
}

void ofApp::doPause(){
    if (timeline->getIsPlaying()){
        timeline->togglePlay();
        showIsPaused = true;
    }
    else if (showIsPaused) {
        showIsPaused = false;
        timeline->togglePlay();
        logger->addMessageToLog(info, "Show unpaused");
    }
}

void ofApp::doStop(){
    playButton->setValue(0);
    pauseButton->setValue(0);
    timeline->stop();
    timeline->setCurrentTimeMillis(0);
    logger->addMessageToLog(info, "Show stopped");
}


//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if(key == ' '){
        timeline->play();
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
    logger->handleResize();
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}

void ofApp::exit()
{
    delete gui;
}
