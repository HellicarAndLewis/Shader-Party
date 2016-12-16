#include "IPCamInput.h"

IPCamInput::IPCamInput() {
}

void IPCamInput::setup() {
}

void IPCamInput::update() {
    if (grabber) {
        grabber->update();
        if(grabber->isFrameNew() && !hasImg) hasImg = true;
    }
}

void IPCamInput::draw() {
    if (grabber) {
        ofSetColor(255);
        float w = ofGetWidth()/2;
        float h = grabber->getHeight() * (w / grabber->getWidth());
        grabber->draw(0, 0, w, h);
    }
}

void IPCamInput::exit() {
}

//////////////////////////////////////////////////////////////////////////////////
// public
//////////////////////////////////////////////////////////////////////////////////

void IPCamInput::load(string url){
    ipcam = IPCameraDef("name", url, "", "");
    IPCameraDef& cam = getCamera();
    auto c = std::make_shared<IPVideoGrabber>();
    c->setCameraName(cam.getName());
    c->setURI(cam.getURL());
    grabber = c;
    hasImg = false;
    //start();
}

ofPixelsRef IPCamInput::getPixelsRef() {
    if (!getIsReady()) ofLogWarning() << "IPCamInput::getPixelsRef grabber is not ready";
    return grabber->getFrame()->getPixels();
}

bool IPCamInput::isFrameNew() {
    return grabber->isFrameNew();
}

bool IPCamInput::getIsReady() {
    return (hasImg && grabber->isConnected());
}

void IPCamInput::start(){
    ofLogNotice() << "IPCamInput::start";
    if (grabber) grabber->connect();
}

void IPCamInput::stop(){
    ofLogNotice() << "IPCamInput::stop";
    if (grabber) grabber->disconnect();
    hasImg = false;
}

//////////////////////////////////////////////////////////////////////////////////
// protected
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
// private
//////////////////////////////////////////////////////////////////////////////////

IPCameraDef& IPCamInput::getCamera() {
    return ipcam;
}

void IPCamInput::loadCamera() {
    
}

//////////////////////////////////////////////////////////////////////////////////
// custom event handlers
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
// oF event handlers
//////////////////////////////////////////////////////////////////////////////////
