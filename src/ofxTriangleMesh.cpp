#include "ofxTriangleMesh.h"
#include "triangle.h"


/*--------------------------------------------------------------------------- */

namespace {

// Wrap the Triangle function to avoid aliasing issues.
void TriangulatePoints(
    char * flags, triangulateio * in, 
    triangulateio * mid, 
    triangulateio * out) 
{
    triangulate(flags, in,  mid, out);
}

// Types wrappers
typedef ofDefaultVec3 Vec2_t;
#define Vec2_t(x,y) Vec2_t(x, y, 0.0f)
typedef vector<Vec2_t> PointVector_t;

// QuickHull 2D Divide & Conquer step :
// Finds the farest vertex from segment AB in direct order, then filter the 
// vertices set and continue further.
void FindHullVertex2D(Vec2_t const &A,
                      Vec2_t const &B,
                      PointVector_t const &vertices,
                      PointVector_t &hull_vertices)
{
    // Compute AB's orthogonal vector.
    const auto AB = B - A;
    const auto N = glm::normalize(Vec2_t(-AB.y, AB.x));

    // Compute vertices dot products relative to the base segment
    // & retrieve the index of valid ones.
    vector<float> dots(vertices.size());
    vector<size_t> indices;
    for (size_t i=0; i<vertices.size(); ++i) {
        const auto dp = glm::dot(vertices[i]-A, N);

        dots[i] = dp;
        if (dp > numeric_limits<float>::epsilon()) { 
            indices.push_back(i);
        }
    }

    // Exit condition: no more vertices on this edge of space.
    // Add the right vertex (B) and quit. 
    if ( indices.empty()
     || ((indices.size() == 1) && (vertices[indices[0]] == B))) {
        hull_vertices.push_back(B);
        return;
    }

    // Filter vertices & retrieve the farest vertex index.
    PointVector_t sub_vertices(indices.size());
    size_t max_index = 0;
    for (size_t i=0; i<indices.size(); ++i) {
        const auto vid = indices[i];
        sub_vertices[i] = vertices[vid];
        max_index = (dots[max_index] > dots[vid]) ? max_index : vid;
    }

    // Farest vertex from the base segment AB.
    const auto &P = vertices[max_index];

    // Continue with the two new sub-segments.
    FindHullVertex2D(A, P, sub_vertices, hull_vertices);
    FindHullVertex2D(P, B, sub_vertices, hull_vertices);
}

} // namespace ""


/* -------------------------------------------------------------------------- */

void ofxTriangleMesh::QuickHull(const ofPolyline &points, ofPolyline &hull)
{
    const PointVector_t &vertices = points.getVertices();
    PointVector_t hull_vertices;

    // Retrieve corner vertices, known to be on the convex hull.
    auto min_v(vertices[0u]), max_v(vertices[0u]);
    for (const auto& v : vertices) {
        min_v = (min_v.x < v.x) ? min_v : v;
        max_v = (max_v.x > v.x) ? max_v : v;
    }

    // Vertices of the hull, initialized with the left vertex.
    hull_vertices.clear();
    hull_vertices.push_back(min_v);

    // Subdivide search.
    FindHullVertex2D(min_v, max_v, vertices, hull_vertices);
    FindHullVertex2D(max_v, min_v, vertices, hull_vertices);

    // Add the result vertices to the polyline.
    hull.addVertices(hull_vertices.data(), hull_vertices.size());
}

/*--------------------------------------------------------------------------- */

ofxTriangleMesh::ofxTriangleMesh(){
    nTriangles = 0;
}

