//
//  ofxTLOSCTrack.cpp
//  example-emptyTrackTemplates
//
//  Created by Christian Clark on 1/6/15.
//
//

#include "ofxTLOSCTrack.h"
#include "ofxTimeline.h"
#include "ofxHotKeys.h"

//flagssss
ofxTLOSCTrack::ofxTLOSCTrack() {
    editing = false;
    clickedOSCKeyFrame = NULL;
	timeInMillisGUI = -1;
    
    gui = new ofxUICanvas();
    gui->addLabel("OSC KEY FRAME");
    gui->addSpacer();
	gui->addTextInput("Timecode", "00:00:00:000");
    gui->addSpacer();
    gui->addLabel("Address", OFX_UI_FONT_SMALL);
    gui->addTextInput("ADDRESS", "")->setAutoClear(false);
    gui->addSpacer();
    gui->addLabelButton("NEW INT PARAM", false);
    gui->addLabelButton("NEW FLOAT PARAM", false);
    gui->addLabelButton("NEW STRING PARAM", false);
    gui->addSpacer();
    gui->addLabelToggle("ENABLED", enabled)->getLabelWidget()->setColorFill(ofColor(255, 0, 0));
    gui->addSpacer();
    gui->addLabel("PARAMETERS", OFX_UI_FONT_SMALL);
	((ofxUITextInput*)gui->getWidget("Timecode"))->setInputTriggerType(OFX_UI_TEXTINPUT_ON_UNFOCUS);
    gui->autoSizeToFitWidgets();
    ofAddListener(gui->newGUIEvent,this,&ofxTLOSCTrack::guiEvent);
    //gui->loadSettings("GUI/guiSettings.xml");
    gui->setVisible(false);
}

void ofxTLOSCTrack::draw(){
    
    if(bounds.height < 2){
        return;
    }
    
    ofPushStyle();
    
    // no custom draw for now
    ofxTLBangs::draw();
    
    ofPopStyle();
    
}

//main function to get values out of the timeline, operates on the given value range
bool ofxTLOSCTrack::mousePressed(ofMouseEventArgs& args, long millis){
    
    //if we get all the way here we didn't click on a text field and we aren't
    //currently entering text so proceed as normal
    
    bool ret = true;
    if(!editing){
        ret = ofxTLBangs::mousePressed(args, millis);
    }
    
    return ret;
    
}

void ofxTLOSCTrack::mouseDragged(ofMouseEventArgs& args, long millis){
    ofxTLBangs::mouseDragged(args, millis);
}

void ofxTLOSCTrack::mouseReleased(ofMouseEventArgs& args, long millis){
    ofxTLBangs::mouseReleased(args, millis);
}

void ofxTLOSCTrack::keyPressed(ofKeyEventArgs& args){
    if(editing){
        //enter key submits the values
        //This could be done be responding to the event from the text field itself...
        if(args.key == OF_KEY_RETURN){
            //updateKeyFrameInfoWithGUI();
			clickedOSCKeyFrame->time = timeInMillisGUI;
            editing = false;
            gui->setVisible(false);
            clickedOSCKeyFrame= NULL;
            timeline->dismissedModalContent();
            timeline->flagTrackModified(this);
        }
    }
    else{
        ofxTLBangs::keyPressed(args);
    }
}
void ofxTLOSCTrack::updateKeyFrameInfoWithGUI(){
    if(clickedOSCKeyFrame!=NULL){
        clickedOSCKeyFrame->m = editingMessage;
		clickedOSCKeyFrame->time = timeInMillisGUI;
    }
}


ofxTLKeyframe* ofxTLOSCTrack::newKeyframe(){
    ofxTLOSCTrackKeyFrame* key = new ofxTLOSCTrackKeyFrame();
    return key;
}

void ofxTLOSCTrack::unselectAll(){
    /*for(int i = 0; i < selectedKeyframes.size(); i++){
     ((ofxTLFlag*)selectedKeyframes[i])->textField.disable();
     }*/
    ofxTLKeyframes::unselectAll();
}

