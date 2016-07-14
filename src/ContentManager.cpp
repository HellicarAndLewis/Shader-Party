//
//  ContentManager.cpp
//  ShaderParty
//
//  Created by James Bentley on 1/5/16.
//
//

#include "ContentManager.h"

// CONTENT CREATOR STUFF

void ContentManager::contentCreator::setupGui(string name) {
    gui.setup(name, "settings/contentSettings.xml");
    EffectsList.setName("Active Effects");
    for(int i = 0; i < effects->size(); i++) {
        EffectsList.add(*(*effects)[i]->getActiveParameter());
        activeEffects.push_back((*effects)[i]->getActiveParameter());
    }
    
    activeEffectsInOrder.clear();
    gui.add(EffectsList);
    
    gui.loadFromFile("settings/contentSettings.xml");
    
    ofAddListener(EffectsList.parameterChangedE(), this, &contentCreator::onActiveEffectChanged);
    
    Presets.setName("Presets");
    
    //setup presetsGui
    Presets.setup("Presets", "settings/Presets.xml");
    ofDirectory presetsDir("presets/");
    presetsDir.listDir();
    Presets.add(presetNum.set("Preset", 0, 0, presetsDir.size()-1));
    Presets.add(save.set("Save", false));
    Presets.add(apply.set("Apply", false));
    Presets.add(presetDuration.set("Preset Duration", 5, 2, 600));
    Presets.add(presetsAutomated.set("Auto Run", false));
    presetNum.addListener(this, &contentCreator::onPresetChange);
    save.addListener(this, &contentCreator::onSaveChange);
    apply.addListener(this, &contentCreator::onApplyChange);
    Presets.loadFromFile("settings/Presets.xml");
    
    Particles.setup("Particles", "settings/Particles.xml");
    Particles.add(particlesOn.set("On", false));
    Particles.loadFromFile("Settings/Particles.xml");
}

void ContentManager::contentCreator::applyEffects(ofFbo* swapIn, ofFbo* swapOut) {
    applyEffectsInFixedOrder(swapIn, swapOut);
}

void ContentManager::contentCreator::applyEffectsInFixedOrder(ofFbo* swapIn, ofFbo* swapOut) {
    for(int i = 0; i < activeEffects.size(); i++) {
        if(activeEffects[i]->get()) {
            (*effects)[i]->apply(swapIn, swapOut, &mesh);
            ofFbo* temp;
            temp = swapIn;
            swapIn = swapOut;
            swapOut = temp;
        }
    }
}

void ContentManager::contentCreator::applyEffectsInOrderOfActivation(ofFbo* swapIn, ofFbo* swapOut) {
    for(int i = 0; i < activeEffectsInOrder.size(); i++) {
        activeEffectsInOrder[i]->apply(swapIn, swapOut, &mesh);
        ofFbo* temp;
        temp = swapIn;
        swapIn = swapOut;
        swapOut = temp;
    }
}

void ContentManager::contentCreator::drawGui() {
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
    presetImg.draw(gui.getPosition().x, gui.getHeight() + Presets.getHeight() + 30, Presets.getWidth(), Presets.getWidth() * 720 / 1280);
    Particles.draw();
}

void ContentManager::contentCreator::onPresetChange(int &e) {
    presetImg.load("presets/" + ofToString(presetNum) + "/" + "img.png");
};

