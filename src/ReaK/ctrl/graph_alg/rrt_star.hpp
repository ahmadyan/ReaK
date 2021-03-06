/**
 * \file rrt_star.hpp
 * 
 * This library contains the Rapidly-Exploring Random Tree generation algorithm with an
 * A* rewiring policy.
 * This is a method to create a random tree that will span over a non-convex space
 * as rapidly as possible. The method relies on a simple randomized insertion algorithm.
 * At each step, a random point is picked from the underlying topology (i.e. configuration 
 * space in path-planning terms). Then, the points in the current graph that are nearest 
 * to the random point are picked for expansion. Then, edges (of a maximum length) are 
 * added to the nearest vertex towards the random point while it is still possible to
 * add such an edge without leaving the free space (the part of the configuration space which 
 * is not occupied by an obstacle). Finally, the last point of the expansion is used to attempt 
 * the same expansion with the other nearest neighbors and the accumulated cost-to-go of all 
 * these alternate paths are compared and the edge that leads to the shortest accumulated cost-to-go
 * is selected as a new edge in the tree. The algorithm will stop when either the number of vertices
 * in the tree has reached a maximum or when the user callback signals the stop.
 * 
 * This library also provides the bidirectional version of the RRT* algorithm. In this version,
 * two trees are generated. Typically, one tree is initialized with the starting vertex and 
 * the other is initialized with the goal vertex. The algorithm works to try and join the 
 * two graphs as quickly as possible and with the most direct path. The algorithm alternates
 * between the two graphs. It first uses the normal procedure (as in the unidirectional variant)
 * to try and add a vertex to one graph. If it does not succeed (i.e. there was no free-space in 
 * the expanded direction), it will try to expand the other graph. If it does succeed,
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
 * \date May 2012
 */

