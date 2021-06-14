#include "ofxTriangleMesh.h"
#include "triangle.h"

/* -------------------------------------------------------------------------- */

void ofxTriangleMesh::QuickHull(const ofPolyline &points, ofPolyline &hull) {
  VertexBuffer_t hull_vertices;
  TriangleUtils::QuickHull( points.getVertices(), hull_vertices);
  hull.addVertices(hull_vertices.data(), hull_vertices.size());
}

/*--------------------------------------------------------------------------- */

void ofxTriangleMesh::triangulate(const ofPolyline &contour, float angleConstraint, int sizeConstraint) {
  TriangleUtils::triangulate( contour.getVertices(), angleConstraint, sizeConstraint);
}

/* -------------------------------------------------------------------------- */

void ofxTriangleMesh::draw(bool use_debug_color) const {
  // draw the triangles in their random colors:
  if (use_debug_color) {
    int index = 0;
    for (const auto& tri : trimesh_.triangles) {
      ofFill();
      if (use_debug_color) {
        ofSetColor( randomColors[index++] );
      }
      ofDrawTriangle( trimesh_.points[tri.indices[0]],
                      trimesh_.points[tri.indices[1]],
                      trimesh_.points[tri.indices[2]] );
    }

    // draw the mesh as a wire frame in white on top.
    ofSetColor(0xff);
    drawWireframe();
  } else {
    triangulatedMesh.draw();
  }
}

/* -------------------------------------------------------------------------- */

void ofxTriangleMesh::drawVoronoi() const {
  for (const auto &s : voronoi_.segments) {
    const auto &a = voronoi_.points[s.x];
    const auto &b = voronoi_.points[s.y];
    ofDrawLine(a.x, a.y, b.x, b.y);
  }
}

void ofxTriangleMesh::drawCleanVoronoi(const VertexBuffer_t& poly) const {
  /// @note
  /// Neither efficient nor it seems always true when the shape contains
  /// holes.

  for (const auto &s : voronoi_.segments) {
    const auto &a = voronoi_.points[s.x];
    const auto &b = voronoi_.points[s.y];

    bool inside1 = ofInsidePoly(a, poly);
    bool inside2 = ofInsidePoly(b, poly);
    if (inside1 && inside2) {
      ofDrawLine(a.x, a.y, b.x, b.y);
    }
  }
}

/* -------------------------------------------------------------------------- */

void ofxTriangleMesh::generateTriangleMesh() {
  triangulatedMesh.clear();
  triangulatedMesh.setMode(OF_PRIMITIVE_TRIANGLES);

  for (const auto &pt : trimesh_.points) {
    triangulatedMesh.addVertex(pt);
  }
  for (const auto &tri : trimesh_.triangles) {
    triangulatedMesh.addIndex(tri.indices[0]);
    triangulatedMesh.addIndex(tri.indices[1]);
    triangulatedMesh.addIndex(tri.indices[2]);
  }

  for (size_t i=randomColors.size(); i<trimesh_.triangles.size(); ++i) {
    randomColors.push_back( ofColor(ofRandom(0xff), ofRandom(0xff), ofRandom(0xff)) );
  }
}

/* -------------------------------------------------------------------------- */