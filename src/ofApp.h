#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxCv.h"
#include "ofxGui.h"
#include "Effect.h"
#include "MotionAmplifier.h"
#include "ofxAVFVideoPlayer.h"

class ofApp : public ofBaseApp{
    
	public:
		void setup();
		void update();
		void draw();
        void swapPlayer();
        void swapFbos();

		void keyPressed(int key);
        
        float lastTime;
        int currentPlayer;
        ofxAVFVideoPlayer* players[2];
        vector<string> movies;
        int currentMovie;
    
        ofShader fade;
    
        vector<Effect*> effects;
    
        ofxPanel gui;
        ofParameterGroup main;
        ofParameter<float> amount;
        ofParameter<int> activeEffect;
        ofParameter<float> strength;
        ofParameter<float> learningRate;
        ofParameter<bool> motionAmp;
    
        ofxPanel videoLoader;
    
        ofParameterGroup EffectsList;
        vector< ofParameter<bool>* > activeEffects;
        ofParameter<bool> badTvOn;
        ofParameter<bool> colorMapOn;
        ofParameter<bool> embossOn;
        ofParameter<bool> rgbShiftOn;
        ofParameter<bool> scanLinesOn;
        ofParameter<bool> sharpenOn;
        ofParameter<float> fadeAmnt;
    
        MotionAmplifier amplifier;
    
        ofxCv::FlowFarneback flow;
        //ofFloatImage vf;
    
        ofFbo* swapIn;
        ofFbo* swapOut;
    
        ofTrueTypeFont ttf;
        ofTrueTypeFont ttfSmall;
    
        ofImage currImg;
            
        ofFbo initialDraw, motionWarp, shaderPass;
};
