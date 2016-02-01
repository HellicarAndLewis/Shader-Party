#include "ofApp.h"

#define WIDTH ofGetScreenWidth()
#define HEIGHT ofGetScreenHeight()
#define OUT_WIDTH 1920
#define OUT_HEIGHT 1080
#define VID_WIDTH 1280
#define VID_HEIGHT 720
#define NUM_SEEDS 400
#define NUM_CHANNELS 20

void stopAndLoadNewVid(ofVideoPlayer* vidPlayer, string vidToLoad) {
    vidPlayer->stop();
    vidPlayer->close();
    vidPlayer->load(vidToLoad);
}

//--------------------------------------------------------------
void ofApp::setup(){
    ofBackground(30, 30, 30);
    
    ofSetLogLevel(OF_LOG_VERBOSE);
    
    drawGui = true;
    
    //Load fonts
    ttf.load("Helvetica.ttf", 48);
    ttfSmall.load("Helvetica.ttf", 14);
    
    //Setup the image tiler
    mosaic = new videoTiler(VID_WIDTH, VID_HEIGHT, 50);
    
    //setup fft
    fft.setMirrorData(false);
    fft.setup();
    fft.setBufferSize(NUM_CHANNELS);
    fft.setConnectedParams(&connectedParams);
    int numChannels = fft.getFftRawData().size()-1;
    
    //setup each effect
    
    //Motion Amplifier
    amplifier.setup(VID_WIDTH, VID_HEIGHT, 2, numChannels, .1);
    amplifier.setBlurAmount(0);
    amplifier.setWindowSize(8);
    
    amplifier.gui.setPosition(10, 500);
    
    dieselHashtag.load("movies/hashtag.mov");
    dieselHashtag.setLoopState(OF_LOOP_NORMAL);
    dieselHashtag.setPaused(false);
    dieselHashtag.play();
    
    //Fade effect for mixing videos
    fade.load("shaders/DummyVert.glsl", "shaders/FadeFrag.glsl");
    
    //setup seed locs for voronoi
    voronoiSeedLocs.resize(NUM_SEEDS);
    voronoiSeedVels.resize(NUM_SEEDS);
    voronoiInitVels.resize(NUM_SEEDS);
    for(int i=0; i < NUM_SEEDS; i++) {
        ofVec2f loc = ofVec2f(ofRandom(WIDTH), ofRandom(HEIGHT));
        ofVec3f col = ofVec3f(ofRandom(255), ofRandom(255), ofRandom(255));
        ofVec2f vel = ofVec2f(ofRandom(-1.0, 1.0), ofRandom(-1.0, 1.0));
        voronoiSeedLocs[i] = loc;
        voronoiSeedVels[i] = vel;
        voronoiInitVels[i] = vel;
    }
        
    //Voronoi
    Effect* voronoi = new Effect();
    voronoi->setupGui("Voronoi", numChannels);
    voronoi->setUniformFlowField(&amplifier.flowTexture);
    voronoi->addUniformFloat("numActiveSeeds", "Active Seeds", 200.0, 0.0, NUM_SEEDS);
    voronoi->addUniformVectorArray("locs", (float *)&voronoiSeedLocs[0], voronoiSeedLocs.size());
    voronoi->addUniformFloat("seedVelsMultiplier", "Speed Factor", 0.0, 0.0, 20.0);
    voronoi->loadShader("shaders/VoronoiFrag.glsl");
    effects.push_back(voronoi);
    
    //BadTv
    Effect* badTv = new Effect();
    badTv->setupGui("BadTv", numChannels);
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
    rampImg->load("textures/ramp1.png");
    colorMap->setupGui("ColorMap", numChannels);
    colorMap->setUniformFlowField(&amplifier.flowTexture);
    colorMap->addUniformFloat("amount", "Amount", 0.5, 0.0, 1.0);
    colorMap->addUniformFloat("timeRate", "Rate", 0.5, 0.1, 1.0);
    badTv->setUniformFlowField(&amplifier.flowTexture);
    colorMap->setUniformImage("colorMap", rampImg);;
    colorMap->loadShader("shaders/colorMap.frag");
    effects.push_back(colorMap);
    
    //Emboss
    Effect* emboss = new Effect();
    emboss->setupGui("Emboss", numChannels);
    emboss->setUniformFlowField(&amplifier.flowTexture);
    emboss->addUniformFloat("amount", "Amount", 0.5, 0.0, 1.0);
    emboss->addUniformFloat("intensity", "Intensity", 50.0, 1.0, 100.0);
    emboss->addUniformFloat("colorization", "Colorization", 0.5, 0.0, 1.0);
    emboss->addUniformFloat("flowEffect", "Flow Effect", 0.0, 0.0, 100.0);
    emboss->loadShader("shaders/emboss.frag");
    effects.push_back(emboss);
    
    //RGB Shift
    Effect* rgbShift = new Effect();
    rgbShift->setupGui("RGB Shift", numChannels);
    rgbShift->setUniformFlowField(&amplifier.flowTexture);
    rgbShift->addUniformFloat("amount", "Amount", 0.5, 0.0, 3.0);
    rgbShift->addUniformFloat("rgbAmount", "RGB Amount", 0.05, 0.0, 0.3);
    rgbShift->loadShader("shaders/rgbshift.frag");
    effects.push_back(rgbShift);
    
    //Scan Lines
    Effect* scanLines = new Effect();
    scanLines->setupGui("Scan Lines", numChannels);
    scanLines->setUniformFlowField(&amplifier.flowTexture);
    scanLines->addUniformFloat("amount", "Amount", 0.5, 0.0, 1.0);
    scanLines->addUniformFloat("nIntensity", "N Intensity", 1.0, 0.0, 2.0);
    scanLines->addUniformFloat("sIntensity", "S Intensity", 1.0, 0.0, 2.0);
    scanLines->addUniformFloat("sCount", "S Count", 500, 50, 1000);
    scanLines->loadShader("shaders/scanlines.frag");
    effects.push_back(scanLines);
    
    //Sharpen
    Effect* sharpen = new Effect();
    sharpen->setupGui("Sharpen", numChannels);
    sharpen->setUniformFlowField(&amplifier.flowTexture);
    sharpen->addUniformFloat("amount", "Amount", 0.5, 0.0, 1.0);
    sharpen->addUniformFloat("theMix", "The Mix", 0.5, 0.0, 1.0);
    sharpen->addUniformFloat("weirdness", "Weirdness", 0.5, 0.0, 1.0);
    sharpen->loadShader("shaders/DummyFrag.glsl");
    effects.push_back(sharpen);
    
    //enDarken
    Effect* endarken = new Effect();
    endarken->setupGui("Endarken", numChannels);
    endarken->setUniformFlowField(&amplifier.flowTexture);
    endarken->addUniformFloat("darkness", "Darkness", 0.5, 0.5, 1.0);
    endarken->loadShader("shaders/endarkenFrag.glsl");
//    endarken->setGuiPosition(10, HEIGHT - 300);
    effects.push_back(endarken);
    
    //explode
    Effect* explode = new Effect();
    explode->setupGui("Explode", numChannels);
    explode->setUniformFlowField(&amplifier.flowTexture);
    explode->addUniformFloat("x_Strength", "X Strength", 0, 0, 400);
    explode->addUniformFloat("y_Strength", "Y Strength", 0, 0, 400);
    explode->addUniformFloat("z_Strength", "Z Strength", 0, 0, 400);
    explode->addUniformFloat("speed", "Speed", 0.2, 0.0, 1.0);
    explode->addUniformFloat("noiseScale", "Scale", 500, 1.0, 1000);
    explode->loadShader("shaders/explode");
    effects.push_back(explode);

    //setup main gui
    //main.setup();
    main.add(camInput.set("Camera On", false));
    main.add(PartyOn.set("Party Mode", false));
    PartyOn.addListener(this, &ofApp::onPartyModeChange);
    
    string xmlSettingsPath = "settings/Settings.xml";
    gui.setup("Main", xmlSettingsPath);
    gui.add(main);
    gui.setPosition(10, HEIGHT - 150);
    
    gui.loadFromFile(xmlSettingsPath);
    for(int i = 0; i < effects.size(); i++) {
        effects[i]->loadSettings();
        effects[i]->setGuiPosition(gui.getWidth() + 20, 10);
    }
    
    //INITIALIZE CAMERA
    cam.initGrabber(VID_WIDTH, VID_HEIGHT);
    
    motionWarp.allocate(VID_WIDTH, VID_HEIGHT);
    //allocate drawing fbo
    finalMix.allocate(VID_WIDTH, VID_HEIGHT);
    mosaicDraw.allocate(VID_WIDTH, VID_HEIGHT);
    
    swapIn = new ofFbo();
    swapOut = new ofFbo();
    
    swapIn->allocate(VID_WIDTH, VID_HEIGHT);
    swapOut->allocate(VID_WIDTH, VID_HEIGHT);

    currImg.allocate(VID_WIDTH, VID_HEIGHT, OF_IMAGE_COLOR);
    
    //LOAD MOVIES HERE
    //***************
    contentManager = new ContentManager();
    contentManager->setup("movies/Press", "movies/Party", VID_WIDTH, VID_HEIGHT);
    contentManager->play();
    
    contentManager->frame.setEffects(&effects);
    contentManager->frame.setupGui("Right");
    contentManager->setAmplifier(&amplifier);
    contentManager->setCamera(&cam);
    contentManager->setSwapBuffers(swapIn, swapOut);
    contentManager->setTiler(mosaic);
    contentManager->frame.gui.setPosition(10, 10);
    contentManager->frame.Presets.setPosition(10, contentManager->frame.gui.getHeight() + 20);
    contentManager->frame.Particles.setPosition(10, HEIGHT - contentManager->frame.Particles.getHeight() - 10);
    
    fftCut.setup("FFT Cut", "settings/fftCut.xml");
    fftCut.add(upperCut.set("Upper", 1.0, 0.0, 3.0));
    fftCut.add(lowerCut.set("Lower", 0.0, 0.0, 3.0));
    fftCut.setPosition(10, HEIGHT - 300 - endarken->gui.getHeight());
    
    mosaic->gui.setPosition(10, 675);
    
    activeBuffer = swapIn;
}

