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

typedef struct{
    ofPoint pts[3];
    int index[3];  // for the mesh, what points does this triangle relate to.
} meshTriangle;

class ofxTriangleMesh {
    // [Legacy code]
    public :
    ofxTriangleMesh();
        
    // usage notes: 
    // -1 = don't use constraint, other values = use constraint 
    // 
    // for angle, 20-30 is pretty good
    // https://www.cs.cmu.edu/~quake/triangle.q.html
    // be careful!  "It usually doesn't terminate for angles above 34o"
    //
    // for size, this depends on the size of your shape, 
    // 100 to 200 is a good first guess for screen resolution based points
    void triangulate(const ofPolyline &contour, float angleConstraint=-1, float sizeConstraint=-1);    

    void draw(bool use_debug_color = true) const;
    void clear();

    // Triangulation output.
    int nTriangles;
    vector <ofPoint> outputPts;
    vector <meshTriangle> triangles;
    vector <ofColor> randomColors;
    ofMesh triangulatedMesh;

    // [extension]
    public :
    // Aliases of internal types (for easy change if needed).
    typedef ofPoint Vertex_t;
    typedef vector<ofPoint> VertexVector_t;
    typedef vector<glm::ivec2> SegmentVector_t;

    // Helper to cast a vector of Vertex_t-like structure to be 
    // accepted by the instance.
    template<typename T>
    static const VertexVector_t& CastVector(const T &v) {
        return *reinterpret_cast<const VertexVector_t*>(&v);
    }

    // Utility function to generate a convex hull.
    static void QuickHull(const ofPolyline &points, ofPolyline &hull);

    //-----------------------------------
    // usage notes:
    // points : unordered set of vertices.
    // segments : set of vertex indices for segment of the shape.
    // holes : points inside interior segment loops not to triangulate.
    // bAddVertices : when set to true, authorize the algorithm to add
    //   new vertices to fullfill its constraints.
    //-----------------------------------

    // Simple triangulation.
    void triangulate(
        const VertexVector_t &points,
        const SegmentVector_t &segments,
        const VertexVector_t &holes = VertexVector_t()
    );

    // Triangulate the convex hull of a set of points.
    void triangulateConvexHull(const VertexVector_t &points);

    // Triangulate using Delaunay's algorithm.
    void triangulateDelaunay(
        const VertexVector_t &points,
        const SegmentVector_t &segments,
        const VertexVector_t &holes = VertexVector_t(),
        bool bAddVertices=true
    );

    // Triangulate using Delaunay's algorithm constrained by
    // triangles angle and/or size.
    void triangulateConstrainedDelaunay(
        const VertexVector_t &points,
        const SegmentVector_t &segments,
        const VertexVector_t &holes = VertexVector_t(),
        float angleConstraint=-1, 
        float sizeConstraint=-1,
        bool bAddVertices=true
    );

    // Generic triangulation template.
    void triangulateArgs(
        const VertexVector_t &points,
        const SegmentVector_t &segments,
        const VertexVector_t &holes,
        const string &triangulateParams
    );

    // Generate a Voronoi diagram based on a previous triangulation
    // result, if any.
    void generateVoronoiDiagram() { 
        generateVoronoiDiagram(outputPts); 
    }

    // Generate a custom Voronoi diagram from a set of points.
    void generateVoronoiDiagram(const VertexVector_t &points, bool bUseDelaunay=false);

    void drawVoronoi() const;
    void drawCleanVoronoi(const VertexVector_t& poly) const;

    // Voronoi diagram output.
    VertexVector_t voronoiPoints;
    SegmentVector_t voronoiSegments;
    std::vector<bool> voronoiInsideSegments;
};

