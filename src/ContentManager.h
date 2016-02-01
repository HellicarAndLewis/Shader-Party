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
#include "ofxCv.h"
#include "ofxOpenCv.h"
#include "ofxGui.h"
#include "Effect.h"
#include "videoTiler.h"
#include "Sparticles.h"

class ContentManager {
public:
    struct contentCreator {
        //Gui stuff
        ofxPanel gui;
        ofParameterGroup EffectsList;
        
        vector< ofParameter<bool>* > activeEffects;
        ofParameter<bool> voronoiOn;
        ofParameter<bool> badTvOn;
        ofParameter<bool> colorMapOn;
        ofParameter<bool> embossOn;
        ofParameter<bool> rgbShiftOn;
        ofParameter<bool> scanLinesOn;
        ofParameter<bool> sharpenOn;
        
        ofxPanel Presets;
        ofParameter<int> presetNum;
        ofParameter<bool> save;
        ofParameter<bool> apply;
        ofParameter<float> camDuration;
        
        ofxPanel Particles;
        ofParameter<bool> particlesOn;
        
        MotionAmplifier* amp;
        videoTiler* tiler;
        int numEffectsOn;
        
        ofImage presetImg;
        
        //buffer
        ofFbo buffer;
        ofFbo* swapIn;
        ofFbo* swapOut;
        
        //pointer to our list of effects
        vector<Effect*>* effects;
        
        void setupGui(string name) {
            gui.setup(name);
            EffectsList.setName("Active Effects");
            EffectsList.add(voronoiOn.set("Voronoi", false));
            EffectsList.add(badTvOn.set("Bad TV", false));
            EffectsList.add(colorMapOn.set("Color Map", false));
            EffectsList.add(embossOn.set("Emboss", false));
            EffectsList.add(rgbShiftOn.set("RGB Shift", false));
            EffectsList.add(scanLinesOn.set("Scan Lines", false));
            EffectsList.add(sharpenOn.set("Sharpen", false));
            
            activeEffects.push_back(&voronoiOn);
            activeEffects.push_back(&badTvOn);
            activeEffects.push_back(&colorMapOn);
            activeEffects.push_back(&embossOn);
            activeEffects.push_back(&rgbShiftOn);
            activeEffects.push_back(&scanLinesOn);
            activeEffects.push_back(&sharpenOn);
            gui.add(EffectsList);
            
            Presets.setName("Presets");
            
            //setup presetsGui
            Presets.setup("Presets", "settings/Presets.xml");
            ofDirectory presetsDir("presets/");
            presetsDir.listDir();
            Presets.add(presetNum.set("Preset", 0, 0, presetsDir.size()-1));
            Presets.add(save.set("Save", false));
            Presets.add(apply.set("Apply", false));
            Presets.add(camDuration.set("Preset Duration", 5, 2, 600));
            presetNum.addListener(this, &contentCreator::onPresetChange);
            save.addListener(this, &contentCreator::onSaveChange);
            apply.addListener(this, &contentCreator::onApplyChange);
            Presets.loadFromFile("settings/Presets.xml");
            
            Particles.setup("Particles", "settings/Particles.xml");
            Particles.add(particlesOn.set("On", false));
            Particles.loadFromFile("Settings/Particles.xml");
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
        void drawGui() {
            gui.draw();
            int x = gui.getPosition().x + gui.getWidth() + 20;
            int y = gui.getPosition().y;
            for(int i = 0; i < activeEffects.size(); i++) {
                if(activeEffects[i]->get()) {
                    (*effects)[i]->setGuiPosition(x, y);
                    (*effects)[i]->drawGui();
                    x += (*effects)[i]->getGuiWidth() + 10;
                    if(x - gui.getPosition().x > ofGetScreenWidth() - gui.getWidth()) {
                        x = gui.getPosition().x + gui.getWidth() + 20;
                        y += gui.getHeight() + 30;
                    }
                }
            }
            Presets.draw();
            presetImg.draw(gui.getPosition().x, gui.getHeight() + Presets.getHeight() + 20, 320, 180);
            Particles.draw();
        }
        void setEffects(vector<Effect*>* newEffects) {
            effects = newEffects;
        }
        void setTiler(videoTiler* newTiler) {
            tiler = newTiler;
        }
        void onPresetChange(int &e) {
            presetImg.loadImage("presets/" + ofToString(presetNum) + "/" + "img.png");
        }
        void onSaveChange(bool &b) {
            if(b) {
                gui.saveToFile("presets/" + ofToString(presetNum) + "/" + gui.getName() + ".xml");
                for(int i = 0; i < effects->size(); i++) {
                    (*effects)[i]->gui.saveToFile("presets/" + ofToString(presetNum) + "/" + (*effects)[i]->gui.getName()+".xml");
                }
                amp->gui.saveToFile("presets/" + ofToString(presetNum) + "/" + "MotionAmp" + ".xml");
                tiler->gui.saveToFile("presets/" + ofToString(presetNum) + "/" + "Tiler" + ".xml");
                Particles.saveToFile("presets/" + ofToString(presetNum) + "/" + "Particles.xml");
                save = false;
                if(numEffectsOn%2 == 1) {
                    ofImage saveImg;
                    saveImg.allocate(buffer.getWidth(), buffer.getHeight(), OF_IMAGE_COLOR);
                    swapOut->readToPixels(saveImg);
                    ofSaveImage(saveImg, "presets/" + ofToString(presetNum) + "/" + "img.png");
                } else {
                    ofImage saveImg;
                    saveImg.allocate(buffer.getWidth(), buffer.getHeight(), OF_IMAGE_COLOR);
                    swapIn->readToPixels(saveImg);
                    ofSaveImage(saveImg, "presets/" + ofToString(presetNum) + "/" + "img.png");
                }
            }
        }
        void onApplyChange(bool &b) {
            if(b) {
                gui.loadFromFile("presets/" + ofToString(presetNum) + "/" + gui.getName() + ".xml");
                for(int i = 0; i < effects->size(); i++) {
                    (*effects)[i]->gui.loadFromFile("presets/" + ofToString(presetNum) + "/" + (*effects)[i]->gui.getName()+".xml");
                }
                amp->gui.loadFromFile("presets/" + ofToString(presetNum) + "/" + "MotionAmp" + ".xml");
                tiler->gui.loadFromFile("presets/" + ofToString(presetNum) + "/" + "Tiler" + ".xml");
                Particles.loadFromFile("presets/" + ofToString(presetNum) + "/" + "Particles.xml");
                apply = false;
            }
        }
    };
    
