//
//  OSCReciever.cpp
//  ShaderParty
//
//  Created by cafe on 12/12/2016.
//
//

#include "OSCBeatReciever.h"

OSCBeatReciever::OSCBeatReciever() {
    
}

void OSCBeatReciever::setup(int _port) {
    reciever.setup(_port);
    port = _port;
    cout << "listening for osc messages on port " << _port << "\n";
}

void OSCBeatReciever::update() {
    k = 0;
    while (reciever.hasWaitingMessages()) {
        ofxOscMessage m;
        reciever.getNextMessage(m);
        k++;
        if(m.getAddress() == "/Flowers")
            confidence = m.getArgAsFloat(0);
        if(valueToModify != nullptr)
            *valueToModify = confidence;
    }
    //cout<<k<<endl;
}
