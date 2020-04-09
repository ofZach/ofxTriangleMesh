#pragma once

#include "ofMain.h"

#include "ofxGui.h"
#include "ofxTriangleMesh.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

 private:
 		enum TriangulationType {
			kFirstTriangulationType = 0,
			ORIGINAL = kFirstTriangulationType,
			SIMPLE,
			CONVEX_HULL,
			DELAUNAY,
			DELAUNAY_CONSTRAINED,
			kNumTriangulationType
		};

		static constexpr char* sTriangulationTypes[kNumTriangulationType] = {
			(char*)"Original",
			(char*)"Simplest",
			(char*)"Convex Hull",
			(char*)"Delaunay",
			(char*)"Delaunay constrained"
		};

		void ReadPolyFile(const string& filename);

		// GUI
    ofxPanel gui_;
		ofxToggle vertexToggle_;
		ofxToggle contourToggle_;
		ofxToggle faceToggle_;
		ofxToggle voronoiToggle_;
		ofxToggle convexToggle_;
		ofxSlider<int> typeSlider_;
		ofxLabel typeLabel_;

		// Polygon geometrical datas.
		std::vector<ofPoint> vertices_;
		std::vector<glm::ivec2> segments_;
		std::vector<ofPoint> holes_;

		// Generated at import.
		std::vector<ofPolyline*> polylines_;
		ofPolyline convexHull_;
		ofPoint center_;

		// Triangulated mesh.
		ofxTriangleMesh trimeshes_[kNumTriangulationType];
		ofxTriangleMesh *trimesh_current_;
};
