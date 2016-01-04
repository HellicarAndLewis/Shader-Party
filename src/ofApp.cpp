#include "ofApp.h"

#define WIDTH ofGetWidth()
#define HEIGHT ofGetHeight()
#define VID_WIDTH 1280
#define VID_HEIGHT 720
#define NUM_SEEDS 200

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
    
    //Setup the image tiler
    mosaic = new videoTiler(1280, 720, 120);
    
    //setup each effect
    
    //Motion Amplifier
    amplifier.setup(WIDTH, HEIGHT, 2, .25);
    
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
    //enDarken
    Effect* endarken = new Effect();
    endarken->setupGui("Endarken");
    endarken->setUniformFlowField(&amplifier.flowTexture);
    endarken->addUniformFloat("darkness", "Darkness", 0.0, 0.0, 1.0);
    endarken->loadShader("shaders/endarkenFrag.glsl");
    effects.push_back(endarken);
    
    //Voronoi
    Effect* voronoi = new Effect();
    voronoi->setupGui("Voronoi");
    voronoi->setUniformFlowField(&amplifier.flowTexture);
    voronoi->addUniformFloat("numActiveSeeds", "Active Seeds", 200.0, 0.0, 200.0);
    voronoi->addUniformVectorArray("locs", (float *)&voronoiSeedLocs[0], voronoiSeedLocs.size());
    voronoi->addUniformFloat("seedVelsMultiplier", "Speed Factor", 0.0, 0.0, 20.0);
    voronoi->loadShader("shaders/VoronoiFrag.glsl");
    effects.push_back(voronoi);
    
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
    main.add(learningRate.set("Motion Learn Rate", 0, -0.2, 1.8));
    main.add(fadeAmnt.set("Fade", 0.0, 0.0, 1.0));
    
    string xmlSettingsPath = "settings/Settings.xml";
    gui.setup("Main", xmlSettingsPath);
    gui.add(main);
    EffectsList.setName("Active Effects");
    EffectsList.add(endarkenOn.set("Endarken", false));
    EffectsList.add(voronoiOn.set("Voronoi", false));
    EffectsList.add(motionAmpOn.set("Motion Amp", false));
    EffectsList.add(badTvOn.set("Bad TV", false));
    EffectsList.add(colorMapOn.set("Color Map", false));
    EffectsList.add(embossOn.set("Emboss", false));
    EffectsList.add(rgbShiftOn.set("RGB Shift", false));
    EffectsList.add(scanLinesOn.set("Scan Lines", false));
    EffectsList.add(sharpenOn.set("Sharpen", false));
    
    activeEffects.push_back(&endarkenOn);
    activeEffects.push_back(&voronoiOn);
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
    mosaicDraw.allocate(VID_WIDTH, VID_HEIGHT);
    
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
    
    //if(animating) {
        for(int i=0; i < voronoiSeedLocs.size(); i++) {
            voronoiSeedLocs[i] += voronoiSeedVels[i] * effects[1]->floatUniforms["seedVelsMultiplier"]->get();
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
    //}

    amplifier.setStrength(strength);
    amplifier.setLearningRate(learningRate);
    amplifier.setBlurAmount(0);
    amplifier.setWindowSize(8);
    
    if(motionAmpOn) {
        amplifier.update(mosaic->img);
    }
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
    
    initialDraw.readToPixels(currImg);
    currImg.update();
    
    mosaic->addImage(currImg);
    
    mosaicDraw.begin();
    mosaic->draw(0, 0, VID_WIDTH, VID_HEIGHT);
    mosaicDraw.end();
    
    if(motionAmpOn) {
        motionWarp.begin();
        ofClear(0);
        ofSetupScreenOrtho(ofGetWidth(), ofGetHeight(), -100, +100);
        ofEnableDepthTest();
        amplifier.draw(mosaic->img);
        ofDisableDepthTest();
        motionWarp.end();
    }
    
    swapIn->begin();
    ofClear(0);
    if(motionAmpOn) motionWarp.draw(0, 0, ofGetWidth() * VID_WIDTH / currImg.getWidth(), ofGetHeight() * VID_HEIGHT / currImg.getHeight());
    else mosaicDraw.draw(0, 0, VID_WIDTH, VID_HEIGHT);
    swapIn->end();

    for(int i = 0; i < activeEffects.size(); i++) {
        if(activeEffects[i]->get()) {
            effects[i]->apply(swapIn, swapOut);
            swapFbos();
        }
    }
    initialDraw.draw(0, 0, WIDTH, HEIGHT);
    mosaicDraw.draw(0, 0, WIDTH, HEIGHT);
    swapIn->draw(0, 0, WIDTH, HEIGHT);
    
    gui.draw();
    int x = gui.getWidth() + 20;
    for(int i = 0; i < activeEffects.size(); i++) {
        if(activeEffects[i]->get()) {
            effects[i]->setGuiPosition(x, 10);
            effects[i]->drawGui();
            x += effects[i]->getGuiWidth() + 10;
        }
    }
    mosaic->gui.setPosition(10, 10+ gui.getHeight());
    mosaic->drawGui();
    
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