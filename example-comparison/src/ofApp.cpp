#include "ofApp.h"

#include <cstdio>
#include <cstdlib>

constexpr char* ofApp::sTriangulationTypes[kNumTriangulationType];

//--------------------------------------------------------------
void ofApp::setup()
{
  gui_.setup();
  gui_.add(typeSlider_.setup("", 
    TriangulationType::DELAUNAY_CONSTRAINED, kFirstTriangulationType, kNumTriangulationType-1
  ));
  gui_.add(vertexToggle_.setup("Original vertices", true));
  gui_.add(contourToggle_.setup("Contour", true));
  gui_.add(faceToggle_.setup("Faces", true));
  gui_.add(convexToggle_.setup("Convex hull", true));
  gui_.add(voronoiToggle_.setup("Voronoi diagram", true));

#if 1

  // Read from a 'Triangle' poly file.
  ReadPolyFile("data/A.poly");

#else

  // Example with custom shape

  vertices_.push_back(ofPoint(0, 0));
  vertices_.push_back(ofPoint(ofGetWidth(), 0));
  vertices_.push_back(ofPoint(ofGetWidth(), ofGetHeight()));
  vertices_.push_back(ofPoint(0, ofGetHeight()));

  float midx = ofGetWidth() / 2.0f;
  float midy = ofGetHeight() / 2.0f;
  float side = ofGetWidth() / 6.0f;

  vertices_.push_back(ofPoint(midx-side, midy-side));
  vertices_.push_back(ofPoint(midx+side, midy-side));
  vertices_.push_back(ofPoint(midx+side, midy+side));
  vertices_.push_back(ofPoint(midx-side, midy+side));

  segments_.push_back(glm::ivec2(0, 1));
  segments_.push_back(glm::ivec2(1, 2));
  segments_.push_back(glm::ivec2(2, 3));
  segments_.push_back(glm::ivec2(3, 0));
  segments_.push_back(glm::ivec2(4, 5));
  segments_.push_back(glm::ivec2(5, 6));
  segments_.push_back(glm::ivec2(6, 7));
  segments_.push_back(glm::ivec2(7, 4));

  holes_.push_back(ofPoint(midx, midy));

#endif

  // Extract contours as polylines
  int lastIndex = -1;
  ofPolyline *pl = nullptr;
  for (const auto& segment : segments_)
  {
    if (lastIndex != segment.x)
    {
      pl = new ofPolyline();
      pl->addVertex( vertices_[segment.x] );
      polylines_.push_back(pl);
    }
    lastIndex = segment.y;
    pl->addVertex( vertices_[lastIndex] );
  }
  ofPolyline *contour = polylines_[0];

  // Center of gravity.
  center_ = contour->getCentroid2D();

  // Generate a convex hull for the base contour.
  ofxTriangleMesh::QuickHull(*contour, convexHull_);

  // Triangulate the shape.
  trimeshes_[ORIGINAL].triangulate(*contour);
  trimeshes_[SIMPLE].triangulate(vertices_, segments_, holes_);
  trimeshes_[CONVEX_HULL].triangulateConvexHull(vertices_);
  trimeshes_[DELAUNAY].triangulateDelaunay(vertices_, segments_, holes_);

  /// @note
  /// Should the need arises to send a buffer a different vector-type
  /// we could hack our way like this :
  //#define castVec ofxTriangleMesh::CastVector
  //trimesh.triangulateConvexHull(castVec(contour->getVertices()));

  // Generate the associated Voronoi diagrams.
  for (auto &trimesh : trimeshes_) {
    trimesh.generateVoronoiDiagram();
  }  
}

//--------------------------------------------------------------
void ofApp::update()
{
  const float dx = ofMap(ofGetMouseX(), 0.0f, ofGetWidth(), 0.0f, 1.0f);
  const float dy = ofMap(ofGetMouseY(), 0.0f, ofGetHeight(), 0.0f, 1.0f);

  // Update the current triangle mesh to display.
  const int typeId = typeSlider_;
  typeSlider_.setName(sTriangulationTypes[typeId]);
  trimesh_current_ = &trimeshes_[typeId];

  // When use Delaunay Constrained updates its constraints based
  // on mouse coordinates.
  if (typeId == DELAUNAY_CONSTRAINED)
  {
    const float angleConstraint = ofLerp(10.0f, 30.0f, dx);
    const float areaConstraint  = ofLerp(20.0f, 1000.0f, dy);
    trimesh_current_->triangulateConstrainedDelaunay( vertices_, segments_, holes_, angleConstraint, areaConstraint);
    trimesh_current_->generateVoronoiDiagram();
  }
}