/*
 *    Copyright 2012 Sven Mikael Persson
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

#ifndef REAK_RRT_STAR_HPP
#define REAK_RRT_STAR_HPP

#include <utility>
#include <limits>
#include <boost/tuple/tuple.hpp>
#include <boost/graph/graph_concepts.hpp>
#include <boost/property_map/property_map.hpp>

#include "path_planning/metric_space_concept.hpp"
#include "path_planning/random_sampler_concept.hpp"

#include "sbmp_visitor_concepts.hpp"
#include "neighborhood_functors.hpp"
#include "node_generators.hpp"

#include "lazy_connector.hpp"
#include "branch_and_bound_connector.hpp"

namespace ReaK {
  
namespace graph {

  
namespace detail {
  
  
  template <typename RRGVisitor,
            typename PositionMap, 
            typename WeightMap,
            typename DistanceMap,  
            typename PredecessorMap>
  struct rrt_conn_visitor
  {
    
    rrt_conn_visitor(RRGVisitor vis, PositionMap pos, WeightMap weight, 
                     DistanceMap dist, PredecessorMap pred) : 
                     m_vis(vis), m_position(pos), m_weight(weight), 
                     m_distance(dist), m_predecessor(pred) { };
    
    typedef typename boost::property_traits<PositionMap>::value_type PositionValue;
    
    template <class Graph>
    typename boost::graph_traits<Graph>::vertex_descriptor create_vertex(const PositionValue& p, Graph& g) const {
      typedef typename boost::graph_traits<Graph>::vertex_descriptor Vertex;
      typedef typename Graph::vertex_bundled VertexProp;
      
      VertexProp up;
      put(m_position, up, p);
      put(m_distance, up, std::numeric_limits<double>::infinity());
#ifdef RK_ENABLE_CXX0X_FEATURES
      Vertex u = add_vertex(std::move(up), g);
#else
      Vertex u = add_vertex(up, g);
#endif
      put(m_predecessor, g[u], u);
      m_vis.vertex_added(u,g);
      
      return u;
    };
    
    template <typename Vertex, typename Graph>
    void vertex_to_be_removed(Vertex u, Graph& g) const {
      m_vis.vertex_to_be_removed(u,g);
    };
    
    template <typename Vertex, typename Graph>
    void vertex_added(Vertex v, Graph& g) const { m_vis.vertex_added(v,g); };
    template <typename Edge, typename Graph>
    void edge_added(Edge e, Graph& g) const { m_vis.edge_added(e,g); };
    
    template <typename Vertex, typename Graph>
    void travel_explored(Vertex u, Vertex v, Graph& g) const { };
    template <typename Vertex, typename Graph>
    void travel_succeeded(Vertex u, Vertex v, Graph& g) const { };
    template <typename Vertex, typename Graph>
    void travel_failed(Vertex u, Vertex v, Graph& g) const { };
    template <typename Vertex, typename Graph>
    void affected_vertex(Vertex, Graph&) const { };
    
    bool keep_going() const { return m_vis.keep_going(); };
    
    template <typename Vertex, typename Graph>
    std::pair<bool, typename Graph::edge_bundled> can_be_connected(Vertex u, Vertex v, Graph& g) const { 
      return m_vis.can_be_connected(u,v,g);
    };
    
    template <typename PositionValue, typename Vertex, typename Graph>
    boost::tuple<PositionValue, bool, typename Graph::edge_bundled> steer_towards_position(const PositionValue& p, Vertex u, Graph& g) const { 
      return m_vis.steer_towards_position(p, u, g);
    };
    
    RRGVisitor m_vis;
    PositionMap m_position;
    WeightMap m_weight;  // needed by generate_rrt_star_loop (given to connector call)
    DistanceMap m_distance;
    PredecessorMap m_predecessor;
  };
  
  
  template <typename Graph,
            typename Topology,
            typename RRTStarConnVisitor,
            typename MotionGraphConnector,
            typename PositionMap,
            typename NodeGenerator,
            typename NcSelector>
  inline void generate_rrt_star_loop(Graph& g,
                                     const Topology& super_space,
                                     RRTStarConnVisitor conn_vis,
                                     MotionGraphConnector connect_vertex,
                                     PositionMap position,
                                     NodeGenerator node_generator_func,
                                     NcSelector select_neighborhood,
                                     std::size_t max_vertex_count) {
    typedef typename boost::property_traits<PositionMap>::value_type PositionValue;
    typedef typename boost::graph_traits<Graph>::vertex_descriptor Vertex;
    typedef typename Graph::edge_bundled EdgeProp;
    
    while((num_vertices(g) < max_vertex_count) && (conn_vis.keep_going())) {
      
      PositionValue p_new; Vertex x_near; EdgeProp eprop;
      boost::tie(x_near, p_new, eprop) = node_generator_func(g, conn_vis, boost::bundle_prop_to_vertex_prop(position, g));
      
      if((x_near != boost::graph_traits<Graph>::null_vertex()) && 
          (get(conn_vis.m_distance, g[x_near]) != std::numeric_limits<double>::infinity())) {
        connect_vertex(p_new, x_near, eprop, g, 
                       super_space, conn_vis, conn_vis.m_position, 
                       conn_vis.m_distance, conn_vis.m_predecessor, 
                       conn_vis.m_weight, select_neighborhood);
      };
      
    };

  };
  
  
  template <typename Graph,
            typename Topology,
            typename RRGVisitor,
            typename PositionMap,
            typename CostMap,
            typename PredecessorMap,
            typename WeightMap,
            typename NodeGenerator,
            typename NcSelector>
  inline void generate_rrt_star_loop(Graph& g,
                                     const Topology& super_space,
                                     RRGVisitor vis,
                                     PositionMap position,
                                     CostMap cost,
                                     PredecessorMap pred,
                                     WeightMap weight,
                                     NodeGenerator node_generator_func,
                                     NcSelector select_neighborhood,
                                     std::size_t max_vertex_count) {
    
    rrt_conn_visitor<RRGVisitor, PositionMap, WeightMap, CostMap, PredecessorMap> 
      conn_vis(vis, position, weight, cost, pred);
    
    generate_rrt_star_loop(g, super_space, conn_vis, lazy_node_connector(),
                           position, node_generator_func, select_neighborhood, max_vertex_count);
    
  };
  
  
  
  
  

};




/**
  * This function template is the RRT* algorithm (refer to rrt_star.hpp dox).
  * \tparam Graph A mutable graph type that will represent the generated tree, should model boost::VertexListGraphConcept and boost::MutableGraphConcept
  * \tparam Topology A topology type that will represent the space in which the configurations (or positions) exist, should model BGL's Topology concept
  * \tparam RRGVisitor An RRT* visitor type that implements the customizations to this RRT* algorithm, should model the RRGVisitorConcept.
  * \tparam PositionMap A property-map type that can store the configurations (or positions) of the vertices.
  * \tparam CostMap This property-map type is used to store the estimated cost-to-go of each vertex to the start (or goal).
  * \tparam PredecessorMap This property-map type is used to store the predecessor of each vertex.
  * \tparam WeightMap This property-map type is used to store the weights of the edges of the graph (cost of travel along an edge).
  * \tparam RandomSampler This is a random-sampler over the topology (see pp::RandomSamplerConcept).
  * \tparam NcSelector A functor type which can perform a neighborhood search of a point to a graph in the topology (see topological_search.hpp).
  * \param g A mutable graph that should initially store the starting and goal 
  *        vertex and will store the generated graph once the algorithm has finished.
  * \param super_space A topology (as defined by the Boost Graph Library). Note 
  *        that it should represent the entire configuration space (not collision-free space).
  * \param vis A RRT* visitor implementing the RRGVisitorConcept. This is the 
  *        main point of customization and recording of results that the 
  *        user can implement.
  * \param position A mapping that implements the MutablePropertyMap Concept. Also,
  *        the value_type of this map should be the same type as the topology's 
  *        value_type.
  * \param cost The property-map which stores the estimated cost-to-go of each vertex to the start (or goal).
  * \param pred The property-map which stores the predecessor of each vertex.
  * \param weight The property-map which stores the weight of each edge of the graph (the cost of travel
  *        along the edge).
  * \param get_sample A random sampler of positions in the free-space (obstacle-free sub-set of the topology).
  * \param select_neighborhood A callable object (functor) which can perform a 
  *        nearest neighbor search of a point to a graph in the topology. (see star_neighborhood)
  * \param max_vertex_count The maximum number of vertices beyond which the algorithm 
  *        should stop regardless of whether the resulting tree is satisfactory or not.
  * 
  */