    int numEffectsOn;
    
    vector<string> pressContentNames;
    vector<string> partyContentNames;
    
    vector<string>* currentEventContent;
    
    Sparticles particles;
    ofxCvContourFinder contourFinder;
    
    MotionAmplifier* amplifier;
    
    float timeSinceLastSwap;
    float currentDuration;
    bool currentContentIsVideo = true;
    int nextContentIndex = 0;
    
    ofVideoPlayer player;
    ofImage image;
    ofVideoGrabber* camera;
    
    contentCreator frame;
    
    ofxCvColorImage			scaledImage;
    ofxCvGrayscaleImage 	grayImage;
    ofxCvGrayscaleImage 	grayBg;
    ofxCvGrayscaleImage 	grayDiff;
    
    ofFbo* currentBuffer;
    ofFbo* swapIn;
    ofFbo* swapOut;
    
    int bufferWidth, bufferHeight;
    
    ContentManager() {};
    
    void setup(string pressContentPath, string partyContentPath, int width, int height) {
        //Load all the movies and images
        ofDirectory pressDir(pressContentPath);
        pressDir.listDir();
        for(int i = 0; i < pressDir.size(); i++) {
            ofDirectory internalDir(pressDir.getPath(i));
            if(internalDir.isDirectory()) {
                internalDir.allowExt("mp4");
                internalDir.allowExt("mov");
                internalDir.allowExt("jpg");
                internalDir.allowExt("png");
                internalDir.allowExt("jpeg");
                internalDir.listDir();
                for(int j = 0; j < internalDir.size(); j++) {
                    pressContentNames.push_back(internalDir.getPath(j));
                }
            }
        }
        
        ofDirectory partyDir(partyContentPath);
        partyDir.listDir();
        for(int i = 0; i < partyDir.size(); i++) {
            ofDirectory internalDir(partyDir.getPath(i));
            if(internalDir.isDirectory()) {
                internalDir.allowExt("mp4");
                internalDir.allowExt("mov");
                internalDir.allowExt("jpg");
                internalDir.allowExt("png");
                internalDir.allowExt("jpeg");
                internalDir.listDir();
                for(int j = 0; j < internalDir.size(); j++) {
                    partyContentNames.push_back(internalDir.getPath(j));
                }
            }
        }
        
        player.setPixelFormat(OF_PIXELS_RGB);
        player.setLoopState(OF_LOOP_NONE);
        player.setPixelFormat(OF_PIXELS_RGB);
        
        currentEventContent = &pressContentNames;
        
        swapContent(currentEventContent);
        
        bufferWidth = width;
        bufferHeight = height;
        frame.buffer.allocate(width, height);
        
        scaledImage.allocate(width, height);
        grayImage.allocate(width, height);
        grayBg.allocate(width, height);
        grayDiff.allocate(width, height);
        
        currentBuffer = &frame.buffer;
        
        particles.setup();
        
    }
    
