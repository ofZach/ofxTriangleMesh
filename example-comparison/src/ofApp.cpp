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
  ReadPolyFile( ofToDataPath("A.poly") );

#else

  // Example with custom shape

  polygon_.points.push_back(ofPoint(0, 0));
  polygon_.points.push_back(ofPoint(ofGetWidth(), 0));
  polygon_.points.push_back(ofPoint(ofGetWidth(), ofGetHeight()));
  polygon_.points.push_back(ofPoint(0, ofGetHeight()));

  float midx = ofGetWidth() / 2.0f;
  float midy = ofGetHeight() / 2.0f;
  float side = ofGetWidth() / 6.0f;

  polygon_.points.push_back(ofPoint(midx-side, midy-side));
  polygon_.points.push_back(ofPoint(midx+side, midy-side));
  polygon_.points.push_back(ofPoint(midx+side, midy+side));
  polygon_.points.push_back(ofPoint(midx-side, midy+side));

  polygon_.segments.push_back(glm::ivec2(0, 1));
  polygon_.segments.push_back(glm::ivec2(1, 2));
  polygon_.segments.push_back(glm::ivec2(2, 3));
  polygon_.segments.push_back(glm::ivec2(3, 0));
  polygon_.segments.push_back(glm::ivec2(4, 5));
  polygon_.segments.push_back(glm::ivec2(5, 6));
  polygon_.segments.push_back(glm::ivec2(6, 7));
  polygon_.segments.push_back(glm::ivec2(7, 4));

  polygon_.holes.push_back(ofPoint(midx, midy));

#endif

  // Extract contours as polylines
  int lastIndex = -1;
  ofPolyline *pl = nullptr;
  for (const auto& segment : polygon_.segments)
  {
    if (lastIndex != segment.x)
    {
      pl = new ofPolyline();
      pl->addVertex( polygon_.points[segment.x] );
      polylines_.push_back(pl);
    }
    lastIndex = segment.y;
    pl->addVertex( polygon_.points[lastIndex] );
  }
  ofPolyline *contour = polylines_[0];

  // Center of gravity.
  center_ = contour->getCentroid2D();

  // Generate a convex hull for the base contour.
  ofxTriangleMesh::QuickHull(*contour, convexHull_);

  // Triangulate the shape.
  trimeshes_[ORIGINAL].triangulate(*contour);
  trimeshes_[SIMPLE].triangulateSimple(polygon_);
  trimeshes_[CONVEX_HULL].triangulateConvexHull(polygon_.points);
  trimeshes_[DELAUNAY].triangulateDelaunay(polygon_);

  /// @note
  /// Should the need arises to send a buffer of a different vector-type
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
    trimesh_current_->triangulate( polygon_, angleConstraint, areaConstraint);
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
      trimesh_current_->draw(bUseDebugColor);
    }

    // Vertices
    if (vertexToggle_) {
      ofSetColor(120, 255, 120);
      for (auto &vertex : polygon_.points) {
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
      for (auto hole : polygon_.holes) {
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
  if (key == 'd') {
    bUseDebugColor ^= true;
  }
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
  polygon_.points.resize(numVertices);
  for (int i=0; i<numVertices && !feof(fd); ++i) 
  {
    auto &pt = polygon_.points[i];
    fscanf(fd, "%d %f %f %f\n", &dummy, &pt.x, &pt.y, &pt.z);
    pt.x *= +scaleFactor;
    pt.y *= -scaleFactor;
  }

  // Read segments
  int numSegments;
  fscanf(fd, "%d 0\n", &numSegments);
  polygon_.segments.resize(numSegments);
  for (int i=0; i<numSegments && !feof(fd); ++i)
  {
    auto &s = polygon_.segments[i];
    fscanf(fd, "%d %d %d\n", &dummy, &s.x, &s.y);
    s.x -= 1;
    s.y -= 1;
  }
  
  // Read holes
  int numHoles;
  fscanf(fd, "%d\n", &numHoles);
  polygon_.holes.resize(numHoles);
  for (int i=0; i<numHoles && !feof(fd); ++i) 
  {
    auto &pt = polygon_.holes[i];
    fscanf(fd, "%d %f %f\n", &dummy, &pt.x, &pt.y);
    pt.x *= +scaleFactor;
    pt.y *= -scaleFactor;
  }

  fclose(fd);

  /*
  // [redundant with ofPolyline::getCentroid2D() but nice to keep]
  // Calculate center of gravity from bounding box.
  auto resultX = std::minmax_element(polygon_.points.begin(), polygon_.points.end(), 
    [](const auto &a, const auto &b) {return (a.x < b.x);}
  );
  auto resultY = std::minmax_element(polygon_.points.begin(), polygon_.points.end(), 
    [](const auto &a, const auto &b) {return (a.y < b.y);}
  );
  center_.x = 0.5f*(resultX.first->x + resultX.second->x);
  center_.y = 0.5f*(resultY.first->y + resultY.second->y);
  */
}