void ofxTLOSCTrack::restoreKeyframe(ofxTLKeyframe* key, ofxXmlSettings& xmlStore){
    ofxTLOSCTrackKeyFrame* triggerKey = (ofxTLOSCTrackKeyFrame*)key;
    string address = xmlStore.getValue("address", "");
    int numArgs = ofToInt(xmlStore.getValue("numArgs","0"));
    bool isEnabled = ofToInt(xmlStore.getValue("enabled","1"));
    
    triggerKey->m.setAddress(address);
    triggerKey->enabled=isEnabled;
    
    for(int i=0; i<numArgs; i++){
        string type = xmlStore.getValue("type_" + ofToString(i),"none");
        
        if(type=="int32"){
            int32_t value = ofToInt(xmlStore.getValue("value_" + ofToString(i),"0"));
            triggerKey->m.addIntArg(value);
        }
        
        if(type=="int64"){
            int64_t value = ofToInt(xmlStore.getValue("value_" + ofToString(i),"0"));
            triggerKey->m.addInt64Arg(value);
        }
        
        if(type=="float"){
            float value = ofToFloat(xmlStore.getValue("value_" + ofToString(i),"0.0"));
            triggerKey->m.addFloatArg(value);
        }
        
        if(type=="string"){
            string value = xmlStore.getValue("value_" + ofToString(i),"");
            triggerKey->m.addStringArg(value);
        }
        
        //blob not supported

    }
    
}

void ofxTLOSCTrack::storeKeyframe(ofxTLKeyframe* key, ofxXmlSettings& xmlStore){
    ofxTLOSCTrackKeyFrame* triggerKey = (ofxTLOSCTrackKeyFrame*)key;
    
    int numArgs = triggerKey->m.getNumArgs();
    string address = triggerKey->m.getAddress();
    bool isEnabled = triggerKey->enabled;
    
    xmlStore.addValue("address",address);
    xmlStore.addValue("numArgs", ofToString(numArgs));
    xmlStore.addValue("enabled", ofToString(isEnabled));
    
    for (int i=0; i<numArgs; i++){
        // we store every argument
        string type = triggerKey->m.getArgTypeName(i);
        
        // for each argument we store its type and its value
        xmlStore.addValue("type_" + ofToString(i), type);
        
        if(type=="int32"){
            int32_t value = triggerKey->m.getArgAsInt32(i);
            xmlStore.addValue("value_" + ofToString(i), ofToString(value));
        }
        
        if(type=="int64"){
            int64_t value = triggerKey->m.getArgAsInt64(i);
            xmlStore.addValue("value_" + ofToString(i), ofToString(value));
        }
        
        if(type=="float"){
            float value = triggerKey->m.getArgAsFloat(i);
            xmlStore.addValue("value_" + ofToString(i), ofToString(value));
        }
        
        if(type=="string"){
            string value = triggerKey->m.getArgAsString(i);
            xmlStore.addValue("value_" + ofToString(i), value);
        }
        
        //blob not supported
        
    }
}

void ofxTLOSCTrack::willDeleteKeyframe(ofxTLKeyframe* keyframe){
    /*ofxTLFlag* flag = (ofxTLFlag*)keyframe;
     if(flag->textField.getIsEditing()){
     timeline->dismissedModalContent();
     timeline->flagTrackModified(this);
     }
     flag->textField.disable();*/
}

void ofxTLOSCTrack::bangFired(ofxTLKeyframe* key){
    ofxTLOSCTrackEventArgs args;
    args.sender = timeline;
    args.track = this;
    //play solo change
    //args.currentMillis = timeline->getCurrentTimeMillis();
    args.currentMillis = currentTrackTime();
    args.currentPercent = timeline->getPercentComplete();
    args.currentFrame = timeline->getCurrentFrame();
    args.currentTime = timeline->getCurrentTime();
    args.m = ((ofxTLOSCTrackKeyFrame*)key)->m;
    args.enabled = ((ofxTLOSCTrackKeyFrame*)key)->enabled;
	logger->addMessageToLog(success, "Firing OSC bang");
    ofNotifyEvent(events().bangFired, args);
}

string ofxTLOSCTrack::getTrackType(){
    return "OSCTrack";
}