void ContentManager::contentCreator::onSaveChange(bool &b) {
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

void ContentManager::contentCreator::onApplyChange(bool &b) {
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

void ContentManager::contentCreator::onActiveEffectChanged(ofAbstractParameter &p) {
    string name = p.getName();
    for(auto it = activeEffectsInOrder.begin(); it != activeEffectsInOrder.end(); it++) {
        if((*it)->gui.getName() == name) {
            activeEffectsInOrder.erase(it);
            return;
        }
    }
    for(int i = 0; i < effects->size(); i++) {
        if((*effects)[i]->gui.getName() == name) {
            activeEffectsInOrder.push_back((*effects)[i]);
        }
    }
}

//CONTENT MANAGER METHODS

void ContentManager::setup(int width, int height) {
    player.setLoopState(OF_LOOP_NONE);
    player.setPixelFormat(OF_PIXELS_RGB);
    
    bufferWidth = width;
    bufferHeight = height;
    frame.buffer.allocate(width, height);
    
    scaledImage.allocate(width, height);
    grayImage.allocate(width, height);
    grayBg.allocate(width, height);
    grayDiff.allocate(width, height);
    
    currentBuffer = &frame.buffer;
    
    float rescale = .25;
    float scaleFactor = 1. / 10; // could dynamically calculate this from flow3
    
    //setup the mesh grid
    
    frame.mesh.setMode(OF_PRIMITIVE_TRIANGLES);
    int stepSize = 2;
    float xSteps = 1+((rescale * width) / stepSize);
    float ySteps = 1+((rescale * height) / stepSize);
    for(int y = 0; y < ySteps; y++) {
        for(int x = 0; x < xSteps; x++) {
            frame.mesh.addVertex(ofVec2f(x, y) * stepSize / rescale);
        }
    }
    for(int y = 0; y + 1 < ySteps; y++) {
        for(int x = 0; x + 1 < xSteps; x++) {
            int nw = y * xSteps + x;
            int ne = nw + 1;
            int sw = nw + xSteps;
            int se = sw + 1;
            frame.mesh.addIndex(nw);
            frame.mesh.addIndex(ne);
            frame.mesh.addIndex(se);
            frame.mesh.addIndex(nw);
            frame.mesh.addIndex(se);
            frame.mesh.addIndex(sw);
        }
    }
    
    particles.setup();
}

void ContentManager::setSwapBuffers( ofFbo* inBuff, ofFbo* outBuff) {
    swapIn = inBuff;
    swapOut = outBuff;
    frame.swapIn = inBuff;
    frame.swapOut = outBuff;
}

void ContentManager::swapContent(vector<string>* content) {
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
       // player.setVolume(1.0);
        player.play();
    }
    if(splitString[1] == "png" || splitString[1] == "jpeg" || splitString[1] == "jpg") {
        currentContentIsVideo = false;
        loadNewImage(nextContent);
        currentDuration = 5.0;
    }
    timeSinceLastSwap = ofGetElapsedTimef();
}

void ContentManager::updateCamera() {
    camera->update();
    if(frame.particlesOn) {
        float camWidth;
        float camHeight;
#ifdef USING_BLACKMAGIC
        camWidth = camera->getColorTexture().getWidth();
        camHeight = camera->getColorTexture().getHeight();
        scaledImage.setFromPixels(camera->getColorPixels(), camWidth, camHeight);
#else
        camWidth = camera->getWidth();
        camHeight = camera->getHeight();
        scaledImage.setFromPixels(camera->getPixels(), camWidth, camHeight);
#endif
        grayImage = scaledImage;
        
        // take the abs value of the difference between background and incoming and then threshold:
        grayDiff.absDiff(grayBg, grayImage);
        grayDiff.threshold(80);
        
        // find contours which are between the size of 20 pixels and 1/3 the w*h pixels.
        // also, find holes is set to true so we will get interior contours as well....
        contourFinder.findContours(grayDiff, 20, (camWidth*camHeight)/3, 10, true);	// find holes
        
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
    }
    numEffectsOn = 0;
    for(int i = 0; i < frame.EffectsList.size(); i++) {
        if(frame.EffectsList.getBool(i)) {
            numEffectsOn++;
        }
    }
    frame.numEffectsOn = numEffectsOn;
}

void ContentManager::updateContent() {
    player.update();

    if(currentContentIsVideo) {
        if(player.getPosition() > 0.95) {
            swapContent(currentContentNames);
        }
        if(amplifier->Enabled) amplifier->update(player);
    } else {
        if(ofGetElapsedTimef() - timeSinceLastSwap > currentDuration) {
            swapContent(currentContentNames);
        }
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

void ContentManager::drawCamera() {
    float camWidth;
    float camHeight;
#ifdef USING_BLACKMAGIC
    camWidth = camera->getColorTexture().getWidth();
    camHeight = camera->getColorTexture().getHeight();
    ofImage img;
    if(amplifier->Enabled) {
        
        img.setFromPixels(camera->getColorPixels());
        img.resize(VID_WIDTH, VID_HEIGHT);
        amplifier->update(img);
    }
#else
    amplifier->update(*camera);
#endif
    frame.buffer.begin();
    glBlendFuncSeparate(GL_ONE, GL_SRC_COLOR, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    ofClear(0);
    if(flipInput->get()) {
        ofScale(-1, 1);
        ofTranslate(-bufferWidth, 0);
    }
#ifdef USING_BLACKMAGIC
    if(amplifier->Enabled) {
        amplifier->draw(img);
    }
    else {
        ofPushMatrix();
        ofScale(VID_WIDTH/camWidth, VID_HEIGHT/camHeight);
        camera->drawColor();
        ofPopMatrix();
    }
#else
    if(amplifier->Enabled) amplifier->draw(*camera);
    else camera->draw(0, 0, bufferWidth, bufferHeight);
#endif
    if(frame.particlesOn) particles.draw();
    frame.buffer.end();
}

void ContentManager::drawContent() {
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








