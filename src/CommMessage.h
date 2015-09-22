//
//  CommMessage.h
//  OSC_MIDI_Test
//
//  Created by Christian Clark on 12/29/14.
//
//

#ifndef __OSC_MIDI_Test__CommMessage__
#define __OSC_MIDI_Test__CommMessage__

#include "ofMain.h"


enum CommMessageType {OSC, MIDI, HEART_BEAT, TCP, HTTP};

class CommMessage {
    
public:
    
    CommMessage(CommMessageType messageType_);
    ~CommMessage();
    
    CommMessageType messageType;
    
    CommMessageType getMessageType();

};


#endif /* defined(__OSC_MIDI_Test__CommMessage__) */
