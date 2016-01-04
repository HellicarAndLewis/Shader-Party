#include "ofApp.h"

#define WIDTH ofGetWidth()
#define HEIGHT ofGetHeight()
#define VID_WIDTH 1280
#define VID_HEIGHT 720

void stopAndLoadNewVid(ofxAVFVideoPlayer* vidPlayer, string vidToLoad) {
    vidPlayer->stop();
    vidPlayer->close();
    vidPlayer->loadMovie(vidToLoad);
}

//--------------------------------------------------------------
void ofApp::setup(){
    ofBackground(30, 30, 30);
    
    //Load fonts
    ttf.loadFont("Helvetica.ttf", 48);
    ttfSmall.loadFont("Helvetica.ttf", 14);
    
    //Set last time in the past
    lastTime = ofGetElapsedTimef() - 10.0;
    
    //setup each effect
    
    //Motion Amplifier
    amplifier.setup(WIDTH, HEIGHT, 2, .25);
    
    //Fade effect for mixing videos
    fade.load("shaders/DummyVert.glsl", "shaders/FadeFrag.glsl");
    
    //BadTv
    Effect* badTv = new Effect();
    badTv->setupGui("BadTv");
    badTv->setUniformFlowField(&amplifier.flowTexture);
    badTv->addUniformFloat("amount", "Amount", 0.5, 0.0, 1.0);
    badTv->addUniformFloat("speed", "Speed", 0.5, 0.0, 1.0);
    badTv->addUniformFloat("rollSpeed", "Roll Speed", 0.5, 0.0, 1.0);
    badTv->addUniformFloat("distortion", "Distortion 1", 5.0, 0.0, 10.0);
    badTv->addUniformFloat("distortion2", "Distortion 2", 0.5, 0.0, 1.0);
    badTv->loadShader("shaders/badtv.frag");
    effects.push_back(badTv);
    
    //ColorMap
    Effect* colorMap = new Effect();
    ofImage* rampImg = new ofImage();
    rampImg->loadImage("textures/ramp1.png");
    colorMap->setupGui("ColorMap");
    colorMap->setUniformFlowField(&amplifier.flowTexture);
    colorMap->addUniformFloat("amount", "Amount", 0.5, 0.0, 1.0);
    badTv->setUniformFlowField(&amplifier.flowTexture);
    colorMap->setUniformImage("colorMap", rampImg);;
    colorMap->loadShader("shaders/colorMap.frag");
    effects.push_back(colorMap);
    
    //Emboss
    Effect* emboss = new Effect();
    emboss->setupGui("Emboss");
    emboss->setUniformFlowField(&amplifier.flowTexture);
    emboss->addUniformFloat("amount", "Amount", 0.5, 0.0, 1.0);
    emboss->addUniformFloat("intensity", "Intensity", 50.0, 1.0, 100.0);
    emboss->addUniformFloat("colorization", "Colorization", 0.5, 0.0, 1.0);
    emboss->addUniformFloat("flowEffect", "Flow Effect", 0.0, 0.0, 100.0);
    emboss->loadShader("shaders/emboss.frag");
    effects.push_back(emboss);
    
    //RGB Shift
    Effect* rgbShift = new Effect();
    rgbShift->setupGui("RGB Shift");
    rgbShift->setUniformFlowField(&amplifier.flowTexture);
    rgbShift->addUniformFloat("amount", "Amount", 0.5, 0.0, 1.0);
    rgbShift->addUniformFloat("rgbAmount", "RGB Amount", 0.05, 0.0, 0.1);
    rgbShift->loadShader("shaders/rgbshift.frag");
    effects.push_back(rgbShift);
    
    //Scan Lines
    Effect* scanLines = new Effect();
    scanLines->setupGui("Scane Lines");
    scanLines->setUniformFlowField(&amplifier.flowTexture);
    scanLines->addUniformFloat("amount", "Amount", 0.5, 0.0, 1.0);
    scanLines->addUniformFloat("nIntensity", "N Intensity", 1.0, 0.0, 2.0);
    scanLines->addUniformFloat("sIntensity", "S Intensity", 1.0, 0.0, 2.0);
    scanLines->addUniformFloat("sCount", "S Count", 500, 50, 1000);
    scanLines->loadShader("shaders/scanlines.frag");
    effects.push_back(scanLines);
    
    //Sharpen
    Effect* sharpen = new Effect();
    sharpen->setupGui("Sharpen");
    sharpen->setUniformFlowField(&amplifier.flowTexture);
    sharpen->addUniformFloat("amount", "Amount", 0.5, 0.0, 1.0);
    sharpen->addUniformFloat("theMix", "The Mix", 0.5, 0.0, 1.0);
    sharpen->addUniformFloat("weirdness", "Weirdness", 0.5, 0.0, 1.0);
    sharpen->loadShader("shaders/DummyFrag.glsl");
    effects.push_back(sharpen);
    
    //setup main gui
    //main.setup();
    main.add(amount.set("Amount", 0.5, 0.0, 1.0));
    main.add(activeEffect.set("Effect", 0, 0, effects.size() - 1));
    main.add(strength.set("Motion Amplification", 0, -30, 30));
    main.add(learningRate.set("Motion Learn Rate", 0, -2, 2));
    main.add(fadeAmnt.set("Fade", 0.0, 0.0, 1.0));
    
    string xmlSettingsPath = "settings/Settings.xml";
    gui.setup("Main", xmlSettingsPath);
    gui.add(main);
    EffectsList.setName("Active Effects");
    EffectsList.add(motionAmp.set("Motion Amp", false));
    EffectsList.add(badTvOn.set("Bad TV", false));
    EffectsList.add(colorMapOn.set("Color Map", false));
    EffectsList.add(embossOn.set("Emboss", false));
    EffectsList.add(rgbShiftOn.set("RGB Shift", false));
    EffectsList.add(scanLinesOn.set("Scan Lines", false));
    EffectsList.add(sharpenOn.set("Sharpen", false));
    
    activeEffects.push_back(&badTvOn);
    activeEffects.push_back(&colorMapOn);
    activeEffects.push_back(&embossOn);
    activeEffects.push_back(&rgbShiftOn);
    activeEffects.push_back(&scanLinesOn);
    activeEffects.push_back(&sharpenOn);

    gui.add(EffectsList);
    
    gui.loadFromFile(xmlSettingsPath);
    
    for(int i = 0; i < effects.size(); i++) {
        effects[i]->loadSettings();
        effects[i]->setGuiPosition(gui.getWidth() + 20, 10);
    }
    
    //Load all the movies
    ofDirectory moviesDir("movies");
    //moviesDir.allowExt("mp4");
    moviesDir.allowExt("mov");
    moviesDir.listDir();
    for(int i=0; i < moviesDir.numFiles(); i++) {
        string path = moviesDir.getPath(i);
        movies.push_back(path);
    }
    
    videoLoader.setup("Video Loader");
    for(int i = 0; i < movies.size(); i++) {
        ofParameter<bool> movie;
        videoLoader.add(movie.set(movies[i], false));
    }
    
    for(int i=0; i<2; i++) {
        players[i] = new ofxAVFVideoPlayer();
        players[i]->setLoopState(OF_LOOP_NORMAL);
        players[i]->setPixelFormat(OF_PIXELS_RGB);
        players[i]->loadMovie(movies[i]);
    }
    
    //allocate drawing fbo
    initialDraw.allocate(VID_WIDTH, VID_HEIGHT);
    motionWarp.allocate(VID_WIDTH, VID_HEIGHT);
    shaderPass.allocate(VID_WIDTH, VID_HEIGHT);
    
    swapIn = new ofFbo();
    swapOut = new ofFbo();
    
    swapIn->allocate(VID_WIDTH, VID_HEIGHT);
    swapOut->allocate(VID_WIDTH, VID_HEIGHT);
    
    currImg.allocate(VID_WIDTH, VID_HEIGHT, OF_IMAGE_COLOR);
}

