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
    
   cout<<midiMessage.getStatusString(midiMessage.status)<<endl;
    
    cout<<midiMessage.toString()<<endl;
    vector<Effect *> * effects = contentManager->frame.effects;
    
    int controller = (int)(midiMessage.control/10);
    cout<<controller<<endl;
    
    int parameter = (int)(midiMessage.control%10);
    cout<<parameter<<endl;
    
    int val = midiMessage.value;
    
    if(controller == 2) {
        if(parameter == 3) {
            int numTiles = ofMap(val, 0, 127, videoTiler->size.getMin(), videoTiler->size.getMax(), true);
            videoTiler->size.set(numTiles);
        }
    }
    if(controller == 1) {
        if(parameter == 4) {
            float strength = ofMap(val, 0, 127, motionAmplifier->strength.getMin(), motionAmplifier->strength.getMax(), true);
            motionAmplifier->setStrength(strength);
        } else if(parameter == 5) {
            float learnRate = ofMap(val, 0, 127, motionAmplifier->learningRate.getMin(), motionAmplifier->learningRate.getMax(), true);
            motionAmplifier->setLearningRate(learnRate);
        }
    }
    
    for(int i = 0; i < effects->size(); i++) {
        Effect* effect = (*effects)[i];
        if(effect->getControllerNumber() == controller) {
            if(parameter > 1) {
                parameter -= 2;
                if(parameter < effect->floatUniforms.size()) {
                    float value = ofMap(val, 0, 127, effect->floatUniformsVector[parameter]->getMin(), effect->floatUniformsVector[parameter]->getMax());
                    effect->updateFromFloat(value, parameter);
                    effect->setActiveParameter(true);
                    //contentManager->frame.activeEffects[controller-1]->set(true);
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
                        effect->setActiveParameter(!(*effect->getActiveParameter()));
                    }
                } else if (parameter == 1) {
                    if(val == 127) {
                        effect->fftConnected[effect->lastParamChanged]->set(!(effect->fftConnected[effect->lastParamChanged]->get()));
                    }
                }
            }
        }
        if(controller == 0) {
            if(effect->getControllerNumber() == 9) {
                cout<<val<<endl;
                if(midiMessage.getStatusString(midiMessage.status) == "Note On") {
                    effect->fftConnected[effect->lastParamChanged]->set(!(effect->fftConnected[effect->lastParamChanged]->get()));
                }
            }
        }
    }
}