//--------------------------------------------------------------
void ofApp::update(){
    for(int i=0; i < voronoiSeedLocs.size(); i++) {
        voronoiSeedLocs[i] += voronoiSeedVels[i] * effects[0]->floatUniforms["seedVelsMultiplier"]->get();
        float projX = ofMap(voronoiSeedLocs[i].x, 0, VID_WIDTH, 1, VID_HEIGHT-1, true);
        float projY = ofMap(voronoiSeedLocs[i].y, 0, VID_WIDTH, 1, VID_WIDTH-1, true);
        
        if(voronoiSeedLocs[i].x < 0 || voronoiSeedLocs[i].x > VID_WIDTH) {
            voronoiSeedVels[i].x *= -1;
            voronoiInitVels[i].x *= -1;
            voronoiSeedLocs[i].x = (voronoiSeedLocs[i].x < 0) ? 0 : VID_WIDTH;
        }
        if(voronoiSeedLocs[i].y < 0 || voronoiSeedLocs[i].y > VID_HEIGHT) {
            voronoiSeedVels[i].y *= -1;
            voronoiInitVels[i].y *= -1;
            voronoiSeedLocs[i].y = (voronoiSeedLocs[i].y < 0) ? 0 : VID_HEIGHT;
        }
    }
    
//    fft.setDampingParams(fftDamping.get(), fftAttraction.get());
    vector<float> vals = fft.getFftPeakData();
    for(int i = 0; i < effects.size(); i++) {
        effects[i]->updateFromFFT(vals, upperCut, lowerCut);
        amplifier.updateFromFFT(vals, upperCut, lowerCut);
    }
    
    fft.update();
    
    contentManager->update(camInput);
    if(camInput) {
        dieselHashtag.update();
    }
    
    connectedParams.clear();
    for(int i=0; i < effects.size(); i++) {
        map<string, ofParameter<bool>*> paramsConnected = effects[i]->fftConnected;
        map<string, ofParameter<int>*> connectedParamValues = effects[i]->fftChannels;
        string effectName = effects[i]->gui.getName();
        for(auto it = paramsConnected.begin(); it != paramsConnected.end(); it++) {
            if(it->second->get()) {
                string paramName = effectName + "\n" + it->first;
                int channel = connectedParamValues.find(it->first)->second->get();
                pair<int, string> channelAndName = make_pair(channel, paramName);
                connectedParams.push_back(channelAndName);
            }
        }
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    contentManager->drawBuffers(camInput);

    swapIn->begin();
    ofClear(0);
    contentManager->frame.buffer.draw(0, 0, VID_WIDTH, VID_HEIGHT);
    swapIn->end();
    
    swapOut->begin();
    ofClear(0);
    contentManager->frame.buffer.draw(0, 0, VID_WIDTH, VID_HEIGHT);
    swapOut->end();
    
    contentManager->applyEffects();
    
    if(contentManager->numEffectsOn%2 == 1) {
        activeBuffer = swapOut;
    } else {
        activeBuffer = swapIn;
    }
    
    finalMix.begin();
        activeBuffer->draw(0, 0, VID_WIDTH, VID_HEIGHT);
    finalMix.end();
    
    finalMix.readToPixels(currImg);
    currImg.update();
    mosaic->addImage(currImg);
    
    if(camInput) {
        mosaic->drawWithTimeOffset(0, 0, OUT_WIDTH, OUT_HEIGHT);
        
        dieselHashtag.draw(90, HEIGHT - dieselHashtag.getHeight() - 90, dieselHashtag.getWidth(), dieselHashtag.getHeight());
        ofPushStyle();
        ofSetColor(0);
        ofDrawRectangle(90 + dieselHashtag.getWidth() - 1, HEIGHT - dieselHashtag.getHeight() - 90, 2, dieselHashtag.getHeight());
        ofPopStyle();
        if(drawGui){            
            mosaic->drawGui();
            
            gui.draw();
            presets.draw();
            
            contentManager->drawGuis();
            
            amplifier.drawGui();
            
//            endarken->drawGui();
            
            fftCut.draw();
            
            int fftWidth = WIDTH/2;
            int fftHeight = 100;
            fft.draw(WIDTH/2 - fftWidth/2, HEIGHT - fftHeight, fftWidth, fftHeight);
            
            ofDrawBitmapStringHighlight(ofToString(ofGetFrameRate()), WIDTH - 100, HEIGHT - 20);
        }
    } else {
        mosaic->drawWithTimeOffset(WIDTH/2 - VID_WIDTH/2, HEIGHT/2 - VID_HEIGHT/2, VID_WIDTH, VID_HEIGHT);
        
        mosaic->drawWithTimeOffset(WIDTH, 0, OUT_WIDTH, OUT_HEIGHT);
        
        mosaic->drawGui();
        
        gui.draw();
        presets.draw();
        
        contentManager->drawGuis();
        
        amplifier.drawGui();
        
//        endarken->drawGui();
        
        fftCut.draw();
        
        int fftWidth = WIDTH/2;
        int fftHeight = 100;
        fft.draw(WIDTH/2 - fftWidth/2, HEIGHT - fftHeight, fftWidth, fftHeight);
        
        ofDrawBitmapStringHighlight(ofToString(ofGetFrameRate()), WIDTH - 100, HEIGHT - 20);
        ofDrawBitmapString(ofGetTimestampString("Time: %H : %M"), WIDTH - 200, 20);
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if(camInput) {
        if(key == ' ') {
            camInput = false;
        }
        if(key == 'g') {
            drawGui = !drawGui;
        }
    }
}

//--------------------------------------------------------------
void ofApp::onPartyModeChange(bool &b) {
    contentManager->changeMode(b);
}


//--------------------------------------------------------------
void ofApp::swapFbos() {
    ofFbo* temp;
    temp = swapIn;
    swapIn = swapOut;
    swapOut = temp;
}