//
//  ofxTLOSCTrack.h
//  example-emptyTrackTemplates
//
//  Created by Christian Clark on 1/6/15.
//
//

#ifndef __example_emptyTrackTemplates__ofxTLOSCTrack__
#define __example_emptyTrackTemplates__ofxTLOSCTrack__

#include "ofMain.h"
#include "ofxTLBangs.h"
#include "ofxUI.h"
#include "ofxOsc.h"
#include "Logger.h"

class ofxTLOSCTrackEventArgs : public ofxTLBangEventArgs {
public:
    ofxOscMessage m;
    bool enabled;
};

class ofxTLOSCTrackKeyFrame : public ofxTLKeyframe {
public:
    ofxOscMessage m;
    bool enabled;
};

class ofxTLOSCTrack : public ofxTLBangs {
public:
    
    ofxTLOSCTrack();
    
    virtual void draw();
    
    virtual bool mousePressed(ofMouseEventArgs& args, long millis);
    virtual void mouseDragged(ofMouseEventArgs& args, long millis);
    virtual void mouseReleased(ofMouseEventArgs& args, long millis);
    
    virtual void keyPressed(ofKeyEventArgs& args);
    virtual void unselectAll();
    
    virtual string getTrackType();
    
    virtual void addOSCMessage(ofxOscMessage m);
    virtual void addOSCMessageAtTime(ofxOscMessage m, unsigned long long time);
    //virtual ofxTLFlag* getFlagWithKey(string key);
    
    void updateKeyFrameInfoWithGUI();
    void guiEvent(ofxUIEventArgs &e);
    ofxUICanvas *gui;

	void addLogger(Logger* logger_);
    
protected:
    
    virtual ofxTLKeyframe* newKeyframe();
    virtual void restoreKeyframe(ofxTLKeyframe* key, ofxXmlSettings& xmlStore);
    virtual void storeKeyframe(ofxTLKeyframe* key, ofxXmlSettings& xmlStore);
    virtual void bangFired(ofxTLKeyframe* key);
    virtual void willDeleteKeyframe(ofxTLKeyframe* keyframe);
    void selectedKeySecondaryClick(ofMouseEventArgs& args);
    
    //only set per mousedown/mouseup cycle
    ofxTLOSCTrackKeyFrame* clickedOSCKeyFrame;
    
    bool editing;
    ofxOscMessage editingMessage;
	unsigned long long timeInMillisGUI;

	Logger* logger;

};


#endif /* defined(__example_emptyTrackTemplates__ofxTLOSCTrack__) */