void ofxTriangleMesh::triangulate(const ofPolyline &contour, float angleConstraint, float sizeConstraint)
{
///
/// Emulate the original method, which act like a Constrained Delaunay
/// with no added vertices and no handling of holes.
///
    VertexVector_t points;
    SegmentVector_t segments;
    VertexVector_t holes;

    // Transform the polyline loop into separate buffers
    // of points and segments while removing the last dupplicate element.
    const auto &vertices = contour.getVertices();

    const int size = vertices.size() - 1;
    for (int i = 0; i<size; ++i) {
        points.push_back(vertices[i]);
        segments.push_back(glm::ivec2(i, (i+1)%size));
    }

    string triangulateParams = "zQYp";
    if (angleConstraint > 0) {
        triangulateParams += "q" + ofToString( angleConstraint );
    }
    if (sizeConstraint > 0) {
        triangulateParams += "a" + ofToString( (int)sizeConstraint );
    }
    triangulateArgs(points, segments, holes, triangulateParams);
}

/* -------------------------------------------------------------------------- */

void ofxTriangleMesh::triangulate(
    const VertexVector_t &points,
    const SegmentVector_t &segments,
    const VertexVector_t &holes
)
{
    triangulateArgs(points, segments, holes, "zQpY");
}

void ofxTriangleMesh::triangulateConvexHull(
    const VertexVector_t &points
)
{
    SegmentVector_t segments;
    VertexVector_t holes;
    triangulateArgs(points, segments, holes, "zQc");
}

void ofxTriangleMesh::triangulateDelaunay(
    const VertexVector_t &points,
    const SegmentVector_t &segments,
    const VertexVector_t &holes,
    bool bAddVertices
)
{
    string triangulateParams = "zQpD";
    if (!bAddVertices) {
        triangulateParams += "Y";   
    }
    triangulateArgs(points, segments, holes, triangulateParams);
}

void ofxTriangleMesh::triangulateConstrainedDelaunay(
    const VertexVector_t &points,
    const SegmentVector_t &segments,
    const VertexVector_t &holes,
    float angleConstraint, 
    float sizeConstraint,
    bool bAddVertices
)
{
    string triangulateParams = "zQp";
    if (angleConstraint > 0) {
        triangulateParams += "q" + ofToString( angleConstraint );
    }
    if (sizeConstraint > 0) {
        triangulateParams += "a" + ofToString( (int)sizeConstraint );
    }
    if (!bAddVertices) {
        triangulateParams += "Y";   
    }
    triangulateArgs(points, segments, holes, triangulateParams);
}

