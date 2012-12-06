#pragma once

#include "ofMain.h"
#include "ofxTriangleMesh.h"

#include "ofxOpenCv.h"

#define CAM_WIDTH      640
#define CAM_HEIGHT     480
class testApp : public ofBaseApp{
	
public:
	void setup();
	void update();
	void draw();
	
	void keyPressed  (int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);
	ofxTriangleMesh mesh;
	
	ofVideoGrabber          vidGrabber;
	ofxCvColorImage         colorImage;
	ofxCvGrayscaleImage     threImg;
	ofxCvGrayscaleImage     bgImg;
	
	ofxCvContourFinder      contourFinder;
	
	int                     threshold;
	bool                    bLearnBackground;
	
};
