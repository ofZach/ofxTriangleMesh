#ifndef TRIANGLE_TRIANGLE_UTILS_H_
#define TRIANGLE_TRIANGLE_UTILS_H_

#include <array>
#include <vector>
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"

/* -------------------------------------------------------------------------- */

class TriangleUtils {
 public :
  using Vertex_t = glm::vec3;
  using VertexBuffer_t = std::vector<Vertex_t>;
  
  using Segment_t = glm::ivec2;
  using SegmentBuffer_t = std::vector<Segment_t>;

  struct Polygon_t {
    Polygon_t() = default;

    Polygon_t(VertexBuffer_t const& _points, SegmentBuffer_t const& _segments, VertexBuffer_t const& _holes = VertexBuffer_t()) 
      : points(_points)
      , segments(_segments)
      , holes(_holes)
    {}

    VertexBuffer_t points;
    SegmentBuffer_t segments;
    VertexBuffer_t holes;
  };

  struct Triangle_t {
    std::array<Vertex_t, 3> coords;
    std::array<int32_t, 3> indices;
  };

  struct TriMesh_t {
    VertexBuffer_t points;
    std::vector<Triangle_t> triangles;
  };

  struct Voronoi_t {
    VertexBuffer_t points;
    SegmentBuffer_t segments;
    //std::vector<bool> is_inside;
  };

  static constexpr float kAngleConstraintLimit = 34.0f;
  static constexpr float kInfVoronoiSegmentFactor = 250.0f;

  // Helper to cast a vector of Vertex_t-like structure to be 
  // accepted by the instance as VertexBuffer.
  template<typename T>
  static VertexBuffer_t const& CastVector(T const& v) {
    return *reinterpret_cast<const VertexBuffer_t*>(&v);
  }

  // Utility function to generate a convex hull.
  static void QuickHull(VertexBuffer_t const& points, VertexBuffer_t &hull);

 public:
  TriangleUtils() = default;
      
  void triangulate(VertexBuffer_t const& contour, float angleConstraint=-1, int sizeConstraint=-1);

  void clear() {
    trimesh_.points.clear();
    trimesh_.triangles.clear();
    voronoi_.points.clear();
    voronoi_.segments.clear();
  }

  //-----------------------------------
  // usage notes:
  // points : unordered set of vertices.
  // segments : set of vertex indices for segment of the shape.
  // holes : points inside interior segment loops not to triangulate.
  // bAddVertices : when set to true, authorize the algorithm to add
  //   new vertices to fullfill its constraints.
  //-----------------------------------

  // Simple polygon triangulation.
  void triangulateSimple(Polygon_t const& polygon);

  // Triangulate the convex hull of a set of points.
  void triangulateConvexHull(VertexBuffer_t const& points);
  void triangulateConvexHull(Polygon_t const& polygon);

  // Triangulate using Delaunay's algorithm.
  void triangulateDelaunay(Polygon_t const& polygon, bool bAddVertices=true);

  // Triangulate using Delaunay's algorithm constrained by triangles angle and/or size.
  void triangulateConstrainedDelaunay(Polygon_t const& polygon, float angleConstraint=-1, int sizeConstraint=-1, bool bAddVertices=true);
  
  // Generic triangulation template.
  void triangulateArgs(Polygon_t const& polygon, std::string const& params);

  // Generate a custom Voronoi diagram from a set of points.
  void generateVoronoiDiagram(VertexBuffer_t const& points, bool bUseDelaunay=false);

  // Generate a Voronoi diagram based on a previous triangulation result, if any exists.
  bool generateVoronoiDiagram();

  // Getters.
  inline TriMesh_t const& trimesh() const noexcept { return trimesh_; }
  inline Voronoi_t const& voronoi() const noexcept { return voronoi_; }

 protected:
  // Function to specialize used to create engine-specific triangle mesh.
  virtual void generateTriangleMesh() {}

  TriMesh_t trimesh_;
  Voronoi_t voronoi_;
};

#endif // TRIANGLE_TRIANGLE_UTILS_H_

/* -------------------------------------------------------------------------- */