void ofxTriangleMesh::triangulateArgs(
    const VertexVector_t &points,
    const SegmentVector_t &segments,
    const VertexVector_t &holes,
    const string &triangulateParams
) 
{
    // >> triangulateParams basic options :
    // Verbose : V
    // Quiet : Q
    // Index start at 0 : z
    // Simple Polygon Triangulation :  p
    // Don't add new vertices : Y
    // Constrained Delaunay : p[aX qX]
    // Conforming Delaunay : pD
    // Convex Hull : c

    struct triangulateio in{};
    in.numberofpoints = static_cast<int>(points.size());
    in.numberofpointattributes = 0;
    in.pointmarkerlist = nullptr;
    in.pointlist = (REAL*)calloc(2 * in.numberofpoints, sizeof(REAL));
    in.numberofsegments = static_cast<int>(segments.size());
    in.segmentlist = (int*)calloc(2 * in.numberofsegments, sizeof(int));
    in.segmentmarkerlist = nullptr;
    in.numberofholes = static_cast<int>(holes.size());
    in.holelist = (REAL*)calloc(2 * in.numberofholes, sizeof(REAL));

    for(int i = 0; i < in.numberofpoints; i++) {
        in.pointlist[i*2+0] = points[i].x;
        in.pointlist[i*2+1] = points[i].y;
    }
    for(int i = 0; i < in.numberofsegments; i++) {
        in.segmentlist[i*2+0] = segments[i].x;
        in.segmentlist[i*2+1] = segments[i].y;
    }
    for(int i = 0; i < in.numberofholes; i++) {
        in.holelist[i*2+0] = holes[i].x;
        in.holelist[i*2+1] = holes[i].y;
    }

    /// @warning 'vorout' is necessary only when 'v' is set in triangulateParams 
    /// but is not actually accounted for here, see generateVoronoiDiagram().
    struct triangulateio out{}, vorout{};
    TriangulatePoints((char *)triangulateParams.c_str(), &in, &out, &vorout);
    
    nTriangles = 0;
    triangles.clear();
    
    // resize the buffer of colors used for debugging.
    randomColors.reserve(out.numberoftriangles);
    const int numRandomColors = static_cast<int>(randomColors.size());

    map<int, ofPoint> goodPts;
    for (int i = 0; i < out.numberoftriangles; i++) {
        meshTriangle triangle;
        
        int whichPt;  
        for (int j = 0; j < 3; j++){
            whichPt = out.trianglelist[i * 3 + j];
            triangle.pts[j] = ofPoint(
                out.pointlist[ whichPt * 2 + 0], 
                out.pointlist[ whichPt * 2 + 1]
            );
            triangle.index[j] = whichPt;
        }
        
        if (i >= numRandomColors) {
            randomColors.push_back(ofColor(ofRandom(255), ofRandom(255), ofRandom(255)));
        }

        triangles.push_back(triangle);
        
        // store the good points in a map
        for (int j = 0; j < 3; j++){
            goodPts[triangle.index[j]] = triangle.pts[j]; 
        }
        nTriangles++;
    }
    
    // put all good points in a vector and handle the remapping of indices.
    // the indices stored above were for all points, but since we drop triangles, we 
    // can drop non used points, and then remap all the indces. 
    // that happens here: 
    
    outputPts.clear();
    map < int, int > indexChanges;
    map< int , ofPoint >::iterator iter;
    for (iter = goodPts.begin(); iter != goodPts.end(); ++iter) {
        //cout << iter->first << " " << iter->second << endl;
        indexChanges[iter->first] = outputPts.size();
        outputPts.push_back(iter->second);
    }
    
    // now, with the new, potentially smaller group of points, 
    // update all the indices of the triangles so their indices point right: 
    for (auto &tri : triangles){
        for (int j = 0; j < 3; j++){
            tri.index[j] = indexChanges[tri.index[j]];
        }
    }
    
    // now make a mesh, using indices: 
    triangulatedMesh.clear();
    triangulatedMesh.setMode(OF_PRIMITIVE_TRIANGLES);
    for (const auto &pt : outputPts){
        triangulatedMesh.addVertex(pt);
    }
    
    for (const auto &tri : triangles){
        triangulatedMesh.addIndex(tri.index[0]);
        triangulatedMesh.addIndex(tri.index[1]);
        triangulatedMesh.addIndex(tri.index[2]);
    }

    // depending on flags, we may need to adjust some of the memory clearing
    // (see tricall.c for full listings)
    // TODO: this should be agressively tested. 
    
    if (in.pointlist) free(in.pointlist);
    if (in.segmentlist) free(in.segmentlist);
    if (in.holelist) free(in.holelist);

    if (out.pointlist) free(out.pointlist);
    if (out.pointattributelist) free(out.pointattributelist);
    if (out.pointmarkerlist) free(out.pointmarkerlist);
    if (out.trianglelist) free(out.trianglelist);
    if (out.triangleattributelist) free(out.triangleattributelist);
    if (out.segmentlist) free (out.segmentlist);
}

/* -------------------------------------------------------------------------- */

