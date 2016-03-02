#pragma once

//#define USING_FFT // uncomment this to use FFT instead of onset detection
#define USING_BLACKMAGIC

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxCv.h"
#include "ofxGui.h"
#include "ofxBlackMagic.h"
#include "Effect.h"
#include "MotionAmplifier.h"
#include "videoTiler.h"
#include "ofxFFTLive.h"
#include "ContentManager.h"
#include "ofxAubio.h"
#include "midiManager.h"
#include "ofxSyphon.h"

class ofApp : public ofBaseApp{
    
	public:
		void setup();
		void update();
		void draw();
        void exit();
        void audioIn(float * input, int bufferSize, int nChannels);

		void keyPressed(int key);
    
        void onPartyModeChange(bool & b);
    
        ContentManager* contentManager;
        MidiManager* midiManager;
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
        ofParameter<bool> syphonOut;
        ofParameter<bool> flipInput;

#ifdef USING_BLACKMAGIC
    ofxBlackMagic cam;
#else
    ofVideoGrabber cam;
#endif
    
        ofVideoPlayer dieselHashtag;
    
        MotionAmplifier amplifier;
    
        vector<ofVec2f> voronoiSeedLocs;
        vector<ofVec2f> voronoiSeedVels;
        vector<ofVec2f> voronoiInitVels;
    
        vector<ofVec2f> kaleidoscopeSeedLocs;
        vector<ofVec2f> kaleidoscopeSeedNormals;
    
        bool drawGui;
    
        int voronoiNum;
    
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
    
        ofxSyphonServer texOutputToSyphon;
};