void ofxTLOSCTrack::addOSCMessage(ofxOscMessage m) {
    addOSCMessageAtTime(m, timeline->getCurrentTimeMillis());
}

void ofxTLOSCTrack::addOSCMessageAtTime(ofxOscMessage m, unsigned long long time){
    ofxTLKeyframe* keyFrame = newKeyframe();
    ofxTLOSCTrackKeyFrame* oscMessage = (ofxTLOSCTrackKeyFrame*)keyFrame;
    setKeyframeTime(keyFrame, time);
    oscMessage->m = m;
    keyframes.push_back(keyFrame);
    updateKeyframeSort();
    timeline->flagTrackModified(this);
}

void ofxTLOSCTrack::guiEvent(ofxUIEventArgs &e){
    string name = e.getName();
    int kind = e.getKind();
    
    //cout << "got event from: " << name << endl;
    
    string prefix = "PARAM_";
    
    //if is a param, we modify the osc message selected
    if(name.substr(0, prefix.size())==prefix){
        
        vector<string> res = ofSplitString(name, "_");
        if(res.size()==3){
            //if not 3, then we do not process it
            int argIndex = ofToInt(res[2]);
            
            if(res[1]=="STRING"){
                ofxUITextInput* t = (ofxUITextInput *) e.widget;
                
                ofxOscArg* oA = clickedOSCKeyFrame->m.getOscArgAt(argIndex);
                
                ofxOscArgString* oS = (ofxOscArgString*) oA;
                
                oS->set(t->getTextString().c_str());
            }
            
            if(res[1]=="INT32"){
                ofxUINumberDialer *nD = (ofxUINumberDialer *) e.widget;
                
                ofxOscArg* oA = clickedOSCKeyFrame->m.getOscArgAt(argIndex);
                
                ofxOscArgInt32* oI = (ofxOscArgInt32*) oA;
                
                oI->set(nD->getValue());

            }
            
            if(res[1]=="FLOAT"){
                ofxUINumberDialer *nD = (ofxUINumberDialer *) e.widget;
                
                ofxOscArg* oA = clickedOSCKeyFrame->m.getOscArgAt(argIndex);
                
                ofxOscArgFloat* oF = (ofxOscArgFloat*) oA;
                
                oF->set(nD->getValue());

            }
            
        }
        
    }
    else{
    // if not, we create the new params according to the event name
        if(kind == OFX_UI_WIDGET_LABELBUTTON){
            
            ofxUILabelButton *l = (ofxUILabelButton *) e.widget;
            
            int numArgs = clickedOSCKeyFrame->m.getNumArgs();
            
            numArgs += 1;
            
            if (name=="NEW INT PARAM"){
                
                if(l->getValue()==0){
                    gui->addNumberDialer("PARAM_INT32_" + ofToString(numArgs-1), std::numeric_limits<int32_t>::min(), std::numeric_limits<int32_t>::max(), 0.0f, 0);
                    clickedOSCKeyFrame->m.addIntArg(0);
                }
                
            }
            
            if (name=="NEW FLOAT PARAM"){
                
                if(l->getValue()==0){
                    gui->addNumberDialer("PARAM_INT32_" + ofToString(numArgs-1), std::numeric_limits<float>::min(), std::numeric_limits<float>::max(), 0.0f, 0);
                    clickedOSCKeyFrame->m.addFloatArg(0.0f);
                }
                
            }
            
            if (name=="NEW STRING PARAM"){
                
                if(l->getValue()==0){
                    gui->addTextInput("PARAM_STRING_" + ofToString(numArgs-1), "")->setAutoClear(false);
                    clickedOSCKeyFrame->m.addStringArg("");
                }
                
            }
            
            gui->autoSizeToFitWidgets();
            gui->setVisible(true);

        }
        
        if(kind==OFX_UI_WIDGET_LABELTOGGLE){
            if (name=="ENABLED"){
                ofxUILabelToggle *n = (ofxUILabelToggle *) e.widget;
                clickedOSCKeyFrame->enabled = n->getValue();
            }
        }
        
        if(kind==OFX_UI_WIDGET_TEXTINPUT){
            if (name=="ADDRESS"){
                ofxUITextInput *n = (ofxUITextInput *) e.widget;
                clickedOSCKeyFrame->m.setAddress(n->getTextString());
            }
			if (name == "Timecode"){
				ofxUITextInput* tinput = (ofxUITextInput*) e.widget;
				string newTimecode = tinput->getTextString();
				unsigned long long newTimeInMillis = timeline->getTimecode().millisForTimecode(newTimecode);
				if ((newTimeInMillis >= 0) && (newTimeInMillis <= timeline->getDurationInMilliseconds())){
					this->timeInMillisGUI = newTimeInMillis;
				}
			}
        }
    }
    
    
}

