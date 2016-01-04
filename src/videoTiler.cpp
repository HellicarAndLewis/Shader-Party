//
//  videoTiler.cpp
//  ShaderParty
//
//  Created by James Bentley on 1/4/16.
//
//

#include "videoTiler.h"
#define MAX_IMAGES 36

videoTiler::videoTiler(int width, int height, int depth) {
    slitScan.setup(width, height, depth);
    slitScan.setTimeDelayAndWidth(0, depth);
    gui.setup("Tiling");
    gui.add(numRows.set("numRows", 1, 1, 6));
    gui.add(numCols.set("numCols", 1, 1, 6));
    gui.add(size.set("size", 1, 1, 6));
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

void videoTiler::draw(int x, int y, int width, int height) {
    img.allocate(slitScan.getWidth(), slitScan.getHeight(), OF_IMAGE_COLOR_ALPHA);
    for(int i = 0; i < size*size; i++) {
        slitScan.pixelsForFrame(ofMap(i, 0, size*size, slitScan.getCapacity()-1, 0), &img);
        int x = i % size;
        int y = (i - x)/size;
        img.draw(x*width/size, y*height/size, width/size, height/size);
    }
}

void videoTiler::drawGui() {
    gui.draw();
}