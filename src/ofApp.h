#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxCv.h"
#include "ofxGui.h"
#include "Effect.h"
#include "MotionAmplifier.h"
#include "videoTiler.h"
#include "ofxFFTLive.h"
#include "ContentManager.h"

class ofApp : public ofBaseApp{
    
	public:
		void setup();
		void update();
		void draw();
        void swapPlayer();
        void swapFbos(int i);

		void keyPressed(int key);
    
        ContentManager* contentManager;
        videoTiler* mosaic;
        vector<string> pressContent;
        vector<ofImage> pressImages;
        vector<string> partyContent;
        vector<ofImage> partyImages;
        int currentMovie;
    
        ofShader fade;
    
        vector<Effect*> effects[2];
    
        ofxPanel gui;
        ofParameterGroup main;
        ofParameter<int> activeEffect;
        ofParameter<float> strength;
        ofParameter<float> learningRate;
    
        ofxPanel presets[2];
        ofParameter<int> presetNum[2];
        
        ofParameterGroup EffectsList;

        ofParameter<float> fadeAmnt;
        ofParameter<float> fftDamping;
        ofParameter<float> fftAttraction;
    
        MotionAmplifier amplifier;
    
        vector<ofVec2f> voronoiSeedLocs;
        vector<ofVec2f> voronoiSeedVels;
        vector<ofVec2f> voronoiInitVels;
    
    bool test;
    
        ofxCv::FlowFarneback flow;
    
        ofxFFTLive fft;
        //ofFloatImage vf;
    
        ofFbo* swapIn[2];
        ofFbo* swapOut[2];
    
        ofTrueTypeFont ttf;
        ofTrueTypeFont ttfSmall;
    
        ofImage currImg, finalOutput;
            
        ofFbo finalMix, motionWarp[2], mosaicDraw[2];
};