template <typename Graph,
          typename Topology,
          typename RRGVisitor,
          typename PositionMap,
          typename CostMap,
          typename PredecessorMap,
          typename WeightMap,
          typename RandomSampler,
          typename NcSelector>
inline void generate_rrt_star(Graph& g,
                              const Topology& super_space,
                              RRGVisitor vis,
                              PositionMap position,
                              CostMap cost,
                              PredecessorMap pred,
                              WeightMap weight,
                              RandomSampler get_sample,
                              NcSelector select_neighborhood,
                              unsigned int max_vertex_count) {
  BOOST_CONCEPT_ASSERT((RRGVisitorConcept<RRGVisitor,Graph,Topology>));
  BOOST_CONCEPT_ASSERT((ReaK::pp::MetricSpaceConcept<Topology>));
  BOOST_CONCEPT_ASSERT((ReaK::pp::RandomSamplerConcept<RandomSampler,Topology>));
  
  typedef typename boost::property_traits<PositionMap>::value_type PositionValue;
  typedef typename boost::graph_traits<Graph>::vertex_descriptor Vertex;
  typedef typename Graph::vertex_bundled VertexProp;
  
  if(num_vertices(g) == 0) {
    PositionValue p = get_sample(super_space);
    while(!vis.is_position_free(p))
      p = get_sample(super_space);
    VertexProp up;
    put(position, up, p);
    put(cost, up, 0.0);
    put(pred, up, boost::graph_traits<Graph>::null_vertex());
#ifdef RK_ENABLE_CXX0X_FEATURES
    Vertex u = add_vertex(std::move(up),g);
#else
    Vertex u = add_vertex(up,g);
#endif
    vis.vertex_added(u, g);
  };
  
  detail::rrt_conn_visitor<RRGVisitor, PositionMap, WeightMap, CostMap, PredecessorMap> 
    conn_vis(vis, position, weight, cost, pred);
  
  detail::generate_rrt_star_loop(
    g, super_space, conn_vis,
    lazy_node_connector(),
    position,
    rrg_node_generator<Topology, RandomSampler, NcSelector>(&super_space, get_sample, select_neighborhood),
    select_neighborhood, max_vertex_count);
  
};





