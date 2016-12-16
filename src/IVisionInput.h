#pragma once
#include "ofMain.h"

class IVisionInput {
public:
    virtual void setup() = 0;
    virtual void update() = 0;
    virtual void draw() = 0;
    virtual ofPixelsRef getPixelsRef() = 0;
    virtual bool isFrameNew() = 0;
    virtual bool getIsReady() = 0;
    virtual void start() = 0;
    virtual void stop() = 0;
};