//--------------------------------------------------------------
void ofApp::draw()
{
  ofPushMatrix();
  {
    // Center the mesh on the screen.
    ofTranslate( 
      ofGetWidth()/2 - center_.x,
      ofGetHeight()/2 - center_.y,
      0.0f 
    );

    // Faces
    if (faceToggle_) {
      trimesh_current_->draw();
    }

    // Vertices
    if (vertexToggle_) {
      ofSetColor(120, 255, 120);
      for (auto &vertex : vertices_) {
        ofDrawCircle( vertex.x, vertex.y, 2);
      }
    }

    // Contours
    if (contourToggle_) {
      // Segments
      ofSetColor(120, 120, 255);
      for (auto *poly : polylines_) {
        poly->draw();
      }
      // Holes
      ofSetColor(255, 120, 120);
      for (auto hole : holes_) {
        ofDrawCircle( hole.x, hole.y, 2);
      }
    }

    // ConvexHull
    if (convexToggle_) {
      ofSetColor(0, 255, 0);
      convexHull_.draw();
    }

    // Voronoi Diagram
    if (voronoiToggle_) {
      ofSetColor(255, 80, 80);
      trimesh_current_->drawVoronoi();
    }
  }
  ofPopMatrix();

  gui_.draw();
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

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

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

//--------------------------------------------------------------
void ofApp::ReadPolyFile(const string& filename) 
{
  /// Simplified Triangle's .poly file reader.

  FILE *fd(nullptr);
  if (nullptr == (fd = fopen(filename.c_str(), "r"))) {
    fprintf(stderr, "Unable to found file \"%s\". Exiting..\n", filename.c_str());
    ofExit();
  }

  const float scaleFactor = std::min(ofGetWidth(), ofGetHeight());

  // Read vertices
  int numVertices, dummy;
  fscanf(fd, "%d 2 %d %d\n", &numVertices, &dummy, &dummy);
  vertices_.resize(numVertices);
  for (int i=0; i<numVertices && !feof(fd); ++i) 
  {
    auto &pt = vertices_[i];
    fscanf(fd, "%d %f %f %f\n", &dummy, &pt.x, &pt.y, &pt.z);
    pt.x *= +scaleFactor;
    pt.y *= -scaleFactor;
  }

  // Read segments
  int numSegments;
  fscanf(fd, "%d 0\n", &numSegments);
  segments_.resize(numSegments);
  for (int i=0; i<numSegments && !feof(fd); ++i)
  {
    auto &s = segments_[i];
    fscanf(fd, "%d %d %d\n", &dummy, &s.x, &s.y);
    s.x -= 1;
    s.y -= 1;
  }
  
  // Read holes
  int numHoles;
  fscanf(fd, "%d\n", &numHoles);
  holes_.resize(numHoles);
  for (int i=0; i<numHoles && !feof(fd); ++i) 
  {
    auto &pt = holes_[i];
    fscanf(fd, "%d %f %f\n", &dummy, &pt.x, &pt.y);
    pt.x *= +scaleFactor;
    pt.y *= -scaleFactor;
  }

  fclose(fd);

  /*
  // [redundant with ofPolyline::getCentroid2D() but nice to keep]
  // Calculate center of gravity from bounding box.
  auto resultX = std::minmax_element(vertices_.begin(), vertices_.end(), 
    [](const auto &a, const auto &b) {return (a.x < b.x);}
  );
  auto resultY = std::minmax_element(vertices_.begin(), vertices_.end(), 
    [](const auto &a, const auto &b) {return (a.y < b.y);}
  );
  center_.x = 0.5f*(resultX.first->x + resultX.second->x);
  center_.y = 0.5f*(resultY.first->y + resultY.second->y);
  */
}

