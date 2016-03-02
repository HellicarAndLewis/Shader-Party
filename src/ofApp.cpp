#include "ofApp.h"

#define WIDTH ofGetScreenWidth()
#define HEIGHT ofGetScreenHeight()
#define OUT_WIDTH 1920
#define OUT_HEIGHT 1080
#define VID_WIDTH 1280
#define VID_HEIGHT 720
#define NUM_VORONOI_SEEDS 200
#define NUM_KALEIDOSCOPE_SEEDS 100
#define NUM_CHANNELS 10

void stopAndLoadNewVid(ofVideoPlayer* vidPlayer, string vidToLoad) {
    vidPlayer->stop();
    vidPlayer->close();
    vidPlayer->load(vidToLoad);
}

//--------------------------------------------------------------
void ofApp::setup(){
    ofBackground(30, 30, 30);
    
    //ofSetLogLevel(OF_LOG_SILENT);
    
    drawGui = true;
    
    //Load fonts
    ttf.load("Helvetica.ttf", 48);
    ttfSmall.load("Helvetica.ttf", 14);
    
    //Setup the image tiler
    mosaic = new videoTiler(VID_WIDTH, VID_HEIGHT, 50);
    
    //setup fft
#ifdef USING_FFT
    fft.setMirrorData(false);
    fft.setup();
    fft.setBufferSize(NUM_CHANNELS);
    fft.setConnectedParams(&connectedParams);
    int numChannels = fft.getFftRawData().size()-1;
#else
    onset.setup();
    int numChannels = 20;
#endif
    ofSoundStreamSetup(0, 2);
    
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
    voronoiSeedLocs.resize(NUM_VORONOI_SEEDS);
    voronoiSeedVels.resize(NUM_VORONOI_SEEDS);
    voronoiInitVels.resize(NUM_VORONOI_SEEDS);
    for(int i=0; i < NUM_VORONOI_SEEDS; i++) {
        ofVec2f loc = ofVec2f(ofRandom(VID_WIDTH), ofRandom(VID_HEIGHT));
        ofVec3f col = ofVec3f(ofRandom(255), ofRandom(255), ofRandom(255));
        ofVec2f vel = ofVec2f(ofRandom(-1.0, 1.0), ofRandom(-1.0, 1.0));
        voronoiSeedLocs[i] = loc;
        voronoiSeedVels[i] = vel;
        voronoiInitVels[i] = vel;
    }
    
    
    kaleidoscopeSeedLocs.resize(NUM_KALEIDOSCOPE_SEEDS);
    kaleidoscopeSeedNormals.resize(NUM_KALEIDOSCOPE_SEEDS);
    
    for(int i = 0; i < NUM_KALEIDOSCOPE_SEEDS; i++) {
        ofVec2f loc = ofVec2f(ofRandom(VID_WIDTH), ofRandom(VID_HEIGHT));
        ofVec2f normal = ofVec2f(ofRandom(-1, 1), ofRandom(-1, 1));
        kaleidoscopeSeedLocs[i] = loc;
        kaleidoscopeSeedNormals[i] = normal;
    }
    
    int num = 1;
//    
//    //Bad VHS
//    Effect* badVHS = new Effect();
//    badVHS->setUniformFlowField(&amplifier.flowTexture);
//    badVHS->setupGui("Bad VHS", numChannels);
//    badVHS->addUniformFloat("waves", "Waves", 1.0, 0.0, 10.0);
//    badVHS->addUniformFloat("chromaAb", "Chroma AB", 1.0, 0.0, 10.0);
//    badVHS->loadShader("shaders/vhs.frag");
//    badVHS->setControllerNumber(num++);
//    effects.push_back(badVHS);
    
//    //Night Vision
//    Effect* nightVision = new Effect();
//    nightVision->setUniformFlowField(&amplifier.flowTexture);
//    nightVision->setupGui("Night Vision", numChannels);
//    nightVision->addUniformFloat("luminanceThreshold", "Luminance", 0.2, 0.01, 1.0);
//    nightVision->addUniformFloat("colorAmplification", "Color Amp", 4.0, 0.0, 20.0);
//    nightVision->loadShader("shaders/nightVision.frag");
//    nightVision->setControllerNumber(num++);
//    effects.push_back(nightVision);
//    
//    //Glitch
//    Effect* glitch =new Effect();
//    ofImage* noiseImage = new ofImage();
//    noiseImage->load("textures/multiColorStatic.jpg");
//    glitch->setUniformFlowField(&amplifier.flowTexture);
//    glitch->setUniformImage("coloredNoise", noiseImage);
//    glitch->setupGui("Glitch", numChannels);
//    glitch->loadShader("shaders/glitch.frag");
//    glitch->setControllerNumber(num++);
//    effects.push_back(glitch);
    
    //Kaleidoscope
    Effect* kaleidoscope = new Effect();
    kaleidoscope->setUniformFlowField(&amplifier.flowTexture);
    kaleidoscope->setupGui("Kaleidoscope", numChannels);
    kaleidoscope->loadShader("shaders/kaleidoscope2.frag");
    kaleidoscope->addUniformFloat("slide1X", "Slide1X", 0.0, 0.0, VID_WIDTH);
    kaleidoscope->addUniformFloat("slide1Y", "Slide1Y", 0.0, 0.0, VID_HEIGHT);
    kaleidoscope->addUniformFloat("slide2X", "Slide2X", 0.0, 0.0, VID_WIDTH);
    kaleidoscope->addUniformFloat("slide2Y", "Slide2Y", 0.0, 0.0, VID_HEIGHT);
    kaleidoscope->addUniformFloat("shiftX", "ShiftX", 0.0, 0.0, VID_WIDTH);
    kaleidoscope->addUniformFloat("shiftY", "ShiftY", 0.0, 0.0, VID_HEIGHT);
    kaleidoscope->addUniformFloat("sides", "Sides", 1.0, 0.0, 32.0);
    kaleidoscope->addUniformFloat("rotation", "Rotation", 0.0, 0.0, 1.0);
    kaleidoscope->addUniformFloat("size", "Size", 0.0, 0.0, 2.0);
    kaleidoscope->addUniformFloat("angle", "Angle", 0.0, 0.0, 1.0);

//    kaleidoscope->addUniformFloat("sections", "Sections", 5.0, 1.0, 100.0);
//    kaleidoscope->addUniformFloat("centerX", "in X", VID_WIDTH/2, 0.0, VID_WIDTH);
//    kaleidoscope->addUniformFloat("centerY", "in Y", VID_HEIGHT/2, 0.0, VID_HEIGHT);
//    kaleidoscope->addUniformFloat("outCenterX", "in X", VID_WIDTH/2, 0.0, VID_WIDTH);
//    kaleidoscope->addUniformFloat("outCenterY", "in Y", VID_HEIGHT/2, 0.0, VID_HEIGHT);

//    kaleidoscope->addUniformVectorArray("locs", (float *)&kaleidoscopeSeedLocs[0], kaleidoscopeSeedLocs.size());
//    kaleidoscope->addUniformVectorArray("normals", (float *)&kaleidoscopeSeedNormals[0], kaleidoscopeSeedNormals.size());
//    kaleidoscope->addUniformFloat("seedVelsMultiplier", "Speed Factor", 0.0, 0.0, 20.0);
//    kaleidoscope->addUniformVectorArray("vels", (float *)&voronoiSeedVels[0], voronoiSeedVels.size());
    kaleidoscope->setControllerNumber(num++);
    effects.push_back(kaleidoscope);
    
    //Pixels
    Effect* pixels = new Effect();
    pixels->setUniformFlowField(&amplifier.flowTexture);
    pixels->setupGui("Pixelation", numChannels);
    pixels->addUniformFloat("pixel_w", "Pixel Width", 15.0, 1.0, 100.0);
    pixels->addUniformFloat("pixel_h", "Pixel Height", 10.0, 1.0, 100.0);
    pixels->loadShader("shaders/pixelation.frag");
    pixels->setControllerNumber(num++);
    effects.push_back(pixels);
    
    //RGB Shift
    Effect* rgbShift = new Effect();
    rgbShift->setupGui("RGB Shift", numChannels);
    rgbShift->setUniformFlowField(&amplifier.flowTexture);
    rgbShift->addUniformFloat("amount", "Amount", 0.5, 0.0, 3.0);
    rgbShift->loadShader("shaders/rgbshift.frag");
    rgbShift->setControllerNumber(num++);
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
    scanLines->setControllerNumber(num++);
    effects.push_back(scanLines);
    
    //Sharpen
    Effect* sharpen = new Effect();
    sharpen->setupGui("Sharpen", numChannels);
    sharpen->setUniformFlowField(&amplifier.flowTexture);
    sharpen->addUniformFloat("amount", "Amount", 0.5, 0.0, 1.0);
    sharpen->addUniformFloat("theMix", "The Mix", 0.5, 0.0, 1.0);
    sharpen->addUniformFloat("weirdness", "Weirdness", 0.5, 0.0, 1.0);
    sharpen->loadShader("shaders/DummyFrag.glsl");
    sharpen->setControllerNumber(num++);
    effects.push_back(sharpen);
    
    //ColorMap
    Effect* colorMap = new Effect();
    ofImage* rampImg = new ofImage();
    rampImg->load("textures/ramp1.png");
    colorMap->setupGui("Color Map", numChannels);
    colorMap->setUniformFlowField(&amplifier.flowTexture);
    colorMap->addUniformFloat("amount", "Amount", 0.5, 0.0, 1.0);
    colorMap->addUniformFloat("timeRate", "Rate", 0.5, 0.1, 1.0);
    colorMap->setUniformFlowField(&amplifier.flowTexture);
    colorMap->setUniformImage("colorMap", rampImg);;
    colorMap->loadShader("shaders/colorMap.frag");
    colorMap->setControllerNumber(num++);
    effects.push_back(colorMap);
    
    //BadTv
    Effect* badTv = new Effect();
    badTv->setupGui("Bad TV", numChannels);
    badTv->setUniformFlowField(&amplifier.flowTexture);
    badTv->addUniformFloat("amount", "Amount", 0.5, 0.0, 1.0);
    badTv->addUniformFloat("speed", "Speed", 0.5, 0.0, 1.0);
    badTv->addUniformFloat("rollSpeed", "Roll Speed", 0.5, 0.0, 1.0);
    badTv->addUniformFloat("distortion", "Distortion 1", 5.0, 0.0, 10.0);
    badTv->addUniformFloat("distortion2", "Distortion 2", 0.5, 0.0, 1.0);
    badTv->loadShader("shaders/badtv.frag");
    badTv->setControllerNumber(num++);
    effects.push_back(badTv);
    
    //Voronoi
    Effect* voronoi = new Effect();
    voronoi->setupGui("Voronoi", numChannels);
    voronoi->setUniformFlowField(&amplifier.flowTexture);
    voronoi->addUniformFloat("numActiveSeeds", "Active Seeds", 200.0, 0.0, NUM_VORONOI_SEEDS);
    voronoi->addUniformVectorArray("locs", (float *)&voronoiSeedLocs[0], voronoiSeedLocs.size());
    voronoi->addUniformFloat("seedVelsMultiplier", "Speed Factor", 0.0, 0.0, 20.0);
    voronoi->loadShader("shaders/VoronoiFrag.glsl");
    voronoiNum = num - 1;
    voronoi->setControllerNumber(num++);
    effects.push_back(voronoi);
    
    //enDarken
    Effect* endarken = new Effect();
    endarken->setupGui("Endarken", numChannels);
    endarken->setUniformFlowField(&amplifier.flowTexture);
    endarken->addUniformFloat("darkness", "Darkness", 0.5, 0.5, 1.0);
    endarken->loadShader("shaders/endarkenFrag.glsl");
    effects.push_back(endarken);

    //explode
    Effect* explode = new Effect();
    explode->setupGui("Explode", numChannels);
    explode->setUniformFlowField(&amplifier.flowTexture);
    explode->addUniformFloat("noiseScale", "Scale", 500, 1.0, 1000);
    explode->addUniformFloat("x_Strength", "X Strength", 0, 0, 400);
    explode->addUniformFloat("y_Strength", "Y Strength", 0, 0, 400);
    explode->addUniformFloat("z_Strength", "Z Strength", 0, 0, 400);
    explode->addUniformFloat("speed", "Speed", 0.2, 0.0, 1.0);
    explode->loadShader("shaders/explode");
    explode->setControllerNumber(num++);
    effects.push_back(explode);
    endarken->setControllerNumber(num++);
    
//    //Emboss
//    Effect* emboss = new Effect();
//    emboss->setupGui("Emboss", numChannels);
//    emboss->setUniformFlowField(&amplifier.flowTexture);
//    emboss->addUniformFloat("amount", "Amount", 0.5, 0.0, 1.0);
//    emboss->addUniformFloat("intensity", "Intensity", 50.0, 1.0, 100.0);
//    emboss->addUniformFloat("colorization", "Colorization", 0.5, 0.0, 1.0);
//    emboss->addUniformFloat("flowEffect", "Flow Effect", 0.0, 0.0, 100.0);
//    emboss->loadShader("shaders/emboss.frag");
//    emboss->setControllerNumber(num++);
//    effects.push_back(emboss);

    //setup main gui
    //main.setup();
    main.add(camInput.set("Camera On", false));
    main.add(PartyOn.set("Party Mode", false));
    main.add(syphonOut.set("Syphon Output", false));
    main.add(flipInput.set("Mirror Input", false));
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
#ifdef USING_BLACKMAGIC
    cam.setup(1920, 1080, 30);
#else
    cam.initGrabber(VID_WIDTH, VID_HEIGHT);
#endif

    //allocate drawing fbo
    motionWarp.allocate(VID_WIDTH, VID_HEIGHT);
    finalMix.allocate(VID_WIDTH, VID_HEIGHT);
    mosaicDraw.allocate(VID_WIDTH, VID_HEIGHT);
    
    swapIn = new ofFbo();
    swapOut = new ofFbo();
    
    swapIn->allocate(VID_WIDTH, VID_HEIGHT);
    swapOut->allocate(VID_WIDTH, VID_HEIGHT);

    currImg.allocate(VID_WIDTH, VID_HEIGHT, OF_IMAGE_COLOR);
    
    contentManager = new ContentManager();
    contentManager->setup("movies/Press", "movies/Party", VID_WIDTH, VID_HEIGHT);
    contentManager->play();
    
    contentManager->frame.setEffects(&effects);
    contentManager->frame.setupGui("Right");
    contentManager->setAmplifier(&amplifier);
    contentManager->setCamera(&cam);
    contentManager->setSwapBuffers(swapIn, swapOut);
    contentManager->setTiler(mosaic);
    contentManager->setFlipInput(&flipInput);
    contentManager->frame.gui.setPosition(10, 10);
    contentManager->frame.Presets.setPosition(10, contentManager->frame.gui.getHeight() + 20);
    contentManager->frame.Particles.setPosition(10, HEIGHT - contentManager->frame.Particles.getHeight() - 10);
    
    midiManager = new MidiManager();
    midiManager->setupMidi();
    midiManager->setContentManager(contentManager);
    midiManager->setVideoTiler(mosaic);
    midiManager->setMotionAmplifier(&amplifier);
    
    fftCut.setup("FFT Cut", "settings/fftCut.xml");
    fftCut.add(upperCut.set("Upper", 10000.0, 0.0, 10000.0));
    fftCut.add(lowerCut.set("Lower", 0.0, 0.0, 10000.0));
    fftCut.setPosition(10, HEIGHT - 300 - endarken->gui.getHeight());
    
    mosaic->gui.setPosition(10, 675);
    
    activeBuffer = swapIn;
    
    texOutputToSyphon.setName("Screen Output");

}

