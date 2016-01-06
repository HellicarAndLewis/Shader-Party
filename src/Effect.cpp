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
    uniformCircularTexSet = false;
    uniformVectorArraySet = false;
    motionAmpLoaded = false;
    numFFTChannels = 0;
}

void Effect::setUniformImage(string name, ofImage *img) {
    uniformImageName = name;
    uniformImage = img;
    uniformImageSet = true;
}

void Effect::setUniformCircularTex(string name, CircularTexture* tex) {
    uniformCircularTexName = name;
    uniformCircularTex = tex;
    uniformCircularTexSet = true;
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
    numFFTChannels = numChannels/2;
}

void Effect::loadSettings() {
    gui.loadFromFile("settings/" + gui.getName() + ".xml");
}

bool Effect::loadShader(string ShaderPath) {
    return shader.load("shaders/DummyVert.glsl", ShaderPath);
}

void Effect::addUniformFloat(string name, string parameterName, float initialValue, float minValue, float maxValue){
    floatUniforms[name] = new ofParameter<float>();
    floatUniforms[name]->set(parameterName, initialValue, minValue, maxValue);
    fftConnected[name] = new ofParameter<bool>();
    fftConnected[name]->set(parameterName + " connected", false);
    fftChannels[name] = new ofParameter<int>();
    fftChannels[name]->set(parameterName + " channel", 0, 0, numFFTChannels);
    gui.add(*floatUniforms[name]);
    gui.add(*fftConnected[name]);
    gui.add(*fftChannels[name]);
}

void Effect::addUniformBool(string name, string parameterName, bool initialValue) {
    boolUniforms[name] = new ofParameter<bool>();
    boolUniforms[name]->set(parameterName, initialValue);
    gui.add(*boolUniforms[name]);
}

void Effect::addUniformVectorArray(string name, float* _vectorArray, int _size) {
    uniformVectorArrayName = name;
    uniformVectorArray = _vectorArray;
    uniformVectorArraySize = _size;
    uniformVectorArraySet = true;
}

void Effect::beginShader() {
    shader.begin();
}

void Effect::setUniqueUniforms() {
    for(auto it = floatUniforms.begin(); it != floatUniforms.end(); it++) {
        shader.setUniform1f(it->first, it->second->get());
    }
//    for(auto it = boolUniforms.begin(); it != boolUniforms.end(); it++) {
//        shader.setUniform1f(it->first, it->second->get());
//        shader.setUniform1
//    }
    if(uniformImageSet) {
        shader.setUniformTexture(uniformImageName, *uniformImage, 1);
        shader.setUniform2f(uniformImageName + "Size", uniformImage->width, uniformImage->height);
    }
    if(uniformCircularTexSet) {
        shader.setUniformTexture(uniformCircularTexName, uniformCircularTex->texData.textureTarget,uniformCircularTex->texData.textureID,  1);
        shader.setUniform2f(uniformCircularTexName + "Size", uniformCircularTex->width, uniformCircularTex->height);
        shader.setUniform1f("depthOffset", uniformCircularTex->depth);
    }
    if(uniformVectorArraySet) {
        shader.setUniform2fv(uniformVectorArrayName, uniformVectorArray, uniformVectorArraySize);
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

void Effect::apply(ofFbo* fboIn, ofFbo* fboOut) {
    fboOut->begin();
    ofClear(0);
    shader.begin();
    setSharedUniforms(fboIn->getTextureReference(), *vectorField, ofGetWidth(), ofGetHeight());
    setUniqueUniforms();
    fboIn->draw(0, 0);
    shader.end();
    fboOut->end();
}

void Effect::updateFromFFT(vector<float> fft) {
    for(auto it = fftConnected.begin(); it != fftConnected.end(); it++) {
        string paramName = it->first;
        bool paramConnected = it->second->get();
        if(paramConnected) {
            float val = fft[fftChannels.find(paramName)->second->get()];
            auto param = floatUniforms.find(paramName)->second;
            val = ofMap(val, 0, 1, param->getMin(), param->getMax());
            param->set(val);
            //floatUniforms.find(paramName)->second->set(val);
        }
    }
}