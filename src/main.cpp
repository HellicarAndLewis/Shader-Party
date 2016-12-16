#include "ofMain.h"
#include "ofApp.h"
#include "ofAppGLFWWindow.h"

//========================================================================
int main( ){
    // setup window
    ofAppGLFWWindow window;
    window.setMultiDisplayFullscreen(true);
    //if (fullScreen) ofSetupOpenGL(&window, width, height, OF_FULLSCREEN);
    ofSetupOpenGL(&window, 1024, 768, OF_FULLSCREEN);
	//ofSetupOpenGL(1024,768,OF_WINDOW);			// <-------- setup the GL context

	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
	ofRunApp(new ofApp());

}
