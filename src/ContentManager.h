//
//  ContentManager.h
//  ShaderParty
//
//  Created by James Bentley on 1/5/16.
//
//

#ifndef __ShaderParty__ContentManager__
#define __ShaderParty__ContentManager__

#include "ofMain.h"
//#include ""
#include "ofxGui.h"
#include "Effect.h"

class ContentManager {
public:
    struct contentCreator {
        //Gui stuff
        ofxPanel gui;
        ofParameterGroup EffectsList;
        
        vector< ofParameter<bool>* > activeEffects;
        ofParameter<bool> motionAmpOn;
        ofParameter<bool> voronoiOn;
        ofParameter<bool> endarkenOn;
        ofParameter<bool> badTvOn;
        ofParameter<bool> colorMapOn;
        ofParameter<bool> embossOn;
        ofParameter<bool> rgbShiftOn;
        ofParameter<bool> scanLinesOn;
        ofParameter<bool> sharpenOn;
        ofxPanel Presets;
        ofParameter<int> presetNum;
        ofParameter<bool> save;
        
        //buffer
        ofFbo buffer;
        
        //pointer to our list of effects
        vector<Effect*>* effects;
        
        void setupGui(string name) {
            gui.setup(name);
            EffectsList.setName("Active Effects");
            EffectsList.add(voronoiOn.set("Voronoi", false));
            EffectsList.add(motionAmpOn.set("Motion Amp", false));
            EffectsList.add(badTvOn.set("Bad TV", false));
            EffectsList.add(colorMapOn.set("Color Map", false));
            EffectsList.add(embossOn.set("Emboss", false));
            EffectsList.add(rgbShiftOn.set("RGB Shift", false));
            EffectsList.add(scanLinesOn.set("Scan Lines", false));
            EffectsList.add(sharpenOn.set("Sharpen", false));
            EffectsList.add(endarkenOn.set("Endarken", false));
            
            activeEffects.push_back(&voronoiOn);
            activeEffects.push_back(&badTvOn);
            activeEffects.push_back(&colorMapOn);
            activeEffects.push_back(&embossOn);
            activeEffects.push_back(&rgbShiftOn);
            activeEffects.push_back(&scanLinesOn);
            activeEffects.push_back(&sharpenOn);
            activeEffects.push_back(&endarkenOn);
            gui.add(EffectsList);
            
            Presets.setName("Presets");
            
            //setup presetsGui
            Presets.setup("Presets");
            Presets.add(presetNum.set("Preset", 0, 0, 20));
            Presets.add(save.set("Save", false));
            presetNum.addListener(this, &contentCreator::onPresetChange);
            save.addListener(this, &contentCreator::onSaveChange);
        }
        void applyEffects(ofFbo* swapIn, ofFbo* swapOut) {
            for(int j = 0; j < activeEffects.size(); j++) {
                if(activeEffects[j]->get()) {
                    (*effects)[j]->apply(swapIn, swapOut);
                    ofFbo* temp;
                    temp = swapIn;
                    swapIn = swapOut;
                    swapOut = temp;
                }
            }
        }
        void drawGui(int guiX, int guiY) {
            gui.setPosition(guiX, guiY);
            gui.draw();
            int x = guiX + gui.getWidth() + 20;
            int y = guiY;
            //        if(motionAmpOn) {
            //            amplifier.setGuiPosition(x, 10);
            //            amplifier.drawGui();
            //            x += amplifier.getGuiWidth() + 10;
            //        }
            for(int i = 0; i < activeEffects.size(); i++) {
                if(activeEffects[i]->get()) {
                    (*effects)[i]->setGuiPosition(x, 10);
                    (*effects)[i]->drawGui();
                    x += (*effects)[i]->getGuiWidth() + 10;
                }
            }
            Presets.setPosition(guiX, gui.getHeight());
            Presets.draw();
        }
        void setEffects(vector<Effect*>* newEffects) {
            effects = newEffects;
        }
        void onPresetChange(int &e) {
            gui.loadFromFile("presets/" + ofToString(e) + "/" + gui.getName() + ".xml");
            for(int i = 0; i < effects->size(); i++) {
                (*effects)[i]->gui.loadFromFile("presets/" + ofToString(e) + "/" + (*effects)[i]->gui.getName()+".xml");
            }
        }
        void onSaveChange(bool &b) {
            gui.saveToFile("presets/" + ofToString(presetNum) + "/" + gui.getName() + ".xml");
            for(int i = 0; i < effects->size(); i++) {
                (*effects)[i]->gui.saveToFile("presets/" + ofToString(presetNum) + "/" + (*effects)[i]->gui.getName()+".xml");
            }
            save = false;
        }
    };
    
    vector<string> pressVideoNames;
    vector<string> partyVideoNames;
    vector<string> pressImageNames;
    vector<string> partyImageNames;
    
    vector<string>* currentEventVideos;
    vector<string>* currentEventImages;
    
    float timeSinceLastSwap;
    float currentDuration;
    bool currentContentIsVideo = true;
    int nextImageIndex = 0;
    int nextVideoIndex = 0;
    
    ofVideoPlayer player;
    ofImage image;
    
    contentCreator frames[2];
    
    ofFbo* currentBuffer;
    
    int bufferWidth, bufferHeight;
    
