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
#include "ofxAubio.h"

class ofApp : public ofBaseApp{
    
	public:
		void setup();
		void update();
		void draw();
        void audioIn(float * input, int bufferSize, int nChannels);

		void keyPressed(int key);
    
        void onPartyModeChange(bool & b);
    
        ContentManager* contentManager;
        videoTiler* mosaic;
        vector<string> pressContent;
        vector<ofImage> pressImages;
        vector<string> partyContent;
        vector<ofImage> partyImages;
        int currentMovie;
    
        ofShader fade;
    
        vector<Effect*> effects;
    
        vector< pair<int, string> > connectedParams;

        ofxPanel gui;
        ofParameterGroup main;
        ofParameter<int> activeEffect;
        ofParameter<float> strength;
        ofParameter<float> learningRate;
    
        ofxPanel presets;
        ofParameter<int> presetNum;
        ofParameter<int> presetDuration;
        int timeOfLastSwap;
    
        ofxPanel fftCut;
        ofParameter<float> upperCut;
        ofParameter<float> lowerCut;
    
        ofParameter<bool> PartyOn; //Garth
        ofParameter<bool> camInput;
    
        ofVideoGrabber cam;
    
        ofVideoPlayer dieselHashtag;
    
        MotionAmplifier amplifier;
    
        vector<ofVec2f> voronoiSeedLocs;
        vector<ofVec2f> voronoiSeedVels;
        vector<ofVec2f> voronoiInitVels;
    
        bool drawGui;
        
        bool test;
    
        ofxCv::FlowFarneback flow;
    
        ofxFFTLive fft;
        ofxAubioOnset onset;
    
        ofFbo* activeBuffer;
        ofFbo* swapIn;
        ofFbo* swapOut;
    
        ofTrueTypeFont ttf;
        ofTrueTypeFont ttfSmall;
    
        ofImage currImg, finalOutput;
            
        ofFbo finalMix, motionWarp, mosaicDraw;
};
