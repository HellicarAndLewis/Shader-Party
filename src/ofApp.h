#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxCv.h"
#include "ofxGui.h"
#include "Effect.h"
#include "CircularTexture.h"
#include "MotionAmplifier.h"

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
        ofVideoPlayer* players[2];
        vector<string> movies;
        int currentMovie;
    
        vector<Effect*> effects;
    
        ofxPanel gui;
        ofParameterGroup main;
        ofParameter<float> amount;
        ofParameter<int> activeEffect;
        ofParameter<float> strength;
        ofParameter<float> learningRate;
    
        ofParameterGroup EffectsList;
        vector< ofParameter<bool>* > activeEffects;
        ofParameter<bool> badTvOn;
        ofParameter<bool> colorMapOn;
        ofParameter<bool> embossOn;
        ofParameter<bool> rgbShiftOn;
        ofParameter<bool> scanLinesOn;
        ofParameter<bool> sharpenOn;
    
        MotionAmplifier amplifier;
    
        ofxCv::FlowFarneback flow;
        //ofFloatImage vf;
    
        CircularTexture circleTex;
    
        ofFbo* swapIn;
        ofFbo* swapOut;
    
        ofTrueTypeFont ttf;
        ofTrueTypeFont ttfSmall;
    
        ofImage currImg;
            
        ofFbo initialDraw, motionWarp, shaderPass;
};
