#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

    
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){

    
    ofBackgroundGradient(ofColor::white, ofColor(200,200,200), OF_GRADIENT_LINEAR);
    
    line.draw();
    
    
    mesh.draw();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    
    
    line.addVertex(ofPoint(x, y));
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    line.clear();
    line.addVertex(ofPoint(x, y));
    
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

    if (line.size() > 2){
        
        ofPolyline lineRespaced = line;
        
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
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
