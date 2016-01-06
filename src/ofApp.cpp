#include "ofApp.h"

#define WIDTH ofGetScreenWidth()
#define HEIGHT ofGetScreenHeight()
#define OUT_WIDTH 1920
#define OUT_HEIGHT 1080
#define VID_WIDTH 1920 //1280
#define VID_HEIGHT 1080 //720
#define NUM_SEEDS 200
#define NUM_CHANNELS 20

void stopAndLoadNewVid(ofVideoPlayer* vidPlayer, string vidToLoad) {
    vidPlayer->stop();
    vidPlayer->close();
    vidPlayer->loadMovie(vidToLoad);
}

//--------------------------------------------------------------
void ofApp::setup(){
    ofBackground(30, 30, 30);
    
//    ofSetLogLevel(OF_LOG_SILENT);
    
    //Load fonts
    ttf.loadFont("Helvetica.ttf", 48);
    ttfSmall.loadFont("Helvetica.ttf", 14);
    
    //Setup the image tiler
    mosaic = new videoTiler(VID_WIDTH, VID_HEIGHT, 120);
    
    //setup fft
    fft.setMirrorData(false);
    fft.setup();
    fft.setBufferSize(NUM_CHANNELS);
    int numChannels = fft.getFftRawData().size()-1;
    
    //setup each effect
    
    //Motion Amplifier
    amplifier.setup(WIDTH, HEIGHT, 2, numChannels, .25);
    
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
    
    for(int i = 0; i < 2; i++) {
        
        //Voronoi
        Effect* voronoi = new Effect();
        voronoi->setupGui("Voronoi", numChannels);
        voronoi->setUniformFlowField(&amplifier.flowTexture);
        voronoi->addUniformFloat("numActiveSeeds", "Active Seeds", 200.0, 0.0, 200.0);
        voronoi->addUniformVectorArray("locs", (float *)&voronoiSeedLocs[0], voronoiSeedLocs.size());
        voronoi->addUniformFloat("seedVelsMultiplier", "Speed Factor", 0.0, 0.0, 20.0);
        voronoi->loadShader("shaders/VoronoiFrag.glsl");
        effects[i].push_back(voronoi);
        
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
        effects[i].push_back(badTv);
        
        //ColorMap
        Effect* colorMap = new Effect();
        ofImage* rampImg = new ofImage();
        rampImg->loadImage("textures/ramp1.png");
        colorMap->setupGui("ColorMap", numChannels);
        colorMap->setUniformFlowField(&amplifier.flowTexture);
        colorMap->addUniformFloat("amount", "Amount", 0.5, 0.0, 1.0);
        badTv->setUniformFlowField(&amplifier.flowTexture);
        colorMap->setUniformImage("colorMap", rampImg);;
        colorMap->loadShader("shaders/colorMap.frag");
        effects[i].push_back(colorMap);
        
        //Emboss
        Effect* emboss = new Effect();
        emboss->setupGui("Emboss", numChannels);
        emboss->setUniformFlowField(&amplifier.flowTexture);
        emboss->addUniformFloat("amount", "Amount", 0.5, 0.0, 1.0);
        emboss->addUniformFloat("intensity", "Intensity", 50.0, 1.0, 100.0);
        emboss->addUniformFloat("colorization", "Colorization", 0.5, 0.0, 1.0);
        emboss->addUniformFloat("flowEffect", "Flow Effect", 0.0, 0.0, 100.0);
        emboss->loadShader("shaders/emboss.frag");
        effects[i].push_back(emboss);
        
        //RGB Shift
        Effect* rgbShift = new Effect();
        rgbShift->setupGui("RGB Shift", numChannels);
        rgbShift->setUniformFlowField(&amplifier.flowTexture);
        rgbShift->addUniformFloat("amount", "Amount", 0.5, 0.0, 1.0);
        rgbShift->addUniformFloat("rgbAmount", "RGB Amount", 0.05, 0.0, 0.1);
        rgbShift->loadShader("shaders/rgbshift.frag");
        effects[i].push_back(rgbShift);
        
        //Scan Lines
        Effect* scanLines = new Effect();
        scanLines->setupGui("Scan Lines", numChannels);
        scanLines->setUniformFlowField(&amplifier.flowTexture);
        scanLines->addUniformFloat("amount", "Amount", 0.5, 0.0, 1.0);
        scanLines->addUniformFloat("nIntensity", "N Intensity", 1.0, 0.0, 2.0);
        scanLines->addUniformFloat("sIntensity", "S Intensity", 1.0, 0.0, 2.0);
        scanLines->addUniformFloat("sCount", "S Count", 500, 50, 1000);
        scanLines->loadShader("shaders/scanlines.frag");
        effects[i].push_back(scanLines);
        
        //Sharpen
        Effect* sharpen = new Effect();
        sharpen->setupGui("Sharpen", numChannels);
        sharpen->setUniformFlowField(&amplifier.flowTexture);
        sharpen->addUniformFloat("amount", "Amount", 0.5, 0.0, 1.0);
        sharpen->addUniformFloat("theMix", "The Mix", 0.5, 0.0, 1.0);
        sharpen->addUniformFloat("weirdness", "Weirdness", 0.5, 0.0, 1.0);
        sharpen->loadShader("shaders/DummyFrag.glsl");
        effects[i].push_back(sharpen);
        
        //enDarken
        Effect* endarken = new Effect();
        endarken->setupGui("Endarken", numChannels);
        endarken->setUniformFlowField(&amplifier.flowTexture);
        endarken->addUniformFloat("darkness", "Darkness", 0.0, 0.0, 1.0);
        endarken->loadShader("shaders/endarkenFrag.glsl");
        effects[i].push_back(endarken);
    }

    //setup main gui
    //main.setup();
    main.add(fadeAmnt.set("Fade", 0.0, 0.0, 1.0));
    main.add(fftDamping.set("Damping", 0.5, 0.0, 2.0));
    main.add(fftAttraction.set("Attraction", 0.5, 0.0, 2.0));
    
    string xmlSettingsPath = "settings/Settings.xml";
    gui.setup("Main", xmlSettingsPath);
    gui.add(main);
    gui.setPosition(WIDTH/2 - gui.getWidth()/2, HEIGHT/2 - gui.getHeight()*2);
    
    gui.loadFromFile(xmlSettingsPath);
    for(int j = 0; j < 2; j++) {
        for(int i = 0; i < effects[j].size(); i++) {
            effects[j][i]->loadSettings();
            effects[j][i]->setGuiPosition(gui.getWidth() + 20, 10 + j * gui.getHeight());
        }
    }
    
    //LOAD MOVIES HERE
    //***************
    //
    contentManager = new ContentManager();
    contentManager->setup("movies/Press", "movies/Party", VID_WIDTH, VID_HEIGHT);
    contentManager->play();
    
    contentManager->frames[0].setEffects(&effects[0]);
    contentManager->frames[1].setEffects(&effects[1]);
    contentManager->frames[0].setupGui("Left");
    contentManager->frames[1].setupGui("Right");
    
    for(int i = 0; i < 2; i++) {
        //allocate drawing fbo
        finalMix.allocate(VID_WIDTH, VID_HEIGHT);
        motionWarp[i].allocate(VID_WIDTH, VID_HEIGHT);
        mosaicDraw[i].allocate(VID_WIDTH, VID_HEIGHT);
        
        swapIn[i] = new ofFbo();
        swapOut[i] = new ofFbo();
        
        swapIn[i]->allocate(VID_WIDTH, VID_HEIGHT);
        swapOut[i]->allocate(VID_WIDTH, VID_HEIGHT);
    }

    
    currImg.allocate(VID_WIDTH, VID_HEIGHT, OF_IMAGE_COLOR);
}

