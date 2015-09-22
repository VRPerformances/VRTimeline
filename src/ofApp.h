#pragma once

#include "ofMain.h"
#include "ofxTimeline.h"
#include "ofxUI.h"
#include "ofxXmlSettings.h"
#include "ofxTLOSCTrack.h"
#include "CommOutManager.h"
#include "OSCManager.h"
#include "Logger.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
        void loadConfig();
		void update();
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
    
        void receivedEvent(ofxTLBangEventArgs& bang);
        void receivedSwitchEvent(ofxTLSwitchEventArgs& tlSwitch);
        void guiEvent(ofxUIEventArgs& e);
        void loggerEvent(LoggerEventArgs& e);
        void exit();
        void doPlay();
        void doPause();
        void doStop();
        void goTo(long long millis);

    
        ofxTimeline* timeline;
        ofxXmlSettings* settings;
        map<string, CommOutManager*> commouts;
        map<string, string> tracksToCommOut;
        ofxUICanvas* gui;
        ofxUIImageToggle* playButton;
        ofxUIImageToggle* pauseButton;
        ofxUIImageButton* stopButton;
        ofxUIImageToggle* recordButton;
        ofxUILabel* timestampLabel;
        ofxUITextInput* timestampInput;
        ofxUIImageButton* saveButton;
        ofxUIImageButton* saveAsButton;
        ofxUIImageButton* loadButton;
        bool toggleLines;
        bool showIsPaused;
        Logger* logger;


		
};