    void setSwapBuffers( ofFbo* inBuff, ofFbo* outBuff) {
        swapIn = inBuff;
        swapOut = outBuff;
        frame.swapIn = inBuff;
        frame.swapOut = outBuff;
    }
    
    void setCamera(ofVideoGrabber* cam) {
        camera = cam;
    }
    
    void setTiler(videoTiler* newTiler) {
        frame.tiler = newTiler;
    }
    
    void setAmplifier(MotionAmplifier* amp) {
        amplifier = amp;
        frame.amp = amp;
    }
    
    void loadNewVideo(string vidPath) {
        player.loadMovie(vidPath);
    };
    void loadNewImage(string imgPath) {
        image.loadImage(imgPath);
    };
    
    void swapContent(vector<string>* content) {
        string nextContent = (*content)[nextContentIndex];
        nextContentIndex++;
        nextContentIndex%=content->size();
        vector<string> splitString = ofSplitString(nextContent, ".");
        if(splitString[1] == "mov" || splitString[1] == "mp4") {
            currentContentIsVideo = true;
            player.stop();
            loadNewVideo(nextContent);
            currentDuration = player.getDuration();
            player.setPaused(false);
            player.play();
            player.setVolume(0);
        }
        if(splitString[1] == "png" || splitString[1] == "jpeg" || splitString[1] == "jpg") {
            currentContentIsVideo = false;
            loadNewImage(nextContent);
            currentDuration = 5.0;
        }
        timeSinceLastSwap = ofGetElapsedTimef();
    }
    