//--------------------------------------------------------------
void ofApp::update(){
    for(int i=0; i < voronoiSeedLocs.size(); i++) {
        voronoiSeedLocs[i] += voronoiSeedVels[i] * effects[voronoiNum]->floatUniforms["seedVelsMultiplier"]->get();
        float projX = ofMap(voronoiSeedLocs[i].x, 0, VID_WIDTH, 1, VID_WIDTH-1, true);
        float projY = ofMap(voronoiSeedLocs[i].y, 0, VID_HEIGHT, 1, VID_HEIGHT-1, true);
        
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
    
    for(int i=0; i < kaleidoscopeSeedLocs.size(); i++) {
        ofVec2f val = ofVec2f(0.5 - ofNoise(ofGetElapsedTimef() + 100*i), 0.5 - ofNoise(ofGetElapsedTimef() + 50*i))*10;
        //kaleidoscopeSeedLocs[i] += val;
        kaleidoscopeSeedNormals[i] = val.normalize()*0.5;
    }
    
#ifdef USING_FFT
    vector<float> vals = fft.getFftPeakData();
    for(int i = 0; i < effects.size(); i++) {
        effects[i]->updateFromFFT(vals, upperCut, lowerCut);
        amplifier.updateFromFFT(vals, upperCut, lowerCut);
    }
    fft.update();
#else
    for(int i = 0; i < effects.size(); i++) {
        effects[i]->updateFromFloat(onset.novelty, upperCut, lowerCut);
    }
#endif
    
    contentManager->update(camInput);
#ifdef USING_FFT
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
#endif
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
    
//    finalMix.begin();
//        activeBuffer->draw(0, 0, VID_WIDTH, VID_HEIGHT);
//    finalMix.end();
//
    activeBuffer->readToPixels(currImg);
//    finalMix.readToPixels(currImg);
    currImg.update();
    mosaic->addImage(currImg);
    
//    if(camInput) {
        finalMix.begin();
            mosaic->drawWithTimeOffset(0, 0, VID_WIDTH, VID_HEIGHT);
        finalMix.end();
    
        if(syphonOut) {
            texOutputToSyphon.publishTexture(&(finalMix.getTexture()));
        } else {
            finalMix.draw(WIDTH, 0, OUT_WIDTH, OUT_HEIGHT);
        }
        finalMix.draw(ofGetScreenWidth()/2 - VID_WIDTH/2, HEIGHT/2 - VID_HEIGHT/2, VID_WIDTH, VID_HEIGHT);
    
        //dieselHashtag.draw(90, HEIGHT - dieselHashtag.getHeight() - 90, dieselHashtag.getWidth(), dieselHashtag.getHeight());
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
            
            fftCut.draw();
            
#ifdef USING_FFT
            int fftWidth = WIDTH/2;
            int fftHeight = 100;
            fft.draw(WIDTH/2 - fftWidth/2, HEIGHT - fftHeight, fftWidth, fftHeight);
            ofPushStyle();
            ofSetColor(255, 0, 0);
            float upperCutHeight = ofMap(upperCut, 0, 3, HEIGHT, HEIGHT - fftHeight*3);
            ofDrawLine(WIDTH/2 - fftWidth/2, upperCutHeight, WIDTH/2 + fftWidth/2, upperCutHeight);
            float lowerCutHeight = ofMap(lowerCut, 0, 3, HEIGHT, HEIGHT - fftHeight*3);
            ofSetColor(0, 0, 255);
            ofDrawLine(WIDTH/2 - fftWidth/2, lowerCutHeight, WIDTH/2 + fftWidth/2, lowerCutHeight);
            ofPopStyle();
#else
            ofPushStyle();
            ofSetColor(255);
            float rectWidth = ofMap(onset.novelty, 0, 10000, 0, VID_WIDTH, true);
            ofDrawRectangle(ofGetScreenWidth()/2 - VID_WIDTH/2, ofGetScreenHeight() - 100, rectWidth, 100);
            ofPopStyle();
            ofPushStyle();
            ofSetColor(255, 0, 0);
            float upperCutX = ofMap(upperCut, 0, 10000, WIDTH/2 - VID_WIDTH/2, WIDTH/2 + VID_WIDTH/2);
            ofDrawLine(upperCutX, HEIGHT, upperCutX, HEIGHT - 100);
            float lowerCutX = ofMap(lowerCut, 0, 10000, WIDTH/2 - VID_WIDTH/2, WIDTH/2 + VID_WIDTH/2);
            ofSetColor(0, 0, 255);
            ofDrawLine(lowerCutX, HEIGHT, lowerCutX, HEIGHT - 100);
            ofPopStyle();
#endif
            ofDrawBitmapStringHighlight(ofToString(ofGetFrameRate()), WIDTH - 100, HEIGHT - 20);
        }

        ofDrawBitmapStringHighlight(ofToString(ofGetFrameRate()), WIDTH - 100, HEIGHT - 20);
        ofDrawBitmapString(ofGetTimestampString("Time: %H : %M"), WIDTH - 200, 20);
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
//    if(key == '1') {
//        contentManager->frame.mesh.setMode(OF_PRIMITIVE_LINE_LOOP);
//    }
//    if(key == '2') {
//        contentManager->frame.mesh.setMode(OF_PRIMITIVE_LINES);
//    }
//    if(key == '2') {
//        contentManager->frame.mesh.setMode(OF_PRIMITIVE_TRIANGLES);
//    }
}

//--------------------------------------------------------------
void ofApp::onPartyModeChange(bool &b) {
    contentManager->changeMode(b);
}

//--------------------------------------------------------------
void ofApp::audioIn(float * input, int bufferSize, int nChannels) {
#ifndef USING_FFT
    onset.audioIn(input, bufferSize, nChannels);
#endif
}

//--------------------------------------------------------------
void ofApp::exit() {
    ofSoundStreamClose();
#ifdef USING_BLACKMAGIC
    cam.close();
#endif
    
}