//--------------------------------------------------------------
void ofApp::update(){
//    for(int i = 0; i < 2; i++) {
//        players[i]->update();
//    }
    
        for(int i=0; i < voronoiSeedLocs.size(); i++) {
            voronoiSeedLocs[i] += voronoiSeedVels[i] * effects[0][0]->floatUniforms["seedVelsMultiplier"]->get();
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
    vector<float> vals = fft.getDampedNormData();
    for(int j = 0; j < 2; j++) {
        for(int i = 0; i < effects[j].size(); i++) {
            effects[j][i]->updateFromFFT(vals);
            amplifier.updateFromFFT(vals);
        }
    }

    amplifier.setBlurAmount(0);
    amplifier.setWindowSize(8);
    
//    if(motionAmpOn) {
//        amplifier.update(mosaic->img);
//    }
    
    fft.update();
    
    contentManager->update();
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    contentManager->drawBuffers();
    
//    finalMix.readToPixels(currImg);
//    currImg.update();
//    
//    mosaic->addImage(currImg);
//
    for(int i = 0; i < 2; i++) {
        mosaicDraw[i].begin();
        mosaic->draw(&contentManager->frames[i].buffer, 0, 0, VID_WIDTH, VID_HEIGHT);
        mosaicDraw[i].end();
        
//        if(motionAmpOn) {
//            //finalMix[i].readToPixels(currImg);
//            currImg.update();
//            motionWarp[i].begin();
//            ofClear(0);
//            ofSetupScreenOrtho(ofGetWidth(), ofGetHeight(), -100, +100);
//            ofEnableDepthTest();
//            amplifier.draw(currImg);
//            ofDisableDepthTest();
//            motionWarp[i].end();
//        }
        
        swapIn[i]->begin();
        ofClear(0);
        //if(motionAmpOn) motionWarp[i].draw(0, 0, ofGetWidth() * VID_WIDTH / currImg.getWidth(), ofGetHeight() * VID_HEIGHT / currImg.getHeight());
        mosaicDraw[i].draw(0, 0, VID_WIDTH, VID_HEIGHT);
        swapIn[i]->end();
        swapOut[i]->begin();
        ofClear(0);
        //if(motionAmpOn) motionWarp[i].draw(0, 0, ofGetWidth() * VID_WIDTH / currImg.getWidth(), ofGetHeight() * VID_HEIGHT / currImg.getHeight());
        mosaicDraw[i].draw(0, 0, VID_WIDTH, VID_HEIGHT);
        swapOut[i]->end();
    }
    contentManager->applyEffects(swapIn[0], swapOut[0], swapIn[1], swapOut[1]);

//    finalMix.draw(0, 0, WIDTH, HEIGHT);
//    mosaicDraw.draw(0, 0, WIDTH, HEIGHT);
//    swapIn->draw(WIDTH/2 - swapIn->getWidth()/2, HEIGHT/2 - swapIn->getHeight()/2);
//    swapIn->draw(WIDTH, 0, OUT_WIDTH, OUT_HEIGHT);
    
    finalMix.begin();
    fade.begin();
    fade.setUniformTexture("texOut", swapOut[0]->getTextureReference(), 1);
    fade.setUniformTexture("texIn", swapOut[1]->getTextureReference(), 2);
    fade.setUniform1f("fadeAmnt", fadeAmnt);
    currImg.draw(0, 0, VID_WIDTH, VID_HEIGHT);
    fade.end();
    finalMix.end();
    
    //swapIn[0]->draw(WIDTH/2 - swapIn[0]->getWidth()/2, HEIGHT/2 - swapIn[0]->getHeight()/2);
    
    swapOut[0]->draw(0, HEIGHT/2, VID_WIDTH/3, VID_HEIGHT/3);
    ofDrawBitmapString("LEFT", 0 + VID_WIDTH/8, HEIGHT/2 - 10);
    swapOut[1]->draw(WIDTH - VID_WIDTH/3, HEIGHT/2, VID_WIDTH/3, VID_HEIGHT/3);
    ofDrawBitmapString("RIGHT", WIDTH - VID_WIDTH/4 + VID_WIDTH/8, HEIGHT/2 - 10);
    finalMix.draw(WIDTH/2 - VID_WIDTH/6, HEIGHT/2, OUT_WIDTH/3, OUT_HEIGHT/3);
    ofDrawBitmapString("OUTPUT", WIDTH/2, HEIGHT/2 - 10);


    //swapIn[1]->draw(WIDTH/2 + VID_WIDTH/4, HEIGHT/2 + VID_HEIGHT/8, VID_WIDTH/4, VID_HEIGHT/4);

    finalMix.draw(WIDTH, 0, OUT_WIDTH, OUT_HEIGHT);
    //swapIn[0]->draw(WIDTH/2 - swapIn[0]->getWidth()/2, HEIGHT/2 - swapIn[0]->getHeight()/2);

    
    for(int j = 0; j < 2; j++) {
        //mosaic->gui.setPosition(10, 10+ gui.getHeight());
        mosaic->drawGui();
    }
    
    gui.draw();
    for(int i = 0; i < 2; i++) {
        presets[i].draw();
    }
    
    contentManager->drawGuis(10, 10, ofGetScreenWidth()/2+10, 10);

    for(int i = 0; i < 2; i++) {
        int fftWidth = WIDTH/2;
        int fftHeight = 100;
        fft.draw(i * fftWidth, HEIGHT - fftHeight, fftWidth, fftHeight, &effects[i]);
    }
    
    ofDrawBitmapStringHighlight(ofToString(ofGetFrameRate()), WIDTH - 100, HEIGHT - 20);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    ofShowCursor();
//    players[currentPlayer]->stop();
//    players[currentPlayer]->close();
//    players[currentPlayer]->loadMovie("movies/Xballs2.mov");
}

//--------------------------------------------------------------
void ofApp::swapFbos(int i) {
    ofFbo* temp;
    temp = swapIn[i];
    swapIn[i] = swapOut[i];
    swapOut[i] = temp;
}