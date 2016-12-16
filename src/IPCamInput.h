//
//  IPCamInput.h
//  show
//
//  Created by Chris Mullany on 02/09/2015.
//
//

#pragma once
#include "ofMain.h"
#include "IVisionInput.h"
#include "IPVideoGrabber.h"

#define CAM_URL "http://192.168.1.149/axis-cgi/mjpg/video.cgi"

class IPCameraDef
{
public:
    IPCameraDef()
    {
    }
    
    IPCameraDef(const std::string& url): _url(url)
    {
    }
    
    IPCameraDef(const std::string& name,
                const std::string& url,
                const std::string& username,
                const std::string& password):
    _name(name),
    _url(url),
    _username(username),
    _password(password)
    {
    }
    
    void setName(const std::string& name) { _name = name; }
    std::string getName() const { return _name; }
    
    void setURL(const std::string& url) { _url = url; }
    std::string getURL() const { return _url; }
    
    void setUsername(const std::string& username) { _username = username; }
    std::string getUsername() const { return _username; }
    
    void setPassword(const std::string& password) { _password = password; }
    std::string getPassword() const { return _password; }
    
    
private:
    std::string _name;
    std::string _url;
    std::string _username;
    std::string _password;
};
using ofx::Video::IPVideoGrabber;

// IP/network camera grabber
// Uses the IPVideoGrabber ofxaddon
//
class IPCamInput : public IVisionInput {
public:
    IPCamInput();
    
    void setup();
    void update();
    void draw();
    void exit();
    ofPixelsRef getPixelsRef();
    bool isFrameNew();
    bool getIsReady();
    void load(string url = CAM_URL);
    void start();
    void stop();
    shared_ptr<IPVideoGrabber> grabber;
    
protected:
private:
    void loadCamera();
    IPCameraDef& getCamera();
    IPCameraDef ipcam;
    bool hasImg;
    
};
