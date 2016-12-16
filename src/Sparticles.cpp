/**
 * == Simplified BSD *** MODIFIED FOR NON-COMMERCIAL USE ONLY!!! *** ==
 * Copyright (c) 2011, Cariad Interactive LTD
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification, are permitted
 * provided that the following conditions are met:
 *
 *   * Redistributions of source code must retain the above copyright notice, this list of
 *     conditions and the following disclaimer.
 *
 *   * Redistributions in binary form must reproduce the above copyright notice, this list of
 *     conditions and the following disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *
 *   * Any redistribution, use, or modification is done solely for personal benefit and not for any
 *     commercial purpose or for monetary gain
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY
 * WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#include "Sparticles.h"
//#include "Settings.h"

//#include "ColorPicker.h"

Sparticles::Sparticles(){
    
    pos = 0;
    maxAge = 50;
    numParticles = 1000;
    positions = new ofVec2f[numParticles];
    velocities = new ofVec2f[numParticles];
    ages = new int[numParticles];
    sizes = new float[numParticles];
    
    for(int i = 0; i < numParticles; i++) {
        positions[i] = ofVec2f(-10, -10);
        velocities[i] = ofVec2f(0, 0);
        ages[i] = maxAge;
        sizes[i] = ofRandom(5, 26);
    }
    
    int i = 0;
    colors[i++] = 0xCBCEE1;
    colors[i++] = 0xDCC8CC;
    colors[i++] = 0xEDB451;
    colors[i++] = 0xACC5D6;
    colors[i++] = 0xB6D597;
    colors[i++] = 0xE0CACB;

//    colors[i++] = 0x0E2356;
//    colors[i++] = 0x74AF27;
//    colors[i++] = 0xFFEC00;
//    colors[i++] = 0xED6B06;
//    colors[i++] = 0x6D1B00;
//    colors[i++] = 0xA80F79;
//    colors[i++] = 0xE7378C;
//    colors[i++] = 0x0070BA;
//    colors[i++] = 0xC0D886;
//    colors[i++] = 0xF0E500;
//    colors[i++] = 0xF0856A;
//    colors[i++] = 0xE42313;
//    colors[i++] = 0xF1DCED;
//    colors[i++] = 0xF6B6CB;
//    colors[i++] = 0xBDE3F2;
//    colors[i++] = 0xF9F3B2;
//    colors[i++] = 0xFFFADE;
//    colors[i++] = 0xF8E7E2;
//    colors[i++] = 0;
}

void Sparticles::setup() {
    
    vector<string> imgs;
    imgs.push_back("textures/hashtag_005.png");
    imgs.push_back("textures/hashtag_001.png");
    imgs.push_back("textures/hashtag_002.png");
    imgs.push_back("textures/hashtag_003.png");
    imgs.push_back("textures/hashtag_004.png");

    //imgs.push_back("textures/glitter.png");
    //imgs.push_back("textures/sparkle.png");
    //imgs.push_back("textures/star.png");
    for(int i = 0; i < imgs.size(); i++) {
        images.push_back(ofImage());
        images.back().loadImage(imgs[i]);
        images.back().setAnchorPercent(0.5, 0.5);
    }
}


Sparticles::~Sparticles(){
    delete [] positions;
    delete [] velocities;
    delete [] ages;
    delete [] sizes;
}


void Sparticles::update(){
    for(int i = 0; i < numParticles; i++) {
        ages[i]++;
        positions[i] += velocities[i];
    }
}

void Sparticles::draw(bool multicoloured){
    ofPushStyle();
    //int colorIndex = Settings::getInstance()->settings["fgColor"];
    ofColor c;
    //if(colorIndex!=20) {
        //c = ofColor::fromHex(colors[colorIndex]);
    //}
    //    images[0].draw(500, 500);
    ofEnableAlphaBlending();
    glBlendFunc(GL_SRC_ALPHA,GL_ONE);
    for(int i = 0; i < numParticles; i++) {
        if(ages[i]<maxAge) {
            float size = ofMap(ages[i], 0, maxAge, sizes[i], 0);
            
            //            if (particleDisplayMode == 2){
//                        ofTranslate(1, 1, -size);
            //            }
            //            else {
//                         ofTranslate(1, 1, 1);
            //            }
            //ofTranslate(1, 1, ages[i]/10); ///moves them in 3D
            //if(colorIndex==20) {
            c.setHex(colors[i%6]);
            //}
            ofSetColor(c.r, c.g, c.b, ofMap(ages[i], 0, maxAge, 255, 0) );
            ofRotate(180, 0, 1, 0);
//            ofCircle(positions[i].x - size/2, positions[i].y - size/2, size);
            if(ofRandom(0, 1)>0.90) {
                int r = 1+rand()%(images.size()-1);
                images[r].draw(positions[i].x, positions[i].y, size, size);
            } else {
                images[0].draw(positions[i].x, positions[i].y, size, size);
            }
            //            ofRect(positions[i].x - size/2, positions[i].y - size/2, size, size);
        }
    }
    ofPopStyle();
}

void Sparticles::spawn(float x, float y, float dx, float dy){
    ages[pos] = 0;
    positions[pos] = ofVec2f(x, y);
    velocities[pos] = ofVec2f(dx, dy);
    
    pos++;
    pos %= numParticles;
}