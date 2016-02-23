//
//  MidiManager.cpp
//  
//
//  Created by James Bentley on 2/17/16.
//
//

#include "MidiManager.h"

MidiManager::MidiManager() {
    contentManager = NULL;
}

void MidiManager::setupMidi() {
    midiIn.listPorts();
    
    midiIn.openPort(0);
    
    midiIn.ignoreTypes(false, false, false);
    
    midiIn.addListener(this);
    
    midiIn.setVerbose(true);
}

void MidiManager::newMidiMessage(ofxMidiMessage& msg) {
    midiMessage = msg;
    
    stringstream text;
    // draw the last recieved message contents to the screen
    
    text << "control: " << midiMessage.control;
    cout<< text.str() <<endl;
    text.str(""); // clear
    
    text << "value: " << midiMessage.value;
    cout<< text.str() <<endl;
    text.str(""); // clear
    
    cout<<midiMessage.toString()<<endl;
    vector<Effect *> * effects = contentManager->frame.effects;
    
    int controller = (int)(midiMessage.control/10);
    cout<<controller<<endl;
    
    int parameter = (int)(midiMessage.control%10);
    cout<<parameter<<endl;
    
    int val = midiMessage.value;
    
    for(int i = 0; i < effects->size(); i++) {
        Effect* effect = (*effects)[i];
        if(effect->getControllerNumber() == controller) {
            if(parameter > 1) {
                parameter -= 2;
                if(parameter < effect->floatUniforms.size()) {
                    float value = ofMap(val, 0, 127, effect->floatUniformsVector[parameter]->getMin(), effect->floatUniformsVector[parameter]->getMax());
                    effect->updateFromFloat(value, parameter);
                    contentManager->frame.activeEffects[controller-1]->set(true);
                    for(auto it = effect->floatUniforms.begin(); it != effect->floatUniforms.end(); it++) {
                        if(it->second == effect->floatUniformsVector[parameter]) {
                            effect->lastParamChanged = it->first;
                            break;
                        }
                    }
                }
            } else {
                if (parameter == 0) {
                    if(val == 127) {
                        if(contentManager->frame.activeEffects[controller-1]->get()) {
                            contentManager->frame.activeEffects[controller-1]->set(false);
                        } else {
                            contentManager->frame.activeEffects[controller-1]->set(true);
                        }
                    }
                } else if (parameter == 1) {
                    if(val == 127) {
                        effect->fftConnected[effect->lastParamChanged]->set(!(effect->fftConnected[effect->lastParamChanged]->get()));
                    }
                }
            }
        }
    }
}