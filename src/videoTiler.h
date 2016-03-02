//
//  videoTiler.h
//  ShaderParty
//
//  Created by James Bentley on 1/4/16.
//
//

#ifndef __ShaderParty__videoTiler__
#define __ShaderParty__videoTiler__

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxSlitScan.h"

class videoTiler {
public:
    ofxPanel gui;
    ofParameter<int> size;
    ofImage img;
    
    ofxSlitScan slitScan;
    
    videoTiler(int width, int height, int depth);
    void drawWithTimeOffset(int x, int y, int fullWidth, int fullHeight);
    void draw(ofFbo* fbo, int x, int y, int fullWidth, int fullHeight);
    void drawGui();
    void onSizeChange();
    void addImage(ofBaseHasPixels& image);
    void addImage(ofPixels& image);
    void addImage(unsigned char* image);
    
};

#endif /* defined(__ShaderParty__videoTiler__) */