/**
  * This function template is the RRT* algorithm (refer to rrt_star.hpp dox).
  * This function uses a branch-and-bound heuristic to limit the number of nodes.
  * \tparam Graph A mutable graph type that will represent the generated tree, should model boost::VertexListGraphConcept and boost::MutableGraphConcept
  * \tparam Topology A topology type that will represent the space in which the configurations (or positions) exist, should model BGL's Topology concept
  * \tparam RRGVisitor An RRT* visitor type that implements the customizations to this RRT* algorithm, should model the RRGVisitorConcept.
  * \tparam PositionMap A property-map type that can store the configurations (or positions) of the vertices.
  * \tparam CostMap This property-map type is used to store the estimated cost-to-go of each vertex to the start (or goal).
  * \tparam PredecessorMap This property-map type is used to store the predecessor of each vertex.
  * \tparam WeightMap This property-map type is used to store the weights of the edges of the graph (cost of travel along an edge).
  * \tparam RandomSampler This is a random-sampler over the topology (see pp::RandomSamplerConcept).
  * \tparam NcSelector A functor type which can perform a neighborhood search of a point to a graph in the topology (see topological_search.hpp).
  * \param g A mutable graph that should initially store the starting and goal 
  *        vertex and will store the generated graph once the algorithm has finished.
  * \param start_vertex The vertex from which the motion-graph is grown.
  * \param goal_vertex The vertex which we want to connect to the motion-graph.
  * \param super_space A topology (as defined by the Boost Graph Library). Note 
  *        that it should represent the entire configuration space (not collision-free space).
  * \param vis A RRT* visitor implementing the RRGVisitorConcept. This is the 
  *        main point of customization and recording of results that the 
  *        user can implement.
  * \param position A mapping that implements the MutablePropertyMap Concept. Also,
  *        the value_type of this map should be the same type as the topology's 
  *        value_type.
  * \param cost The property-map which stores the estimated cost-to-go of each vertex to the start (or goal).
  * \param pred The property-map which stores the predecessor of each vertex.
  * \param weight The property-map which stores the weight of each edge of the graph (the cost of travel
  *        along the edge).
  * \param get_sample A random sampler of positions in the free-space (obstacle-free sub-set of the topology).
  * \param select_neighborhood A callable object (functor) which can perform a 
  *        nearest neighbor search of a point to a graph in the topology. (see star_neighborhood)
  * \param max_vertex_count The maximum number of vertices beyond which the algorithm 
  *        should stop regardless of whether the resulting tree is satisfactory or not.
  * 
  */
template <typename Graph,
          typename Topology,
          typename RRGVisitor,
          typename PositionMap,
          typename CostMap,
          typename PredecessorMap,
          typename WeightMap,
          typename RandomSampler,
          typename NcSelector>
inline void generate_bnb_rrt_star(Graph& g,
                                  typename boost::graph_traits<Graph>::vertex_descriptor start_vertex,
                                  typename boost::graph_traits<Graph>::vertex_descriptor goal_vertex,
                                  const Topology& super_space,
                                  RRGVisitor vis,
                                  PositionMap position,
                                  CostMap cost,
                                  PredecessorMap pred,
                                  WeightMap weight,
                                  RandomSampler get_sample,
                                  NcSelector select_neighborhood,
                                  unsigned int max_vertex_count) {
  BOOST_CONCEPT_ASSERT((RRGVisitorConcept<RRGVisitor,Graph,Topology>));
  BOOST_CONCEPT_ASSERT((ReaK::pp::MetricSpaceConcept<Topology>));
  BOOST_CONCEPT_ASSERT((ReaK::pp::RandomSamplerConcept<RandomSampler,Topology>));
  
  typedef typename boost::property_traits<PositionMap>::value_type PositionValue;
  typedef typename boost::graph_traits<Graph>::vertex_descriptor Vertex;
  typedef typename Graph::vertex_bundled VertexProp;
  
  if( (num_vertices(g) == 0) ||
      ( start_vertex == boost::graph_traits<Graph>::null_vertex() ) ||
      ( goal_vertex  == boost::graph_traits<Graph>::null_vertex() ) ) {
    generate_rrt_star(g,super_space,vis,position,cost,pred,weight,get_sample,select_neighborhood,max_vertex_count);
    return;
  };
  
  detail::rrt_conn_visitor<RRGVisitor, PositionMap, WeightMap, CostMap, PredecessorMap> 
    conn_vis(vis, position, weight, cost, pred);
  
  detail::generate_rrt_star_loop(
    g, super_space, conn_vis,
    branch_and_bound_connector<Graph>(
      g,
      start_vertex, 
      goal_vertex
    ), 
    position,
    rrg_node_generator<Topology, RandomSampler, NcSelector>(&super_space, get_sample, select_neighborhood),
    select_neighborhood, max_vertex_count);
  
};







};

};


#endif

