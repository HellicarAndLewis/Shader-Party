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
    gui.setup(name);
    EffectsList.setName("Active Effects");
    EffectsList.add(voronoiOn.set("Voronoi", false));
    EffectsList.add(badTvOn.set("Bad TV", false));
    EffectsList.add(colorMapOn.set("Color Map", false));
    EffectsList.add(embossOn.set("Emboss", false));
    EffectsList.add(rgbShiftOn.set("RGB Shift", false));
    EffectsList.add(scanLinesOn.set("Scan Lines", false));
    EffectsList.add(sharpenOn.set("Sharpen", false));
    EffectsList.add(endarkenOn.set("Endarken", false));
    EffectsList.add(explodeOn.set("Explode", false));
    
    activeEffects.push_back(&voronoiOn);
    activeEffects.push_back(&badTvOn);
    activeEffects.push_back(&colorMapOn);
    activeEffects.push_back(&embossOn);
    activeEffects.push_back(&rgbShiftOn);
    activeEffects.push_back(&scanLinesOn);
    activeEffects.push_back(&sharpenOn);
    activeEffects.push_back(&endarkenOn);
    activeEffects.push_back(&explodeOn);
    activeEffectsInOrder.clear();
    gui.add(EffectsList);
    
    ofAddListener(EffectsList.parameterChangedE(), this, &contentCreator::onActiveEffectChanged);
    
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
    presetImg.draw(gui.getPosition().x, gui.getHeight() + Presets.getHeight() + 20, 320, 180);
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

void ContentManager::setup(string pressContentPath, string partyContentPath, int width, int height) {
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
    
    float rescale = .25;
    float scaleFactor = 1. / 10; // could dynamically calculate this from flow3
    
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

void ContentManager::updateCamera() {
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

void ContentManager::updateContent() {
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

void ContentManager::drawCamera() {
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








