//
//  ContentManager.h
//  ShaderParty
//
//  Created by James Bentley on 1/5/16.
//
//

#ifndef __ShaderParty__ContentManager__
#define __ShaderParty__ContentManager__

//#define USING_BLACKMAGIC
//#define USING_WEBCAM

#define VID_WIDTH 1280
#define VID_HEIGHT 720

#include "ofMain.h"
#include "ofxCv.h"
#include "ofxOpenCv.h"
#include "ofxGui.h"
#include "ofxBlackMagic.h"
#include "Effect.h"
#include "videoTiler.h"
#include "Sparticles.h"
#include "IPCamInput.h"

class ContentManager {
public:
    struct contentCreator {
        //Gui stuff
        ofxPanel gui;
        ofParameterGroup EffectsList;
        
        vector< ofParameter<bool>* > activeEffects;
        
        ofxPanel Presets;
        ofParameter<int> presetNum;
        ofParameter<bool> save;
        ofParameter<bool> apply;
        ofParameter<float> presetDuration;
        ofParameter<bool> presetsAutomated;
        
        ofxPanel Particles;
        ofParameter<bool> particlesOn;
        
        MotionAmplifier* amp;
        videoTiler* tiler;
        int numEffectsOn;
        
        ofImage presetImg;
        
        ofMesh mesh;
        
        //buffer
        ofFbo buffer;
        ofFbo* swapIn;
        ofFbo* swapOut;
        
        //pointer to our list of effects
        vector<Effect*>* effects;
        // an internal list of our active effects in order
        vector<Effect*> activeEffectsInOrder;
        
        void setupGui(string name);
        
        void applyEffectsInFixedOrder(ofFbo* swapIn, ofFbo* swapOut);
        void applyEffectsInOrderOfActivation(ofFbo* swapIn, ofFbo* swapOut);
        void applyEffects(ofFbo* swapIn, ofFbo* swapOut);
        void drawGui();
        
        void setEffects(vector<Effect*>* newEffects) {effects = newEffects;};
        void setTiler(videoTiler* newTiler) {tiler = newTiler;};
        
        void onPresetChange(int &e);
        void onSaveChange(bool &b);
        void onApplyChange(bool &b);
        void onActiveEffectChanged(ofAbstractParameter &p);
    };
    
    int numEffectsOn;
    
    map<string, vector<string>>* contentNamesLibrary;
    
    vector<string>* currentContentNames;
    
    Sparticles particles;
    ofxCvContourFinder contourFinder;
    
    MotionAmplifier* amplifier;
    
    float timeSinceLastSwap;
    float currentDuration;
    bool currentContentIsVideo = true;
    int nextContentIndex = 0;
    
    ofVideoPlayer player;
    ofImage image;
    
#ifdef USING_BLACKMAGIC
    ofxBlackMagic* camera;
#elif defined USING_WEBCAM
    ofVideoGrabber* camera;
#else
    IPCamInput* camera;
#endif
    
    contentCreator frame;
    
    ofxCvColorImage			scaledImage;
    ofxCvGrayscaleImage 	grayImage;
    ofxCvGrayscaleImage 	grayBg;
    ofxCvGrayscaleImage 	grayDiff;
    
    ofFbo* currentBuffer;
    ofFbo* swapIn;
    ofFbo* swapOut;
    
    ofParameter<bool>* flipInput;
    
    int bufferWidth, bufferHeight;
    
    ContentManager() {};
    
    void setup(int width, int height);
    
    void setSwapBuffers( ofFbo* inBuff, ofFbo* outBuff);
    
    void setFlipInput(ofParameter<bool>* _flipInput) { flipInput = _flipInput; };
    
#ifdef USING_BLACKMAGIC
    void setCamera(ofxBlackMagic* cam) { camera = cam;};
#elif defined USING_WEBCAM
    void setCamera(ofVideoGrabber* cam) { camera = cam;};
#else
    void setCamera(IPCamInput* cam) { camera = cam;};
#endif
    
    
    void setTiler(videoTiler* newTiler) {frame.tiler = newTiler;};
    
    void setContentNamesLibrary(map<string, vector<string>>* _contentNamesLibrary) {
        contentNamesLibrary = _contentNamesLibrary;
        nextContentIndex = 0;
        currentContentNames = &(contentNamesLibrary->begin()->second);
        swapContent(currentContentNames);
    };
    
    void setAmplifier(MotionAmplifier* amp) {
        amplifier = amp;
        frame.amp = amp;
    };
    
    void loadNewVideo(string vidPath) {
        player.load(vidPath);
    };
    void loadNewImage(string imgPath) {
        image.load(imgPath);
    };
    
    void swapContent(vector<string>* content);
    
    void updateCamera();
    
    void updateContent();
    
    void update(bool b) {
        if(frame.presetsAutomated.get()) {
            if(ofGetElapsedTimef() - timeSinceLastSwap > frame.presetDuration) {
                frame.presetNum++;
                frame.presetNum %= frame.presetNum.getMax();
                frame.apply = true;
                timeSinceLastSwap = ofGetElapsedTimef();
            }
        }
        if(b) {
            updateCamera();
            
        } else {
            updateContent();
        }
    };
    
    void applyEffects() {
        frame.applyEffects(swapIn, swapOut);
    };
    
    void drawGuis() {
        frame.drawGui();
    };
    
    void drawCamera();
    
    void drawContent();
    
    void drawBuffers(bool b) {
        if(b) {
            drawCamera();
        } else {
            drawContent();
        }
    }
    
    ofTexture* getCurrentBuffer() {
        return &currentBuffer->getTexture();
    };
    
    ofTexture* getFrame1Buffer() {
        return &frame.buffer.getTexture();
    }
    
    ofTexture* getFrame2Buffer() {
        return &frame.buffer.getTexture();
    }
    
    void play() {
        player.setPaused(false);
        //player.setVolume(1.0);
        player.play();
    };
    
    void changeContent(string newContentBin) {
        currentContentNames = &(*contentNamesLibrary)[newContentBin];
        nextContentIndex = 0;
        swapContent(currentContentNames);
    }
    
};

#endif /* defined(__ShaderParty__ContentManager__) */
