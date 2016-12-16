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
#include "videoTiler.h"
#include "MotionAmplifier.h"

class MidiManager : public ofxMidiListener {
public:
    MidiManager();
    void setupMidi();
    void setContentManager(ContentManager* _contentManager) { contentManager = _contentManager; };
    void setVideoTiler(videoTiler* _videoTiler) { videoTiler = _videoTiler; };
    void setMotionAmplifier(MotionAmplifier* _motionAmplifier) { motionAmplifier = _motionAmplifier; };
private:
    ContentManager* contentManager;
    videoTiler* videoTiler;
    MotionAmplifier* motionAmplifier;
    ofxMidiIn midiIn;
    ofxMidiMessage midiMessage;
    void newMidiMessage(ofxMidiMessage& eventArgs);
};

#endif /* defined(____midiManager__) */
