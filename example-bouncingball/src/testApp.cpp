#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	for(int i = 0 ; i < 10 ; i++)
	{
	Ball* ball = new Ball(ofRandomWidth(), ofRandomHeight()	, ofGetWidth(),ofGetHeight());
    balls.push_back(ball);
	}
}

//--------------------------------------------------------------
void testApp::update(){

}

//--------------------------------------------------------------
void testApp::draw(){
	ofBackgroundGradient(ofColor::white, ofColor::gray);
	ofBackgroundGradient(ofColor::white, ofColor(200,200,200), OF_GRADIENT_LINEAR);
    
    for (int i = 0; i < balls.size(); i++) {
        balls[i]->calc();
        balls[i]->draw();
    }
    
    if (balls.size() > 2){
        
        ofPolyline lineRespaced;
		for (int i = 0; i < balls.size(); i++) {
			lineRespaced.addVertex(ofVec2f(balls[i]->x,balls[i]->y));
		}
        
        // add the last point (so when we resample, it's a closed polygon)
        lineRespaced.addVertex(lineRespaced[0]);
        // resample
        lineRespaced = lineRespaced.getResampledBySpacing(20);
        // I want to make sure the first point and the last point are not the same, since triangle is unhappy:
        lineRespaced.getVertices().erase(lineRespaced.getVertices().begin());
        // if we have a proper set of points, mesh them:
        if (lineRespaced.size() > 5){
            
            // angle constraint = 28
            // size constraint = -1 (don't constraint triangles by size);
            
            mesh.triangulate(lineRespaced, 28, -1);
            
            
            // this is an alternative, constrain on size not angle:
            //mesh.triangulate(lineRespaced, -1, 200);
            
            // see ofxTriangleMesh.h for info.
            
        }
    }
    mesh.draw();
	
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
	
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
	Ball* ball = new Ball(x, y, ofGetWidth(),ofGetHeight());
	balls.push_back(ball);
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
	

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}