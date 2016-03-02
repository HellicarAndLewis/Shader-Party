//
//  Effect.h
//  DigitalEphemera
//
//  Created by James Bentley on 12/17/15.
//
//

#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "MotionAmplifier.h"

class Effect {
    ofShader shader;
    map<string, ofParameter<bool>* > boolUniforms;
    ofParameter<bool> active;
    string uniformImageName;
    ofImage* uniformImage;
    ofTexture* vectorField;
    MotionAmplifier* motionAmp;
    vector<string> uniformVectorArrayNames;
    vector<float*> uniformVectorArrays;
    vector<int> uniformVectorArraySizes;
    bool uniformImageSet, /*uniformCircularTexSet,*/ motionAmpLoaded;
    ofParameterGroup uniformsGroup;
    int numFFTChannels;
    int controllerNumber;
    
public:
    Effect();
    void setSharedUniforms(ofTexture diffuse, ofTexture vectorField, int screenWidth, int screenHeight) {
        shader.setUniformTexture( "diffuseTexture", diffuse, 0 );
        shader.setUniformTexture( "vectorField", vectorField, 2 );
        shader.setUniform2f( "diffuseTextureSize", diffuse.getWidth(), diffuse.getHeight() );
        shader.setUniform2f( "vectorFieldSize", vectorField.getWidth(), vectorField.getHeight() );
        shader.setUniform1f( "time", ofGetElapsedTimef() );
        shader.setUniform2f( "size", (float)screenWidth, (float)screenHeight);
        shader.setUniform2f( "resolution", (float)screenWidth, (float)screenHeight);
    }
    map<string, ofParameter<float>* > floatUniforms;
    vector< ofParameter<float>* > floatUniformsVector;
    map<string, ofParameter<bool>* > fftConnected;
    map<string, ofParameter<int>* > fftChannels;
    ofxPanel gui;
    
    string lastParamChanged;
    void updateFromFFT(vector<float> fft, float upperCut, float lowerCut);
    void updateFromFloat(float value, float upperCut, float lowerCut);
    void updateFromFloat(float value, int param);

    
    void setMotionAmp(MotionAmplifier* amp);
    void setUniformFlowField(ofTexture* texture);
    void setUniformImage(string name, ofImage* img);
    void setControllerNumber(int _controllerNumber) { controllerNumber = _controllerNumber; };
    int getControllerNumber() { return controllerNumber; };
    ofParameter<bool> * getActiveParameter() { return &active; };
    void setActiveParameter(bool _param) { active.set(_param); };
    void loadSettings();
    void setGuiPosition(int x, int y);
    float getGuiWidth();
    void setupGui(string name, int numFFTChannels);
    bool loadShader(string ShaderPath);
    void addUniformFloat(string name, string parameterName, float initialValue, float minValue, float maxValue);
    void addUniformBool(string name, string parameterName, bool initialValue);
    void addUniformVectorArray(string name, float* _vectorArray, int _size);
    void apply(ofFbo* fboIn, ofFbo* fboOut);
    void apply(ofFbo* fboIn, ofFbo* fboOut, ofMesh* mesh);
    void beginShader();
    void setUniqueUniforms();
    void endShader();
    ofShader* getShaderRef();
    ofParameterGroup* getParametersRef();
    void drawGui();
};