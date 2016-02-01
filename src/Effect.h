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
//#include "CircularTexture.h"
#include "MotionAmplifier.h"

class Effect {
    
    ofShader shader;
    map<string, ofParameter<bool>* > boolUniforms;
    string uniformImageName;
    ofImage* uniformImage;
    ofTexture* vectorField;
//    string uniformCircularTexName;
//    CircularTexture* uniformCircularTex;
    MotionAmplifier* motionAmp;
    string uniformVectorArrayName;
    float* uniformVectorArray;
    int uniformVectorArraySize;
    bool uniformImageSet, /*uniformCircularTexSet,*/ uniformVectorArraySet, motionAmpLoaded;
    ofParameterGroup uniformsGroup;
    int numFFTChannels;
    
public:
    Effect();
    void setSharedUniforms(ofTexture diffuse, ofTexture vectorField, int screenWidth, int screenHeight) {
        shader.setUniformTexture( "diffuseTexture", diffuse, 0 );
        shader.setUniformTexture( "vectorField", vectorField, 2 );
        shader.setUniform2f( "vectorFieldSize", vectorField.getWidth(), vectorField.getHeight() );
        shader.setUniform1f( "time", ofGetElapsedTimef() );
        shader.setUniform2f( "size", (float)screenWidth, (float)screenHeight);
        shader.setUniform2f( "resolution", (float)screenWidth, (float)screenHeight);
        //shader.setUniform2f( "mouse", (float)(ofGetMouseX()/WIDTH), (float)(ofGetMouseY()/HEIGHT));
    }
    map<string, ofParameter<float>* > floatUniforms;
    map<string, ofParameter<bool>* > fftConnected;
    map<string, ofParameter<int>* > fftChannels;
    ofxPanel gui;
    
    void updateFromFFT(vector<float> fft, float upperCut, float lowerCut);
    
    void setMotionAmp(MotionAmplifier* amp);
    void setUniformFlowField(ofTexture* texture);
    void setUniformImage(string name, ofImage* img);
//    void setUniformCircularTex(string name, CircularTexture* tex);
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