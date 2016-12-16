#pragma once

//#define USING_FFT // uncomment this to use FFT instead of onset detection

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxCv.h"
#include "ofxGui.h"
#include "ofxBlackMagic.h"
#include "Effect.h"
#include "MotionAmplifier.h"
#include "videoTiler.h"
//#include "ofxFFTLive.h"
#include "ContentManager.h"
#include "ofxAubio.h"
#include "midiManager.h"
//#include "ofxSyphon.h"
#include "ofxNestedFileLoader.h"
#include "OSCBeatReciever.h"
#include "IPCamInput.h"

class ofApp : public ofBaseApp{
    
public:
    void setup();
    void update();
    void draw();
    void exit();
    void audioIn(float * input, int bufferSize, int nChannels);
    
    void keyPressed(int key);
    
    void onOverlayChanged(ofAbstractParameter & param);
    void onVideoBinChanged(ofAbstractParameter & param);
    
    
    ContentManager* contentManager;
    MidiManager* midiManager;
    //videoTiler* mosaic;
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
    
    ofxPanel fftCut;
    ofParameter<float> upperCut;
    ofParameter<float> lowerCut;
    
    ofParameter<bool> PartyOn; //Garth
    ofParameter<bool> playing;
    ofParameter<bool> camInput;
    ofParameter<bool> syphonOut;
    ofParameter<bool> flipInput;
    ofParameter<float> oscillationSpeed;
    ofParameter<bool> OSCOn;
    
    ofxPanel contentGui;
    ofParameterGroup overlayGroup;
    ofParameterGroup videoGroup;
    ofParameterGroup videoOverlayGroup;
    ofParameter<bool> videoOverlayOn;
    ofParameter<int> videoOverlayX;
    ofParameter<int> videoOverlayY;
    ofParameter<float> videoOverlayScale;
    map<string, string> overlays;
    map<string, vector<string>> videos;
    vector<string> overlayMovieNames;
    int overlayMovieIndex;
    float currentOverlayVideoDuration;
    float timeOfLastOverlayVideoSwap;
    
    ofVideoPlayer overlayVideoPlayer;
    
#ifdef USING_BLACKMAGIC
    ofxBlackMagic cam;
#elif defined USING_WEBCAM
    ofVideoGrabber cam;
#else
    IPCamInput cam;
#endif
    
    
    MotionAmplifier amplifier;
    
    vector<ofVec2f> voronoiSeedLocs;
    vector<ofVec2f> voronoiSeedVels;
    vector<ofVec2f> voronoiInitVels;
    bool drawGui;
    
    int voronoiNum;
    
    float oscillatorStep = 0.0;
    float onsetConfidence = 0.0;
    
    ofxCv::FlowFarneback flow;
    
    OSCBeatReciever reciever;
    
    ofxAubioOnset onset;
    
    ofFbo* activeBuffer;
    ofFbo* swapIn;
    ofFbo* swapOut;
    
    ofTrueTypeFont ttf;
    ofTrueTypeFont ttfSmall;
    
    ofImage currImg, finalOutput;
    ofImage overlayImage;
    
    ofFbo finalMix, motionWarp, mosaicDraw;
    
    //ofxSyphonServer texOutputToSyphon;
};