void ofxTriangleMesh::generateVoronoiDiagram(const VertexVector_t &points, bool bUseDelaunay)
{
    if (points.size() < 3) {
        return;
    }

    struct triangulateio in{}, out{}, vorout{};
    in.numberofpoints = static_cast<int>(points.size());
    in.pointlist = (REAL*)calloc(2 * in.numberofpoints, sizeof(REAL));
    for(int i = 0; i < in.numberofpoints; i++) {
        in.pointlist[i*2+0] = points[i].x;
        in.pointlist[i*2+1] = points[i].y;
    }

    string params = "zQv";
    if (bUseDelaunay) {
        params += "D";
    }

    TriangulatePoints((char*)params.c_str(), &in, &out, &vorout);

    // Save Voronoi result vertices.
    voronoiPoints.resize(vorout.numberofpoints);
    for (int i=0; i<vorout.numberofpoints; ++i) {
        voronoiPoints[i] = ofPoint(
            vorout.pointlist[2*i+0],
            vorout.pointlist[2*i+1]
        );
    }

    // Remap the edges indices to account for infinite vertices.
    const float infSegmentFactor = 250.0f;
    voronoiSegments.resize(vorout.numberofedges);
    for (int i=0; i<vorout.numberofedges; ++i) {
        int e1 = vorout.edgelist[2*i+0];
        int e2 = vorout.edgelist[2*i+1];

        // if the edge is at infinite, add a virtually infinite distant vertex.
        if (e2 < 0) {
            float nx = vorout.normlist[2*i+0];
            float ny = vorout.normlist[2*i+1];
            
            voronoiPoints.push_back(
                voronoiPoints[e1] + infSegmentFactor*ofPoint(nx, ny)
            );

            e2 = static_cast<int>(voronoiPoints.size()) - 1;
        }

        voronoiSegments[i] = glm::ivec2(e1, e2);
    }

    /*
    // Detect interior segments for clean voronoi
    voronoiInsideSegments.clear();
    voronoiInsideSegments.resize(voronoiSegments.size());
    
    //const auto &poly = points;
    
    int index(0);
    for (const auto &s : voronoiSegments) {
        const auto &a = voronoiPoints[s.x];
        const auto &b = voronoiPoints[s.y];

        bool inside1 = ofInsidePoly(a, poly);
        bool inside2 = ofInsidePoly(b, poly);
        voronoiInsideSegments[index++] = (inside1 && inside2);
    }
    */

    if (in.pointlist) free(in.pointlist);
    if (in.segmentlist) free(in.segmentlist);
    if (in.holelist) free(in.holelist);

    if (out.pointlist) free(out.pointlist);
    if (out.pointattributelist) free(out.pointattributelist);
    if (out.pointmarkerlist) free(out.pointmarkerlist);
    if (out.trianglelist) free(out.trianglelist);
    if (out.triangleattributelist) free(out.triangleattributelist);
    if (out.segmentlist) free (out.segmentlist);

    if (vorout.pointattributelist) free(vorout.pointattributelist);
    if (vorout.edgelist) free(vorout.edgelist);
    if (vorout.normlist) free(vorout.normlist);
}

void ofxTriangleMesh::drawVoronoi() const {
  for (const auto &s : voronoiSegments) {
    const auto &a = voronoiPoints[s.x];
    const auto &b = voronoiPoints[s.y];
    ofDrawLine(a.x, a.y, b.x, b.y);
  }
}

void ofxTriangleMesh::drawCleanVoronoi(const VertexVector_t& poly) const {
  /// @note
  /// Neither efficient nor it seems always true when the shape contains
  /// holes.

  for (const auto &s : voronoiSegments) {
    const auto &a = voronoiPoints[s.x];
    const auto &b = voronoiPoints[s.y];

    bool inside1 = ofInsidePoly(a, poly);
    bool inside2 = ofInsidePoly(b, poly);
    if ((inside1 && inside2)) {
      ofDrawLine(a.x, a.y, b.x, b.y);
    }
  }

}

/* -------------------------------------------------------------------------- */

void ofxTriangleMesh::clear() 
{
  triangles.clear();
  nTriangles = 0;
}

void ofxTriangleMesh::draw(bool use_debug_color) const
{
  // draw the triangles in their random colors:
  int index = 0;
  for (const auto& tri : triangles) {
    ofFill();
    if (use_debug_color) {
        ofSetColor( randomColors[index++] );
    }
    ofDrawTriangle( outputPts[tri.index[0]],
                    outputPts[tri.index[1]],
                    outputPts[tri.index[2]] );
  }

  // draw the mesh as a wire frame in white on top. 
  if (use_debug_color) {
    ofSetColor(255,255,255);
    triangulatedMesh.drawWireframe();
  }
}

/* -------------------------------------------------------------------------- */
