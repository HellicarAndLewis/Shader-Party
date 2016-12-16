//
//  videoTiler.cpp
//  ShaderParty
//
//  Created by James Bentley on 1/4/16.
//
//

#include "videoTiler.h"
#define MAX_IMAGES 25

videoTiler::videoTiler(int width, int height, int depth) {
    slitScan.setup(width, height, depth);
    slitScan.setTimeDelayAndWidth(0, depth);
    gui.setup("Tiling", "settings/Tiler.xml");
    gui.add(size.set("size", 1, 1, 5));
    //gui.setPosition(ofGetScreenWidth()/2, ofGetScreenHeight()/2);
    gui.loadFromFile("settings/Tiler.xml");
}

void videoTiler::addImage(ofBaseHasPixels& image) {
    slitScan.addImage(image);
}
void videoTiler::addImage(ofPixels& image) {
    slitScan.addImage(image);
    
}
void videoTiler::addImage(unsigned char* image){
    slitScan.addImage(image);
}

void videoTiler::drawWithTimeOffset(int xOffset, int yOffset, int width, int height) {
    img.allocate(slitScan.getWidth(), slitScan.getHeight(), OF_IMAGE_COLOR_ALPHA);
    for(int i = 0; i < size*size; i++) {
        slitScan.pixelsForFrame(ofMap(i, 0, size*size, slitScan.getCapacity()-1, 0), img.getPixels().getData());
        int x = i % size;
        int y = (i - x)/size;
        img.draw(xOffset + x*width/size-1, yOffset + y*height/size-1, width/size+2, height/size+2);
    }
}

void videoTiler::draw(ofFbo* fbo, int x, int y, int width, int height) {
    for(int i = 0; i < size*size; i++) {
        int x = i % size;
        int y = (i - x)/size;
        fbo->draw(x*width/size-1, y*height/size-1, width/size+2, height/size+2);
    }
}

void videoTiler::drawGui() {
    gui.draw();
}
