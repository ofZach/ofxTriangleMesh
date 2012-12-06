#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
    colorImage.allocate(CAM_WIDTH, CAM_HEIGHT);
    threImg.allocate(CAM_WIDTH, CAM_HEIGHT);
    bgImg.allocate(CAM_WIDTH, CAM_HEIGHT);
	
	vidGrabber.setDeviceID(0);
    vidGrabber.initGrabber(CAM_WIDTH, CAM_HEIGHT);
	
	threshold = 20;
	bLearnBackground = true;
}

//--------------------------------------------------------------
void testApp::update(){
	vidGrabber.update();
	
	if (vidGrabber.isFrameNew()){
        colorImage.setFromPixels(vidGrabber.getPixels(), CAM_WIDTH, CAM_HEIGHT);
//        colorImage.mirror(false, true);
		
        if(bLearnBackground) {
            bgImg = colorImage;
            bLearnBackground = false;
        }
		
        threImg = colorImage;
        threImg.absDiff(bgImg);
        threImg.blur(5);
        threImg.threshold(threshold);
		
        contourFinder.findContours(threImg, 20, (CAM_WIDTH*CAM_HEIGHT)/3, 5, false);
		
		if(contourFinder.nBlobs>0)
		{
			int i = 0;
			ofPolyline lineRespaced;
			for(int j = 0 ; j < contourFinder.blobs[i].pts.size() ; j++)
			{
				lineRespaced.addVertex(contourFinder.blobs[i].pts[j].x,contourFinder.blobs[i].pts[j].y);
			}
			lineRespaced.addVertex(lineRespaced[0]);
			
			lineRespaced = lineRespaced.getResampledBySpacing(20);
			// I want to make sure the first point and the last point are not the same, since triangle is unhappy:
			lineRespaced.getVertices().erase(lineRespaced.getVertices().begin());
			// if we have a proper set of points, mesh them:
			if (lineRespaced.size() > 5){
				
				// angle constraint = 28
				// size constraint = -1 (don't constraint triangles by size);
				
				//mesh.triangulate(lineRespaced, 28, -1);
				
				// this is an alternative, constrain on size not angle:
				mesh.triangulate(lineRespaced, -1, 200);
				int num = mesh.triangulatedMesh.getNumVertices();
				for (int j = 0 ;  j < num ; j++)
				{
					ofVec2f v2= mesh.triangulatedMesh.getVertex(j);
					mesh.triangulatedMesh.addTexCoord(v2);
					
				}
				for(int i = 0 ; i <mesh.triangles.size() ; i++)
				{
					ofVec2f v2 = mesh.triangles[i].pts[0];
					mesh.triangles[i].randomColor = vidGrabber.getPixelsRef().getColor(v2.x, v2.y);
					
				}
				
				// see ofxTriangleMesh.h for info.
				
			}
			
        }
		
	}
}

//--------------------------------------------------------------
void testApp::draw(){
	ofBackgroundGradient(ofColor::white, ofColor::gray);
	ofBackgroundGradient(ofColor::white, ofColor(200,200,200), OF_GRADIENT_LINEAR);
	
	
	ofSetColor(255, 255, 255);
	
	vidGrabber.draw(0, 0);
	

	
	mesh.draw();
	ofPushMatrix();
	ofTranslate(CAM_WIDTH, 0);
	ofPushStyle();
	ofSetColor(ofColor::white);
	vidGrabber.getTextureReference().bind();
	mesh.triangulatedMesh.drawFaces();
	vidGrabber.getTextureReference().unbind();
	ofPopStyle();
	
	ofPushStyle();
	ofSetColor(ofColor::black);
	mesh.triangulatedMesh.drawWireframe();
	ofPopStyle();
	
	ofPushStyle();
	ofSetColor(ofColor::gray);
	mesh.triangulatedMesh.drawVertices();
	ofPopStyle();
	ofPopMatrix();
	
	ofPushMatrix();
	ofTranslate(CAM_WIDTH*2, 0);
	for (int i=0; i<mesh.nTriangles; i++){
		
        ofFill();
        ofSetColor( mesh.triangles[i].randomColor);
        ofTriangle( mesh.outputPts[mesh.triangles[i].index[0]],
				   mesh.outputPts[mesh.triangles[i].index[1]],
                   mesh.outputPts[mesh.triangles[i].index[2]]);
        
    }
	ofPopMatrix();
    // Infos
    ofSetColor(0, 0, 0);
	
    ofDrawBitmapString("FPS : " + ofToString(ofGetFrameRate()), 10, CAM_HEIGHT + 20);

	
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	switch (key) {
		case ' ':
			bLearnBackground = true;
			break;
		case 's':
			vidGrabber.videoSettings();
			break;
	}
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){
	
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
	
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
	threshold = x;
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
	
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