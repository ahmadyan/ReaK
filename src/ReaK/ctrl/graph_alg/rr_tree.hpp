/**
 * \file rr_tree.hpp
 * 
 * This library contains the Rapidly-Exploring Random Tree generation algorithm.
 * This is a method to create a random tree that will span over a non-convex space
 * as rapidly as possible. The method relies on a simple randomized insertion algorithm.
 * At each step, a random point is picked from the underlying topology (i.e. configuration 
 * space in path-planning terms). Then, the point in the current graph that is nearest 
 * to the random point is picked for expansion. Finally, edges (of a maximum length) are 
 * added to the vertex of the graph towards the random point while it is still possible to
 * add such an edge without leaving the free space (the part of the configuration space which 
 * is not occupied by an obstacle). The algorithm will stop when either the number of vertices
 * in the tree has reached a maximum or when the user callback signals the stop.
 * 
 * This library also provides the bidirectional version of the RRT algorithm. In this version,
 * two trees are generated. Typically, one tree is initialized with the starting vertex and 
 * the other is initialized with the goal vertex. The algorithm works to try and join the 
 * two graphs as quickly as possible and with the most direct path. The algorithm alternates
 * between the two graphs. It first uses the normal procedure (as in the unidirectional variant)
 * to try and add a vertex to one graph. If it does not succeed (i.e. there was no free-space in 
 * the expanded direction), it will move to the other graph and try to expand it. If it does succeed,
 * than the last vertex that was added to the tree will be the point towards which the other tree 
 * will be expanded (if free-space permits). In other words, any successful vertex addition to one
 * tree causes the other to attempt an expansion in that direction, and any failure at adding a vertex
 * to one tree causes the other to attempt to expand towards a random point. This version of the 
 * algorithm will also notify the user (via a visitor's callback) whenever one tree was successfully
 * expanded towards the other tree to the point that they meet at two vertices (one on each graph).
 * The user can thus record successful connections as paths and decide whether it's worth continuing 
 * with the generation the Bi-RRT in the hopes of finding a better path with richer trees.
 * 
 * \author Sven Mikael Persson <mikael.s.persson@gmail.com>
 * \date February 2011
 */