    void updateCamera() {
        camera->update();
        if(ofGetElapsedTimef() - timeSinceLastSwap > frame.camDuration) {
            frame.presetNum++;
            frame.presetNum %= frame.presetNum.getMax();
            frame.apply = true;
            timeSinceLastSwap = ofGetElapsedTimef();
        }
        if(frame.particlesOn) {
            scaledImage.setFromPixels(camera->getPixels(), camera->getWidth(), camera->getHeight());
            
            grayImage = scaledImage;
            
            // take the abs value of the difference between background and incoming and then threshold:
            grayDiff.absDiff(grayBg, grayImage);
            grayDiff.threshold(80);
            
            // find contours which are between the size of 20 pixels and 1/3 the w*h pixels.
            // also, find holes is set to true so we will get interior contours as well....
            contourFinder.findContours(grayDiff, 20, (camera->getWidth()*camera->getHeight())/3, 10, true);	// find holes
            
            // now just stick some particles on the contour and emit them randomly
            for(int i = 0; i < contourFinder.nBlobs; i++) {
                int step = 10;//contourFinder.blobs[i].pts.size()/10;
                for(int j = 0; j < contourFinder.blobs[i].pts.size(); j+=step) {
                    particles.spawn(
                                    contourFinder.blobs[i].pts[j].x,//*WIDTH/VISION_WIDTH,
                                    contourFinder.blobs[i].pts[j].y,//*HEIGHT/VISION_HEIGHT,
                                    ofRandom(-5, 5), ofRandom(-5, 5));
                }
            }
            
            grayBg.convertToRange(0, 255.f*(1.f - 0.3));
            grayImage.convertToRange(0, 255.f*(0.3));
            
            // grayBg = grayBg * 0.9 + grayImage * 0.1
            grayBg += grayImage;
            particles.update();
            numEffectsOn = 0;
            for(int i = 0; i < frame.EffectsList.size(); i++) {
                if(frame.EffectsList.getBool(i)) {
                    numEffectsOn++;
                }
            }
            frame.numEffectsOn = numEffectsOn;

        }
    }
    
    void updateContent() {
        player.update();
        if(ofGetElapsedTimef() - timeSinceLastSwap > currentDuration) {
            swapContent(currentEventContent);
        }
        if(currentContentIsVideo) {
            if(amplifier->Enabled) amplifier->update(player);
        } else {
            if(amplifier->Enabled) amplifier->update(image);
        }
        numEffectsOn = 0;
        for(int i = 0; i < frame.EffectsList.size(); i++) {
            if(frame.EffectsList.getBool(i)) {
                numEffectsOn++;
            }
        }
        frame.numEffectsOn = numEffectsOn;
    }
    
    void update(bool b) {
        if(b) {
            updateCamera();
            
        } else {
            updateContent();
        }
    }
    
    void applyEffects() {
        frame.applyEffects(swapIn, swapOut);
    }
    
    void drawGuis() {
        frame.drawGui();
        
    }
    
    void startParty() {
        currentEventContent = &partyContentNames;
    }
    
    void startPress() {
        currentEventContent = &pressContentNames;
    }
    
    void drawCamera() {
        amplifier->update(*camera);
        frame.buffer.begin();
        glBlendFuncSeparate(GL_ONE, GL_SRC_COLOR, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
        ofClear(0);
        ofScale(-1, 1);
        ofTranslate(-bufferWidth, 0);
        if(amplifier->Enabled) amplifier->draw(*camera);
        else camera->draw(0, 0, bufferWidth, bufferHeight);
        if(frame.particlesOn) particles.draw();
        frame.buffer.end();
    }
    
    void drawContent() {
        frame.buffer.begin();
        ofClear(0);
        if(currentContentIsVideo) {
            if(amplifier->Enabled) {
                amplifier->draw(player);//player.draw(0, 0, bufferWidth, bufferHeight);
            } else {
                player.draw(0, 0, bufferWidth, bufferHeight);
            }
        } else {
            if(amplifier->Enabled) {
                amplifier->draw(image);//player.draw(0, 0, bufferWidth, bufferHeight);
            } else {
                image.draw(0, 0, bufferWidth, bufferHeight);
            }
        }
        frame.buffer.end();
    }
    
    void drawBuffers(bool b) {
        if(b) {
            drawCamera();
        } else {
            drawContent();
        }
    }
    
    ofTexture* getCurrentBuffer() {
        return &currentBuffer->getTextureReference();
    };
    
    ofTexture* getFrame1Buffer() {
        return &frame.buffer.getTextureReference();
    }
    
    ofTexture* getFrame2Buffer() {
        return &frame.buffer.getTextureReference();
    }
    
    void play() {
        player.setPaused(false);
        player.play();
    };
    
    void changeMode(bool b) {
        if(b) {
            nextContentIndex = 0;
            startParty();
        } else {
            nextContentIndex = 0;
            startPress();
        }
    }

};

#endif /* defined(__ShaderParty__ContentManager__) */