//--------------------------------------------------------------
void ofApp::update(){
    for(int i = 0; i < 2; i++) {
        players[i]->update();
        if(players[i]->isLoaded()) {
            players[i]->setPaused(false);
            players[i]->play();
        }
    }

    amplifier.setStrength(strength);
    amplifier.setLearningRate(learningRate);
    amplifier.setBlurAmount(0);
    amplifier.setWindowSize(8);
    
    initialDraw.readToPixels(currImg);
    currImg.update();
    
    amplifier.update(currImg);
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    initialDraw.begin();
    fade.begin();
    fade.setUniformTexture("texOut", players[0]->getTextureReference(), 1);
    fade.setUniformTexture("texIn", players[1]->getTextureReference(), 2);
    fade.setUniform1f("fadeAmnt", fadeAmnt);
    currImg.draw(0, 0, VID_WIDTH, VID_HEIGHT);
    fade.end();
    initialDraw.end();
    
    initialDraw.draw(0, 0);
    
        if(motionAmp) {
        motionWarp.begin();
        ofClear(0);
        ofSetupScreenOrtho(ofGetWidth(), ofGetHeight(), -100, +100);
        ofEnableDepthTest();
        amplifier.draw(currImg);
        ofDisableDepthTest();
        motionWarp.end();
    }
    
    swapIn->begin();
    ofClear(0);
    if(motionAmp) motionWarp.draw(0, 0, ofGetWidth() * VID_WIDTH / currImg.getWidth(), ofGetHeight() * VID_HEIGHT / currImg.getHeight());
    else currImg.draw(0, 0, VID_WIDTH, VID_HEIGHT);
    swapIn->end();

    for(int i = 0; i < activeEffects.size(); i++) {
        if(activeEffects[i]->get()) {
            effects[i]->apply(swapIn, swapOut);
            swapFbos();
        }
    }
    
    swapIn->draw(0, 0, WIDTH, HEIGHT);
    
    gui.draw();
    int x = gui.getWidth() + 20;
    videoLoader.setPosition(x, 10);
    videoLoader.draw();
    x += videoLoader.getWidth() + 10;
    for(int i = 0; i < activeEffects.size(); i++) {
        if(activeEffects[i]->get()) {
            effects[i]->setGuiPosition(x, 10);
            effects[i]->drawGui();
            x += effects[i]->getGuiWidth() + 10;
        }
    }
    
    ttfSmall.drawString(ofToString(ofGetFrameRate()), ofGetWidth() - 100, ofGetHeight() - 20);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    players[currentPlayer]->stop();
    players[currentPlayer]->close();
    players[currentPlayer]->loadMovie("movies/Xballs2.mov");
}

//--------------------------------------------------------------
void ofApp::swapFbos() {
    ofFbo* temp;
    temp = swapIn;
    swapIn = swapOut;
    swapOut = temp;
}