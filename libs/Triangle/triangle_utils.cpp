#include "triangle_utils.h"

#include <map>
#include "glm/glm.hpp"
#include "triangle.h"

/*--------------------------------------------------------------------------- */

constexpr float TriangleUtils::kAngleConstraintLimit;
constexpr float TriangleUtils::kInfVoronoiSegmentFactor;

namespace {

// Wrap the Triangle main call function to avoid aliasing issues.
void TriangulatePoints(char const* flags, triangulateio *in, triangulateio *mid, triangulateio *out) {
  triangulate((char*)flags, in,  mid, out);
}

// QuickHull 2D Divide & Conquer step :
// Finds the farest vertex from segment AB in direct order, then filter the 
// vertices set and continue further.
using Vec2_t = glm::vec3;
using PointVector_t = std::vector<Vec2_t>;
#define Vec2_t(x,y) Vec2_t(x, y, 0.0f)
void FindHullVertex2D(Vec2_t const &A,
                      Vec2_t const &B,
                      PointVector_t const &vertices,
                      PointVector_t &hull_vertices)
{
  // Compute AB's orthogonal vector.
  auto const AB = B - A;
  auto const N = glm::normalize(Vec2_t(-AB.y, AB.x));

  // Compute vertices dot products relative to the base segment
  // & retrieve the index of valid ones.
  std::vector<float> dots(vertices.size());
  std::vector<size_t> indices;
  for (size_t i=0; i<vertices.size(); ++i) {
    auto const dp = glm::dot(vertices[i]-A, N);

    dots[i] = dp;
    if (dp > std::numeric_limits<float>::epsilon()) { 
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
    auto const vid = indices[i];
    sub_vertices[i] = vertices[vid];
    max_index = (dots[max_index] > dots[vid]) ? max_index : vid;
  }

  // Farest vertex from the base segment AB.
  auto const &P = vertices[max_index];

  // Continue with the two new sub-segments.
  FindHullVertex2D(A, P, sub_vertices, hull_vertices);
  FindHullVertex2D(P, B, sub_vertices, hull_vertices);
}

} // namespace ""


/* -------------------------------------------------------------------------- */

void TriangleUtils::QuickHull(VertexBuffer_t const& vertices, VertexBuffer_t &hull_vertices) {
  // Retrieve corner vertices, known to be on the convex hull.
  auto min_v(vertices[0u]), max_v(vertices[0u]);
  for (auto const& v : vertices) {
    min_v = (min_v.x < v.x) ? min_v : v;
    max_v = (max_v.x > v.x) ? max_v : v;
  }

  // Vertices of the hull, initialized with the left vertex.
  hull_vertices.clear();
  hull_vertices.push_back(min_v);

  // Subdivide search.
  FindHullVertex2D(min_v, max_v, vertices, hull_vertices);
  FindHullVertex2D(max_v, min_v, vertices, hull_vertices);
}

/*--------------------------------------------------------------------------- */

void TriangleUtils::triangulate(VertexBuffer_t const& vertices, float angleConstraint, int sizeConstraint) {
  Polygon_t polygon;

  // Transform the polyline loop into separate buffers
  // of points and segments while removing the last duplicated element.
  int const size = vertices.size() - 1; //

  polygon.points.reserve(size);
  polygon.segments.reserve(size);
  for (int i = 0; i < size; ++i) {
    polygon.points.push_back( vertices[i] );
    polygon.segments.push_back( Segment_t(i, (i+1)%size) );
  }
  triangulateConstrainedDelaunay( polygon, angleConstraint, sizeConstraint, false);
}

/* -------------------------------------------------------------------------- */

void TriangleUtils::triangulateSimple(Polygon_t const& polygon) {
  triangulateArgs(polygon, "zQpY");
}

void TriangleUtils::triangulateConvexHull(Polygon_t const& polygon) {
  triangulateArgs( polygon, "zQc"); 
}

void TriangleUtils::triangulateConvexHull(VertexBuffer_t const& points) {
  triangulateConvexHull( { points, SegmentBuffer_t() } );
}

void TriangleUtils::triangulateDelaunay(Polygon_t const& polygon, bool bAddVertices) {
  std::string params = "zQpD";
  if (!bAddVertices) {
    params += "Y";   
  }
  triangulateArgs( polygon, params);
}

void TriangleUtils::triangulateConstrainedDelaunay(Polygon_t const& polygon, float angleConstraint, int sizeConstraint, bool bAddVertices) {
  std::string params = "zQp";
  if (angleConstraint > 0) {
    params += "q" + std::to_string( std::min(kAngleConstraintLimit, angleConstraint) );
  }
  if (sizeConstraint > 0) {
    params += "a" + std::to_string( sizeConstraint );
  }
  if (!bAddVertices) {
    params += "Y";   
  }
  triangulateArgs( polygon, params);
}

void TriangleUtils::triangulateArgs(Polygon_t const& polygon, std::string const& params) {
  // >> params basic options :
  // Verbose : V
  // Quiet : Q
  // Index start at 0 : z
  // Simple Polygon Triangulation :  p
  // Don't add new vertices : Y
  // Constrained Delaunay : p[aX qX]
  // Conforming Delaunay : pD
  // Convex Hull : c

  auto &points = polygon.points;
  auto &segments = polygon.segments;
  auto &holes = polygon.holes;

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

  /// @note 'vorout' is necessary only when 'v' is set in params 
  /// but is not actually accounted for here, see generateVoronoiDiagram().
  struct triangulateio out{}, vorout{};
  TriangulatePoints(params.c_str(), &in, &out, &vorout);
  
  // -------------------------------------

  auto &Triangles = trimesh_.triangles;
  Triangles.clear();
  Triangles.reserve(out.numberoftriangles);

  std::map<int, Vertex_t> goodPts;
  for (int i = 0; i < out.numberoftriangles; i++) {
    Triangle_t triangle;
      
    for (int j = 0; j < 3; j++){
      int const index = out.trianglelist[3 * i + j];
      triangle.coords[j].x  = out.pointlist[2*index + 0];
      triangle.coords[j].y  = out.pointlist[2*index + 1];
      triangle.coords[j].z  = 0;
      triangle.indices[j]   = index;
    }
    Triangles.push_back(triangle);
    
    for (int j = 0; j < 3; j++){
      goodPts[triangle.indices[j]] = triangle.coords[j]; 
    }
  }
  
  // put all good points in a std::vector and handle the remapping of indices.
  // the indices stored above were for all points, but since we drop triangles, we 
  // can drop non used points, and then remap all the indces. 
  // that happens here: 
  auto &Points = trimesh_.points;
  Points.clear();

  std::map<int, int> indexChanges;
  for (auto iter = goodPts.cbegin(); iter != goodPts.cend(); ++iter) {
    indexChanges[iter->first] = static_cast<int>(Points.size());
    Points.push_back(iter->second);
  }
  
  // now, with the new, potentially smaller group of points, 
  // update all the indices of the triangles so their indices point right: 
  for (auto &tri : Triangles){
    for (int j = 0; j < 3; j++){
      tri.indices[j] = indexChanges[tri.indices[j]];
    }
  }
 
  generateTriangleMesh();

  // -------------------------------------

  if (in.pointlist) free(in.pointlist);
  if (in.segmentlist) free(in.segmentlist);
  if (in.holelist) free(in.holelist);

  if (out.pointlist) free(out.pointlist);
  if (out.pointattributelist) free(out.pointattributelist);
  if (out.pointmarkerlist) free(out.pointmarkerlist);
  if (out.trianglelist) free(out.trianglelist);
  if (out.triangleattributelist) free(out.triangleattributelist);
  if (out.segmentlist) free(out.segmentlist);
}

/* -------------------------------------------------------------------------- */

void TriangleUtils::generateVoronoiDiagram(VertexBuffer_t const& points, bool bUseDelaunay) {
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

  std::string params = "zQv";
  if (bUseDelaunay) {
    params += "D";
  }

  TriangulatePoints(params.c_str(), &in, &out, &vorout);

  // Save Voronoi result vertices.
  voronoi_.points.resize(vorout.numberofpoints);
  for (int i = 0; i < vorout.numberofpoints; ++i) {
    auto &pt = voronoi_.points[i];
    pt.x = vorout.pointlist[2*i+0];
    pt.y = vorout.pointlist[2*i+1];
    pt.z = 0;
  }

  // Remap the edges indices to account for infinite vertices.
  voronoi_.segments.resize(vorout.numberofedges);
  for (int i=0; i < vorout.numberofedges; ++i) {
    int const e1 = vorout.edgelist[2*i+0];
    int       e2 = vorout.edgelist[2*i+1];

    // if the edge is at infinite, add a virtually infinite distant vertex.
    if (e2 < 0) {
      float const nx = vorout.normlist[2*i+0];
      float const ny = vorout.normlist[2*i+1];
      
      auto pt = voronoi_.points[e1];
      pt.x += kInfVoronoiSegmentFactor * nx;
      pt.y += kInfVoronoiSegmentFactor * ny;
      voronoi_.points.push_back(pt);

      e2 = static_cast<int>(voronoi_.points.size()) - 1;
    }

    voronoi_.segments[i] = Segment_t(e1, e2);
  }

  /*
  // Detect interior segments for clean voronoi
  voronoi_.is_inside.clear();
  voronoi_.is_inside.resize(voronoi_.segments.size());
  
  //auto const &poly = points;
  
  int index(0);
  for (auto const &s : voronoi_.segments) {
    auto const &a = voronoi_.points[s.x];
    auto const &b = voronoi_.points[s.y];

    bool inside1 = ofInsidePoly(a, poly);
    bool inside2 = ofInsidePoly(b, poly);
    voronoi_.is_inside[index++] = (inside1 && inside2);
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

bool TriangleUtils::generateVoronoiDiagram() {
  if (trimesh_.points.empty()) {
    return false;
  }
  generateVoronoiDiagram( trimesh_.points );
  return true;
}

/* -------------------------------------------------------------------------- */