/*
 *    Copyright 2011 Sven Mikael Persson
 *
 *    THIS SOFTWARE IS DISTRIBUTED UNDER THE TERMS OF THE GNU GENERAL PUBLIC LICENSE v3 (GPLv3).
 *
 *    This file is part of ReaK.
 *
 *    ReaK is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    ReaK is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with ReaK (as LICENSE in the root folder).  
 *    If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef REAK_RR_TREE_HPP
#define REAK_RR_TREE_HPP

#include <utility>
#include <boost/tuple/tuple.hpp>
#include <boost/graph/graph_concepts.hpp>
#include <boost/property_map/property_map.hpp>

#include "path_planning/metric_space_concept.hpp"
#include "path_planning/random_sampler_concept.hpp"

#include "graph_alg/bgl_tree_adaptor.hpp"
#include "graph_alg/bgl_more_property_maps.hpp"

#include "graph_alg/sbmp_visitor_concepts.hpp"

namespace ReaK {
  
namespace graph {


namespace detail {

  template <typename Graph,
            typename Topology,
            typename RRTVisitor,
            typename PositionMap>
  inline std::pair< typename boost::graph_traits<Graph>::vertex_descriptor, bool>
    expand_rrt_vertex(Graph& g, const Topology& space, RRTVisitor vis, PositionMap position,
                      typename boost::graph_traits<Graph>::vertex_descriptor u,
                      const typename boost::property_traits<PositionMap>::value_type& p_target) {
    typedef typename boost::property_traits<PositionMap>::value_type PositionValue;
    typedef typename boost::graph_traits<Graph>::vertex_descriptor Vertex;
    typedef typename Graph::vertex_bundled VertexProp;
    typedef typename Graph::edge_bundled EdgeProp;
    typedef typename boost::graph_traits<Graph>::edge_descriptor Edge;
    
    bool reached_new; PositionValue p_v; EdgeProp ep;
    boost::tie(p_v, reached_new, ep) = vis.steer_towards_position(p_target,u,g);
    if(reached_new) {  // i.e., a new position was reached, collision-free.
      VertexProp vp; 
      put(position, vp, p_v);
      Vertex v; Edge e;
#ifdef RK_ENABLE_CXX0X_FEATURES
      boost::tie(v,e) = add_child_vertex(u, std::move(vp), std::move(ep), g);
#else
      boost::tie(v,e) = add_child_vertex(u, vp, ep, g);
#endif
      vis.vertex_added(v,g);
      vis.edge_added(e, g);
      u = v;
    };
    return std::make_pair(u,reached_new);
  };
  
  
  template <typename Graph, 
            typename Topology, 
            typename RRTVisitor,
            typename RandomSampler,
            typename PositionMap>
  typename boost::graph_traits<Graph>::vertex_descriptor rrt_get_or_create_root(Graph& g, const Topology& space, RRTVisitor vis, RandomSampler get_sample, PositionMap position) {
    
    typedef typename boost::property_traits<PositionMap>::value_type PositionValue;
    typedef typename boost::graph_traits<Graph>::vertex_descriptor Vertex;
    typedef typename Graph::vertex_bundled VertexProp;
    
    if(num_vertices(g) == 0) {
      PositionValue p = get_sample(space);
      while(!vis.is_position_free(p))
        p = get_sample(space);
      VertexProp up;
      put(position, up, p);
#ifdef RK_ENABLE_CXX0X_FEATURES
      Vertex u = create_root(std::move(up),g);
#else
      Vertex u = create_root(up,g);
#endif
      vis.vertex_added(u, g);
      return u;
    } else {
      return get_root_vertex(g);
    };
  };
  
}; //namespace detail



  /**
   * This function template is the unidirectional version of the RRT algorithm (refer to rr_tree.hpp dox).
   * \tparam Graph A mutable graph type that will represent the generated tree, should model boost::VertexListGraphConcept and boost::MutableGraphConcept
   * \tparam Topology A topology type that will represent the space in which the configurations (or positions) exist, should model BGL's Topology concept
   * \tparam RRTVisitor An RRT visitor type that implements the customizations to this RRT algorithm, should model the RRTVisitorConcept.
   * \tparam PositionMap A property-map type that can store the configurations (or positions) of the vertices.
   * \tparam RandomSampler This is a random-sampler over the topology (see pp::RandomSamplerConcept).
   * \tparam NNFinder A functor type which can perform a nearest-neighbor search of a point to a graph in the topology (see topological_search.hpp).
   * \param g A mutable graph that should initially store the starting 
   *        vertex (if not it will be randomly generated) and will store 
   *        the generated tree once the algorithm has finished.
   * \param space A topology (as defined by the Boost Graph Library). Note 
   *        that it is not required to generate only random points in 
   *        the free-space.
   * \param vis A RRT visitor implementing the RRTVisitorConcept. This is the 
   *        main point of customization and recording of results that the 
   *        user can implement.
   * \param position A mapping that implements the MutablePropertyMap Concept. Also,
   *        the value_type of this map should be the same type as the topology's 
   *        value_type. This map should allow read-write access to the position associated 
   *        to a vertex's property (Graph::vertex_property_type).
   * \param get_sample A random sampler of positions in the free-space (obstacle-free sub-set of the topology).
   * \param find_nearest_neighbor A callable object (functor) which can perform a 
   *        nearest neighbor search of a point to a graph in the topology. (see topological_search.hpp)
   * \param max_vertex_count The maximum number of vertices beyond which the algorithm 
   *        should stop regardless of whether the resulting tree is satisfactory or not.
   * \param max_edge_distance The maximum length (w.r.t. the topology) of an edge.
   * \param min_edge_distance The minimum length (w.r.t. the topology) of an edge. If a free-space 
   *        edge cannot be made longer than this number, it will not be added. This 
   *        parameter also serves as the minimum resolution of the free-query (i.e. 
   *        the collision detection).
   * 
   */
  template <typename Graph,
            typename Topology,
            typename RRTVisitor,
            typename PositionMap,
            typename RandomSampler,
            typename NNFinder>
  inline void generate_rrt(Graph& g,
                           const Topology& space,
                           RRTVisitor vis,
                           PositionMap position,
                           RandomSampler get_sample,
                           NNFinder find_nearest_neighbor,
                           unsigned int max_vertex_count) {
    BOOST_CONCEPT_ASSERT((RRTVisitorConcept<RRTVisitor,Graph,Topology>));
    BOOST_CONCEPT_ASSERT((ReaK::pp::RandomSamplerConcept<RandomSampler,Topology>));
    
    typedef typename boost::property_traits<PositionMap>::value_type PositionValue;
    typedef typename boost::graph_traits<Graph>::vertex_descriptor Vertex;
    
    detail::rrt_get_or_create_root(g, space, vis, get_sample, position);
    
    while((num_vertices(g) < max_vertex_count) && (vis.keep_going())) {
      PositionValue p_rnd = get_sample(space);
      Vertex u = find_nearest_neighbor(p_rnd, g, space, boost::bundle_prop_to_vertex_prop(position, g));
      detail::expand_rrt_vertex(g, space, vis, position, u, p_rnd);
    };

  };
  
  
  struct rrt_generator {
    template <typename Graph,
              typename Topology,
              typename RRTVisitor,
              typename PositionMap,
              typename RandomSampler,
              typename NNFinder>
    void operator()(Graph& g,
                    const Topology& space,
                    RRTVisitor vis,
                    PositionMap position,
                    RandomSampler get_sample,
                    NNFinder find_nearest_neighbor,
                    unsigned int max_vertex_count) {
      generate_rrt(g,space,vis,position,get_sample,find_nearest_neighbor,max_vertex_count);
    };
  };


  /**
   * This function is the bidirectional version of the RRT algorithm (refer to rr_tree.hpp dox).
   * \tparam Graph A mutable graph type that will represent the generated tree, should model boost::VertexListGraphConcept and boost::MutableGraphConcept
   * \tparam Topology A topology type that will represent the space in which the configurations (or positions) exist, should model BGL's Topology concept
   * \tparam BiRRTVisitor A Bi-RRT visitor type that implements the customizations to this Bi-RRT algorithm, should model the BiRRTVisitorConcept.
   * \tparam PositionMap A property-map type that can store the configurations (or positions) of the vertices.
   * \tparam RandomSampler This is a random-sampler over the topology (see pp::RandomSamplerConcept).
   * \tparam NNFinder A functor type which can perform a nearest-neighbor search of a point to a graph in the topology (see topological_search.hpp).
   * \param g1 A mutable graph that should initially store the starting 
   *        vertex (if not it will be randomly generated) and will store 
   *        the generated tree once the algorithm has finished.
   * \param g2 A mutable graph that should initially store the goal 
   *        vertex (if not it will be randomly generated) and will store 
   *        the generated tree once the algorithm has finished.
   * \param space A topology (as defined by the Boost Graph Library). Note 
   *        that it is not required to generate only random points in 
   *        the free-space.
   * \param vis A RRT visitor implementing the RRTVisitorConcept. This is the 
   *        main point of customization and recording of results that the 
   *        user can implement.
   * \param position A mapping for the graph vertex properties that implements the MutablePropertyMap Concept. 
   *        Also, the value_type of this map should be the same type as the topology's 
   *        value_type. This map should allow read-write access to the position associated 
   *        to a vertex's property (Graph::vertex_property_type).
   * \param get_sample A random sampler of positions in the free-space (obstacle-free sub-set of the topology).
   * \param find_nearest_neighbor A callable object (functor) which can perform a 
   *        nearest neighbor search of a point to a graph in the 
   *        topology. (see topological_search.hpp)
   * \param max_vertex_count The maximum number of vertices beyond which the algorithm 
   *        should stop regardless of whether the resulting tree is satisfactory or not.
   * 
   */
  template <typename Graph,
            typename Topology,
            typename BiRRTVisitor,
            typename PositionMap,
            typename RandomSampler,
            typename NNFinder>
  inline void generate_bidirectional_rrt(Graph& g1, Graph& g2,
                                         const Topology& space,
                                         BiRRTVisitor vis,
                                         PositionMap position,
                                         RandomSampler get_sample,
                                         NNFinder find_nearest_neighbor,
                                         unsigned int max_vertex_count) {
    BOOST_CONCEPT_ASSERT((BiRRTVisitorConcept<BiRRTVisitor,Graph,Topology>));
    BOOST_CONCEPT_ASSERT((ReaK::pp::RandomSamplerConcept<RandomSampler,Topology>));
    
    typedef typename boost::property_traits<PositionMap>::value_type PositionValue;
    typedef typename boost::graph_traits<Graph>::vertex_descriptor Vertex;
    
    std::pair<Vertex,bool> v_target2( 
      detail::rrt_get_or_create_root(g1, space, vis, get_sample, position),
      true);
    PositionValue p_target2 = get(position, g1[v_target2.first]);
    
    std::pair<Vertex,bool> v_target1( 
      detail::rrt_get_or_create_root(g2, space, vis, get_sample, position),
      true);
    PositionValue p_target1 = get(position, g2[v_target1.first]);
    
    while((num_vertices(g1) + num_vertices(g2) < max_vertex_count) && (vis.keep_going())) {
      //first, expand the first graph towards its target:
      Vertex u1 = find_nearest_neighbor(p_target1, g1, space, boost::bundle_prop_to_vertex_prop(position, g1));
      std::pair< Vertex, bool> v1 =
        detail::expand_rrt_vertex(g1,space,vis,position,u1,p_target1);
      if((v1.second) && (v_target1.second)) {
        //joining vertex has been reached!
        vis.joining_vertex_found(v1.first, v_target1.first, g1, g2);
        p_target2 = get_sample(space);
        v_target2.second = false;
      } else {
        if(v1.first == u1) { //we didn't move at all! Unsuccessful expansion.
          p_target2 = get_sample(space);
          v_target2.second = false;
        } else {
          p_target2 = get(position, g1[v1.first]);
          v_target2.first = v1.first; v_target2.second = true;
        };
      };

      //then, expand the second graph towards its target:
      Vertex u2 = find_nearest_neighbor(p_target2, g2, space, boost::bundle_prop_to_vertex_prop(position, g2));
      std::pair< Vertex, bool> v2 =
        detail::expand_rrt_vertex(g2,space,vis,position,u2,p_target2);
      if((v2.second) && (v_target2.second)) {
        //joining vertex has been reached!
        vis.joining_vertex_found(v_target2.first, v2.first, g1, g2);
        p_target1 = get_sample(space);
        v_target1.second = false;
      } else {
        if(v2.first == u2) { //we didn't move at all! Unsuccessful expansion.
          p_target1 = get_sample(space);
          v_target1.second = false;
        } else {
          p_target1 = get(position, g2[v2.first]);
          v_target1.first = v2.first; v_target1.second = true;
        };
      };
    };

  };

  struct birrt_generator {
    template <typename Graph,
              typename Topology,
              typename BiRRTVisitor,
              typename PositionMap,
              typename RandomSampler,
              typename NNFinder>
    void operator()(Graph& g1, Graph& g2,
                    const Topology& space,
                    BiRRTVisitor vis,
                    PositionMap position,
                    RandomSampler get_sample,
                    NNFinder find_nearest_neighbor,
                    unsigned int max_vertex_count) {
      generate_bidirectional_rrt(g1,g2,space,vis,position,get_sample,find_nearest_neighbor,max_vertex_count);
    };
  };


};

};


#endif