void ofxTLOSCTrack::selectedKeySecondaryClick(ofMouseEventArgs& args){
    
    if(!editing){
        
        editing = true;
        if(selectedKeyframe!=NULL){
            clickedOSCKeyFrame= (ofxTLOSCTrackKeyFrame*) selectedKeyframe;
			timeInMillisGUI = clickedOSCKeyFrame->time;
            
            //setting common parameters
            ofxUITextInput* textInput = (ofxUITextInput*) gui->getWidget("ADDRESS");
            ofxUILabelToggle* enabledToggle = (ofxUILabelToggle*) gui->getWidget("ENABLED");
			ofxUITextInput* inputTimecode = (ofxUITextInput*) gui->getWidget("Timecode");
            
            textInput->setTextString(clickedOSCKeyFrame->m.getAddress());
            enabledToggle->setValue(clickedOSCKeyFrame->enabled);
			inputTimecode->setTextString(timeline->getTimecode().timecodeForMillis(timeInMillisGUI));
            
            //setting custom parameters
            
            //cleaning paramWidgets
            
            vector<ofxUIWidget*> v = gui->getWidgets();
            map<string,int> toRemove;
            
            for (int i=0; i<v.size(); i++){
                ofxUIWidget* w = v[i];
                string prefix = "PARAM_";
                if(w->getName().substr(0, prefix.size())==prefix){
                    if(toRemove.find(w->getName())==toRemove.end()){
                        toRemove.insert(std::pair<string,int>(w->getName(),1));
                    }
                }
            }
            
            map<string, int>::iterator it;
            
            for(it = toRemove.begin(); it != toRemove.end(); it++) {
                gui->removeWidget(it->first);
            }
            
            //adding new parameters from selected keyframe
            
            int numArgs = clickedOSCKeyFrame->m.getNumArgs();
            
            for(int i=0; i<numArgs; i++){
                string type = clickedOSCKeyFrame->m.getArgTypeName(i);
                
                if(type=="int32"){
                    int32_t value = clickedOSCKeyFrame->m.getArgAsInt32(i);
                    gui->addNumberDialer("PARAM_INT32_" + ofToString(i), std::numeric_limits<int32_t>::min(), std::numeric_limits<int32_t>::max(), value, 0);
                }
                
                if(type=="int64"){
                    int64_t value = clickedOSCKeyFrame->m.getArgAsInt64(i);
                    gui->addNumberDialer("PARAM_INT64_" + ofToString(i), std::numeric_limits<int64_t>::min(), std::numeric_limits<int64_t>::max(), value, 0);
                }
                
                if(type=="float"){
                    float value = clickedOSCKeyFrame->m.getArgAsFloat(i);
                    gui->addNumberDialer("PARAM_FLOAT_" + ofToString(i), std::numeric_limits<float>::min(), std::numeric_limits<float>::max(), value, 3);
                }
                
                if(type=="string"){
                    string value = clickedOSCKeyFrame->m.getArgAsString(i);
                    gui->addTextInput("PARAM_STRING_" + ofToString(i), value)->setAutoClear(false);
                }
                
                //blob not supported
            }
            
            //cout << "key position x:" << ofToString(selectedKeyframe->screenPosition.x) << " y:" << ofToString(selectedKeyframe->screenPosition.y) << endl;
            
            gui->autoSizeToFitWidgets();
            gui->setPosition(args.x, args.y);
            gui->setVisible(true);
			
        }
        
        timeline->presentedModalContent(this);
    }
}

void ofxTLOSCTrack::addLogger(Logger* logger_){
	this->logger = logger_;
}
