//
//  midiManager.h
//  
//
//  Created by James Bentley on 2/17/16.
//
//

#ifndef ____midiManager__
#define ____midiManager__

#include "ofMain.h"
#include "ofxMidi.h"
#include "ContentManager.h"

class MidiManager : public ofxMidiListener {
public:
    MidiManager();
    void setupMidi();
    void setContentManager(ContentManager* _contentManager) { contentManager = _contentManager; };
private:
    ContentManager* contentManager;
    ofxMidiIn midiIn;
    ofxMidiMessage midiMessage;
    void newMidiMessage(ofxMidiMessage& eventArgs);
};

#endif /* defined(____midiManager__) */
