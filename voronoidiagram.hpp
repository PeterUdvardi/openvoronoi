/*  
 *  Copyright 2010-2011 Anders Wallin (anders.e.e.wallin "at" gmail.com)
 *  
 *  This file is part of OpenVoronoi.
 *
 *  OpenCAMlib is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  OpenCAMlib is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with OpenCAMlib.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef VORONOI_DIAGRAM_HPP
#define VORONOI_DIAGRAM_HPP

#include <queue>
#include <boost/tuple/tuple.hpp>

#include "point.hpp"
#include "voronoidiagram_graph.hpp"
#include "vertex_positioner.hpp"
#include "version_string.hpp" // autogenerated by version_string.cmake

namespace ovd
{

class VoronoiDiagramChecker;
class FaceGrid;


typedef std::pair<HEVertex, double> VertexDetPair;
class abs_comparison {
public:
  bool operator() (const VertexDetPair& lhs, const VertexDetPair&rhs) const {
    return ( fabs(lhs.second) < fabs(rhs.second) );
  }
};

// vertices for processing held in this queue
// sorted by decreasing fabs() of in_circle-predicate, so that the vertices whose IN/OUT status we are 'most certain' about are processed first
typedef std::priority_queue< VertexDetPair , std::vector<VertexDetPair>, abs_comparison > VertexQueue;

/// \brief Voronoi diagram.
///
/// see http://en.wikipedia.org/wiki/Voronoi_diagram
/// 
/// the dual of a voronoi diagram is the delaunay diagram(triangulation).
///  voronoi-faces are dual to delaunay-vertices.
///  vornoi-vertices are dual to delaunay-faces 
///  voronoi-edges are dual to delaunay-edges
class VoronoiDiagram {
    public:
        /// ctor
        VoronoiDiagram() {}
        /// create diagram with given far-radius and number of bins
        /// \param far radius of circle centered at (0,0) within which all sites must lie
        /// \param n_bins number of bins used for nearest vd-vertex bucket-search
        VoronoiDiagram(double far, unsigned int n_bins);
        /// dtor
        virtual ~VoronoiDiagram();
        /// insert a point site into the diagram 
        /// returns an integer handle to the inserted point. use this integer when inserting lines/arcs
        int insert_point_site(const Point& p);
        /// insert a line-segment site into the diagram
        void insert_line_site(int idx1, int idx2);
        void insert_line_site1(int idx1, int idx2);
        void insert_line_site2(int idx1, int idx2);
        void insert_line_site3(int idx1, int idx2);
        /// string repr
        std::string print() const;
        /// return the far radius
        double get_far_radius() const {return far_radius;}
        
        friend class VoronoiDiagramChecker;
        friend class VertexPositioner;

        std::string version() const { return VERSION_STRING; }
    protected:
        /// initialize the diagram with three generators
        void initialize();
        HEVertex find_seed_vertex(HEFace f, Site* site) const;
        EdgeVector find_in_out_edges(); 
        boost::tuple<HEEdge, HEVertex, HEEdge> find_new_vertex(HEFace f, VoronoiVertexStatus s1);
        
        void augment_vertex_set(HEVertex& v_seed, Site* site);        
        bool predicate_c4(HEVertex v);
        bool predicate_c5(HEVertex v);
        void mark_adjacent_faces(HEVertex v);
        void mark_vertex(HEVertex& v,  Site* site); 
        void   add_new_vertices( Site* site);
        HEFace add_new_face(Site* site);
        void   add_new_edge(HEFace new_f, HEFace f);
        void   add_vertex_in_edge(HEVertex v, HEEdge e);
        void add_separator(HEFace f, Site* s, HEVertex endp);
        void remove_vertex_set( HEFace newface );
        void reset_status();

    // PRINT ETC
        void print_face(HEFace f);
        void print_vertices(VertexVector& q);
    // HELPER-CLASSES
        /// sanity-checks on the diagram are done by this helper class
        VoronoiDiagramChecker* vd_checker;
        /// a grid-search algorithm which allows fast nearest-neighbor search
        FaceGrid* fgrid;
        /// an algorithm for positioning vertices
        VertexPositioner* vpos;
    // DATA
        /// the half-edge diagram of the vd
        HEGraph g;
        /// the voronoi diagram is constructed for sites within a circle with radius far_radius
        double far_radius;
        /// special initial/outer vertices
        HEVertex out_verts[3];
        /// the number of generators
        int gen_count;
        /// temporary variable for incident faces, will be reset to NONINCIDENT after a site has been inserted
        FaceVector incident_faces;
        /// temporary variable for in-vertices, out-vertices that need to be reset
        /// after a site has been inserted
        VertexVector modified_vertices;
        /// IN-vertices, i.e. to-be-deleted
        VertexVector v0;
        /// queue of vertices to be processed
        VertexQueue vertexQueue; 

};


} // end namespace
#endif
// end voronoidiagram.h
