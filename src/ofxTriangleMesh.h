 /*!
 
 ofxTriangleMesh by zach lieberman
 
 based off of 

 ofxTriangle by kikko.fr
 Point inside polygon by Theo / paul bourke
 

 this code uses the original triangle (not triangle++) to allow for switches like 
 q33 a100, etc 
 
 https://www.cs.cmu.edu/~quake/triangle.switch.html
 
 there are options that allow for the addition of new points to make a much nicer mesh, 
 triangle++ had that turned off and a heavily modified version of triangle, 
 so this is using the original triangle library and allows for some control of parameters
 
*/

#pragma once

#include "ofMain.h"
#include "triangle_utils.h"

class ofxTriangleMesh : public TriangleUtils {
  public:
    using TriangleUtils::triangulate;

    // usage notes: 
    // -1 = don't use constraint, other values = use constraint 
    // 
    // for angle, 20-30 is pretty good
    // https://www.cs.cmu.edu/~quake/triangle.q.html
    // be careful!  "It usually doesn't terminate for angles above 34o"
    //
    // for size, this depends on the size of your shape, 
    // 100 to 200 is a good first guess for screen resolution based points
    void triangulate(const ofPolyline &contour, float angleConstraint=-1, int sizeConstraint=-1);
    
    void draw(bool use_debug_color = false) const;
    void drawWireframe() const { triangulatedMesh.drawWireframe(); }

    void drawVoronoi() const;
    void drawCleanVoronoi(const VertexBuffer_t& poly) const;

    ofMesh& mesh() { return triangulatedMesh; }
    ofMesh const& mesh() const { return triangulatedMesh; }
    
    // Utility function to generate a convex hull from a polyline.
    static void QuickHull(const ofPolyline &points, ofPolyline &hull);

  private:
    void generateTriangleMesh() override;
    
    ofMesh triangulatedMesh;
    vector <ofColor> randomColors;
};
