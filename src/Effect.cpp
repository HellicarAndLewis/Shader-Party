//
//  Effect.cpp
//  DigitalEphemera
//
//  Created by James Bentley on 12/17/15.
//
//

#include "Effect.h"

Effect::Effect() {
    uniformImageSet = false;
    motionAmpLoaded = false;
    numFFTChannels = 0;
    lastParamChanged = "";
}

void Effect::setUniformImage(string name, ofImage *img) {
    uniformImageName = name;
    uniformImage = img;
    uniformImageSet = true;
}

void Effect::setMotionAmp(MotionAmplifier* amp) {
    motionAmp = amp;
    motionAmpLoaded = true;
}

void Effect::setUniformFlowField(ofTexture* texture) {
    vectorField = texture;
}


void Effect::setGuiPosition(int x, int y) {
    gui.setPosition(x, y);
}

void Effect::setupGui(string name,int numChannels) {
    gui.setup(name, "settings/" + name + ".xml");
    active.set(name, false);
    numFFTChannels = numChannels/2;
}

void Effect::loadSettings() {
    gui.loadFromFile("settings/" + gui.getName() + ".xml");
}

bool Effect::loadShader(string ShaderPath) {
    if(!shader.load(ShaderPath)) {
        return shader.load("shaders/DummyVert.glsl", ShaderPath);
    } else {
        return true;
    }
}

void Effect::addUniformFloat(string name, string parameterName, float initialValue, float minValue, float maxValue){
    floatUniforms[name] = new ofParameter<float>();
    floatUniforms[name]->set(parameterName, initialValue, minValue, maxValue);
    floatUniformsVector.push_back(floatUniforms[name]);
    fftConnected[name] = new ofParameter<bool>();
    fftConnected[name]->set(parameterName + " connected", false);
    lastParamChanged= parameterName;
#ifdef USING_FFT
    fftChannels[name] = new ofParameter<int>();
    fftChannels[name]->set(parameterName + " channel", 0, 0, numFFTChannels);
#endif
    gui.add(*floatUniforms[name]);
    gui.add(*fftConnected[name]);
#ifdef USING_FFT
    gui.add(*fftChannels[name]);
#endif
}

void Effect::addUniformBool(string name, string parameterName, bool initialValue) {
    boolUniforms[name] = new ofParameter<bool>();
    boolUniforms[name]->set(parameterName, initialValue);
    gui.add(*boolUniforms[name]);
}

void Effect::addUniformVectorArray(string name, float* _vectorArray, int _size) {
    uniformVectorArrayNames.push_back(name);
    uniformVectorArrays.push_back(_vectorArray);
    uniformVectorArraySizes.push_back(_size);
}

void Effect::beginShader() {
    shader.begin();
}

void Effect::setUniqueUniforms() {
    for(auto it = floatUniforms.begin(); it != floatUniforms.end(); it++) {
        shader.setUniform1f(it->first, it->second->get());
    }
    if(uniformImageSet) {
        shader.setUniformTexture(uniformImageName, *uniformImage, 1);
        shader.setUniform2f(uniformImageName + "Size", uniformImage->getWidth(), uniformImage->getHeight());
    }
    for(int i = 0; i < uniformVectorArrays.size(); i++) {
        shader.setUniform2fv(uniformVectorArrayNames[i], uniformVectorArrays[i], uniformVectorArraySizes[i]);
    }
}

void Effect::endShader() {
    shader.end();
}

ofShader* Effect::getShaderRef() {
    return &shader;
}

void Effect::drawGui() {
    gui.draw();
}

float Effect::getGuiWidth() {
    return gui.getWidth();
}


ofParameterGroup* Effect::getParametersRef() {
    return &uniformsGroup;
}

void Effect::apply(ofFbo* fboIn, ofFbo* fboOut, ofMesh* mesh) {
    fboOut->begin();
    ofClear(0);
    shader.begin();
    setSharedUniforms(fboIn->getTexture(), *vectorField, ofGetWidth(), ofGetHeight());
    setUniqueUniforms();
    mesh->draw();
    shader.end();
    fboOut->end();
}

void Effect::apply(ofFbo* fboIn, ofFbo* fboOut) {
    fboOut->begin();
    ofClear(0);
    shader.begin();
    setSharedUniforms(fboIn->getTexture(), *vectorField, ofGetWidth(), ofGetHeight());
    setUniqueUniforms();
    fboIn->draw(0, 0);
    shader.end();
    fboOut->end();
}

void Effect::updateFromFFT(vector<float> fft, float upperCut, float lowerCut) {
#ifdef USING_FFT
    for(auto it = fftConnected.begin(); it != fftConnected.end(); it++) {
        string paramName = it->first;
        bool paramConnected = it->second->get();
        if(paramConnected) {
            float val = fft[fftChannels.find(paramName)->second->get()];
            auto param = floatUniforms.find(paramName)->second;
            val = ofMap(val, lowerCut, upperCut, param->getMin(), param->getMax(), true);
            param->set(val);
            //floatUniforms.find(paramName)->second->set(val);
        }
    }
#endif
}

void Effect::updateFromFloat(float value, float upperCut, float lowerCut) {
    for(auto it = fftConnected.begin(); it != fftConnected.end(); it++) {
        string paramName = it->first;
        bool paramConnected = it->second->get();
        if(paramConnected) {
            float val = value;
            auto param = floatUniforms.find(paramName)->second;
            val = ofMap(value, lowerCut, upperCut, param->getMin(), param->getMax(), true);
            param->set(val);
        }
    }
}

void Effect::updateFromFloat(float value, int paramIndex) {
    if(paramIndex < floatUniformsVector.size()) {
        floatUniformsVector[paramIndex]->set(value);
    }
}