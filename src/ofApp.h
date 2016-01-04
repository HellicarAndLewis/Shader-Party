#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxCv.h"
#include "ofxGui.h"
#include "Effect.h"
#include "MotionAmplifier.h"
#include "ofxAVFVideoPlayer.h"
#include "videoTiler.h"

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
        videoTiler* mosaic;
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
    
        ofParameter<bool> motionAmpOn;
        ofParameter<bool> voronoiOn;
        
        ofParameterGroup EffectsList;
        vector< ofParameter<bool>* > activeEffects;
        ofParameter<bool> endarkenOn;
        ofParameter<bool> badTvOn;
        ofParameter<bool> colorMapOn;
        ofParameter<bool> embossOn;
        ofParameter<bool> rgbShiftOn;
        ofParameter<bool> scanLinesOn;
        ofParameter<bool> sharpenOn;
        ofParameter<float> fadeAmnt;
    
        MotionAmplifier amplifier;
    
        vector<ofVec2f> voronoiSeedLocs;
        vector<ofVec2f> voronoiSeedVels;
        vector<ofVec2f> voronoiInitVels;

    
        ofxCv::FlowFarneback flow;
        //ofFloatImage vf;
    
        ofFbo* swapIn;
        ofFbo* swapOut;
    
        ofTrueTypeFont ttf;
        ofTrueTypeFont ttfSmall;
    
        ofImage currImg, finalOutput;
            
        ofFbo initialDraw, motionWarp, mosaicDraw, shaderPass;
};
