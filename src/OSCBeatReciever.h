//
//  OSCReciever.hpp
//  ShaderParty
//
//  Created by cafe on 12/12/2016.
//
//

#pragma once

#include "ofMain.h"
#include "ofxOsc.h"

class OSCBeatReciever {
public:
    OSCBeatReciever();
    void setup(int _port);
    void update();
    void setValueToModify(float* v) { valueToModify = v;};
    int getPort() { return port;};
    void setActive(ofParameter<bool>* _active) { active = _active;};
    float getConfidence() { return confidence; };
    int k;
private:
    int port;
    float confidence;
    ofxOscReceiver reciever;
    ofParameter<bool>* active;
    float* valueToModify;
};