    ContentManager() {
        
    };
    void setup(string pressContentPath, string partyContentPath, int width, int height) {
        //Load all the movies and images
        ofDirectory pressMovieDir(pressContentPath);
        pressMovieDir.allowExt("mp4");
        pressMovieDir.allowExt("mov");
        pressMovieDir.listDir();
        for(int i=0; i < pressMovieDir.numFiles(); i++) {
            string path = pressMovieDir.getPath(i);
            pressVideoNames.push_back(path);
        }
        ofDirectory pressImageDir(pressContentPath);
        pressImageDir.allowExt("jpg");
        pressImageDir.allowExt("jpeg");
        pressImageDir.allowExt("png");
        pressImageDir.listDir();
        for(int i=0; i < pressImageDir.numFiles(); i++) {
            string path = pressImageDir.getPath(i);
            pressImageNames.push_back(path);
        }
        
        ofDirectory partyMovieDir(pressContentPath);
        partyMovieDir.allowExt("mp4");
        partyMovieDir.allowExt("mov");
        partyMovieDir.listDir();
        partyMovieDir.listDir();
        for(int i=0; i < partyMovieDir.numFiles(); i++) {
            string path = partyMovieDir.getPath(i);
            partyVideoNames.push_back(path);
        }
        ofDirectory partyImageDir(pressContentPath);
        partyImageDir.allowExt("jpg");
        partyImageDir.allowExt("jpeg");
        partyImageDir.allowExt("png");
        partyImageDir.listDir();
        for(int i=0; i < partyImageDir.numFiles(); i++) {
            string path = partyImageDir.getPath(i);
            partyImageNames.push_back(path);
        }
        
        currentEventVideos = &pressVideoNames;
        currentEventImages = &pressImageNames;
        
        
        player.setPixelFormat(OF_PIXELS_RGB);
        player.setLoopState(OF_LOOP_NONE);
        player.setPixelFormat(OF_PIXELS_RGB);
        
        if(pressVideoNames.size()) {
            loadNewVideo((*currentEventVideos)[0]);
        } else {
            cout<<"No Press Videos!"<<endl;
        }
        
        if(pressImageNames.size()) {
            loadNewImage((*currentEventImages)[0]);
        } else {
            cout<<"No Press Images!"<<endl;
        }
        
        bufferWidth = width;
        bufferHeight = height;
        frames[0].buffer.allocate(width, height);
        frames[1].buffer.allocate(width, height);

        
        player.setVolume(0);
        currentContentIsVideo = true;
        nextImageIndex = 0;
        nextVideoIndex = 0;
        currentDuration = player.getDuration();
        timeSinceLastSwap = 0;
        
        currentBuffer = &frames[0].buffer;
    }
    
    void loadNewVideo(string vidPath) {
        player.loadMovie(vidPath);
    };
    void loadNewImage(string imgPath) {
        image.loadImage(imgPath);
    };
    
    void swapContent(vector<string>* vids, vector<string>* imgs) {
        if(currentContentIsVideo) {
            player.stop();
            loadNewVideo((*vids)[nextVideoIndex]);
            nextVideoIndex++;
            nextVideoIndex %= vids->size();
            currentContentIsVideo = false;
            currentDuration = 15.0;
        } else {
            loadNewImage((*imgs)[nextImageIndex]);
            nextImageIndex++;
            nextImageIndex %= imgs->size();
            player.setPaused(false);
            player.play();
            player.setVolume(0);
            currentDuration = player.getDuration();
            currentContentIsVideo = true;
        }
        timeSinceLastSwap = ofGetElapsedTimef();
    }
    
    void update() {
        for(int i = 0; i < 2; i++) {
            player.update();
            if(ofGetElapsedTimef() - timeSinceLastSwap > currentDuration) {
                float currentTime = ofGetElapsedTimef();
                float lastTime = timeSinceLastSwap;
                float duration = currentDuration;
                swapContent(currentEventVideos, currentEventImages);
            }
        }
    }
    
    void applyEffects(ofFbo* swapIn1, ofFbo* swapOut1, ofFbo* swapIn2, ofFbo* swapOut2) {
        frames[0].applyEffects(swapIn1, swapOut1);
        frames[1].applyEffects(swapIn2, swapOut2);

    }
    void drawGuis(int x1, int y1, int x2, int y2) {
        frames[0].drawGui(x1, y1);
        frames[1].drawGui(x2, y2);
    }
    
    void startParty() {
        currentEventImages = &partyImageNames;
        currentEventVideos = &partyVideoNames;
    }
    
    void startPress() {
        currentEventImages = &pressImageNames;
        currentEventVideos = &pressVideoNames;
    }
    
    void drawBuffers() {
        frames[0].buffer.begin();
        ofClear(0);
        if(currentContentIsVideo) player.draw(0, 0, bufferWidth, bufferHeight);
        else image.draw(0, 0, bufferWidth, bufferHeight);
        frames[0].buffer.end();
        
        frames[1].buffer.begin();
        ofClear(0);
        if(currentContentIsVideo) player.draw(0, 0, bufferWidth, bufferHeight);
        else image.draw(0, 0, bufferWidth, bufferHeight);
        frames[1].buffer.end();
        
    }
    
    ofTexture* getCurrentBuffer() {
        return &currentBuffer->getTextureReference();
    };
    
    ofTexture* getFrame1Buffer() {
        return &frames[0].buffer.getTextureReference();
    }
    
    ofTexture* getFrame2Buffer() {
        return &frames[1].buffer.getTextureReference();
    }
    
    void play() {
        player.setPaused(false);
        player.play();
    };

};

#endif /* defined(__ShaderParty__ContentManager__) */
