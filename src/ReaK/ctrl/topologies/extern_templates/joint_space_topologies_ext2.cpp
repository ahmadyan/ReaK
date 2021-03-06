
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

#include "base/defs.hpp"

#if (defined(RK_ENABLE_CXX11_FEATURES) && defined(RK_ENABLE_EXTERN_TEMPLATES))

#include "topologies/joint_space_topologies.hpp"

namespace ReaK {

namespace pp {

// Multiple degrees of freedom (1-10) for 0th-order space:

template class metric_space_tuple< arithmetic_tuple< 
  joint_space_0th_order<double>::type>, euclidean_tuple_distance >;

template class metric_space_tuple< arithmetic_tuple< 
  joint_space_0th_order<double>::type,
  joint_space_0th_order<double>::type>, euclidean_tuple_distance >;

template class metric_space_tuple< arithmetic_tuple< 
  joint_space_0th_order<double>::type,
  joint_space_0th_order<double>::type,
  joint_space_0th_order<double>::type>, euclidean_tuple_distance >;

template class metric_space_tuple< arithmetic_tuple< 
  joint_space_0th_order<double>::type,
  joint_space_0th_order<double>::type,
  joint_space_0th_order<double>::type,
  joint_space_0th_order<double>::type>, euclidean_tuple_distance >;

template class metric_space_tuple< arithmetic_tuple< 
  joint_space_0th_order<double>::type,
  joint_space_0th_order<double>::type,
  joint_space_0th_order<double>::type,
  joint_space_0th_order<double>::type,
  joint_space_0th_order<double>::type>, euclidean_tuple_distance >;

template class metric_space_tuple< arithmetic_tuple< 
  joint_space_0th_order<double>::type,
  joint_space_0th_order<double>::type,
  joint_space_0th_order<double>::type,
  joint_space_0th_order<double>::type,
  joint_space_0th_order<double>::type,
  joint_space_0th_order<double>::type>, euclidean_tuple_distance >;

template class metric_space_tuple< arithmetic_tuple< 
  joint_space_0th_order<double>::type,
  joint_space_0th_order<double>::type,
  joint_space_0th_order<double>::type,
  joint_space_0th_order<double>::type,
  joint_space_0th_order<double>::type,
  joint_space_0th_order<double>::type,
  joint_space_0th_order<double>::type>, euclidean_tuple_distance >;

template class metric_space_tuple< arithmetic_tuple< 
  joint_space_0th_order<double>::type,
  joint_space_0th_order<double>::type,
  joint_space_0th_order<double>::type,
  joint_space_0th_order<double>::type,
  joint_space_0th_order<double>::type,
  joint_space_0th_order<double>::type,
  joint_space_0th_order<double>::type,
  joint_space_0th_order<double>::type>, euclidean_tuple_distance >;

template class metric_space_tuple< arithmetic_tuple< 
  joint_space_0th_order<double>::type,
  joint_space_0th_order<double>::type,
  joint_space_0th_order<double>::type,
  joint_space_0th_order<double>::type,
  joint_space_0th_order<double>::type,
  joint_space_0th_order<double>::type,
  joint_space_0th_order<double>::type,
  joint_space_0th_order<double>::type,
  joint_space_0th_order<double>::type>, euclidean_tuple_distance >;

template class metric_space_tuple< arithmetic_tuple< 
  joint_space_0th_order<double>::type,
  joint_space_0th_order<double>::type,
  joint_space_0th_order<double>::type,
  joint_space_0th_order<double>::type,
  joint_space_0th_order<double>::type,
  joint_space_0th_order<double>::type,
  joint_space_0th_order<double>::type,
  joint_space_0th_order<double>::type,
  joint_space_0th_order<double>::type,
  joint_space_0th_order<double>::type>, euclidean_tuple_distance >;


  
// Multiple degrees of freedom (1-10) for 1st-order space:

template class metric_space_tuple< arithmetic_tuple< 
  joint_space_1st_order<double>::type>, euclidean_tuple_distance >;

template class metric_space_tuple< arithmetic_tuple< 
  joint_space_1st_order<double>::type,
  joint_space_1st_order<double>::type>, euclidean_tuple_distance >;

template class metric_space_tuple< arithmetic_tuple< 
  joint_space_1st_order<double>::type,
  joint_space_1st_order<double>::type,
  joint_space_1st_order<double>::type>, euclidean_tuple_distance >;

template class metric_space_tuple< arithmetic_tuple< 
  joint_space_1st_order<double>::type,
  joint_space_1st_order<double>::type,
  joint_space_1st_order<double>::type,
  joint_space_1st_order<double>::type>, euclidean_tuple_distance >;

template class metric_space_tuple< arithmetic_tuple< 
  joint_space_1st_order<double>::type,
  joint_space_1st_order<double>::type,
  joint_space_1st_order<double>::type,
  joint_space_1st_order<double>::type,
  joint_space_1st_order<double>::type>, euclidean_tuple_distance >;

template class metric_space_tuple< arithmetic_tuple< 
  joint_space_1st_order<double>::type,
  joint_space_1st_order<double>::type,
  joint_space_1st_order<double>::type,
  joint_space_1st_order<double>::type,
  joint_space_1st_order<double>::type,
  joint_space_1st_order<double>::type>, euclidean_tuple_distance >;

template class metric_space_tuple< arithmetic_tuple< 
  joint_space_1st_order<double>::type,
  joint_space_1st_order<double>::type,
  joint_space_1st_order<double>::type,
  joint_space_1st_order<double>::type,
  joint_space_1st_order<double>::type,
  joint_space_1st_order<double>::type,
  joint_space_1st_order<double>::type>, euclidean_tuple_distance >;

template class metric_space_tuple< arithmetic_tuple< 
  joint_space_1st_order<double>::type,
  joint_space_1st_order<double>::type,
  joint_space_1st_order<double>::type,
  joint_space_1st_order<double>::type,
  joint_space_1st_order<double>::type,
  joint_space_1st_order<double>::type,
  joint_space_1st_order<double>::type,
  joint_space_1st_order<double>::type>, euclidean_tuple_distance >;

template class metric_space_tuple< arithmetic_tuple< 
  joint_space_1st_order<double>::type,
  joint_space_1st_order<double>::type,
  joint_space_1st_order<double>::type,
  joint_space_1st_order<double>::type,
  joint_space_1st_order<double>::type,
  joint_space_1st_order<double>::type,
  joint_space_1st_order<double>::type,
  joint_space_1st_order<double>::type,
  joint_space_1st_order<double>::type>, euclidean_tuple_distance >;

template class metric_space_tuple< arithmetic_tuple< 
  joint_space_1st_order<double>::type,
  joint_space_1st_order<double>::type,
  joint_space_1st_order<double>::type,
  joint_space_1st_order<double>::type,
  joint_space_1st_order<double>::type,
  joint_space_1st_order<double>::type,
  joint_space_1st_order<double>::type,
  joint_space_1st_order<double>::type,
  joint_space_1st_order<double>::type,
  joint_space_1st_order<double>::type>, euclidean_tuple_distance >;

  
// Multiple degrees of freedom (1-10) for 1st-order space:

template class metric_space_tuple< arithmetic_tuple< 
  joint_space_2nd_order<double>::type>, euclidean_tuple_distance >;

template class metric_space_tuple< arithmetic_tuple< 
  joint_space_2nd_order<double>::type,
  joint_space_2nd_order<double>::type>, euclidean_tuple_distance >;

template class metric_space_tuple< arithmetic_tuple< 
  joint_space_2nd_order<double>::type,
  joint_space_2nd_order<double>::type,
  joint_space_2nd_order<double>::type>, euclidean_tuple_distance >;

template class metric_space_tuple< arithmetic_tuple< 
  joint_space_2nd_order<double>::type,
  joint_space_2nd_order<double>::type,
  joint_space_2nd_order<double>::type,
  joint_space_2nd_order<double>::type>, euclidean_tuple_distance >;

template class metric_space_tuple< arithmetic_tuple< 
  joint_space_2nd_order<double>::type,
  joint_space_2nd_order<double>::type,
  joint_space_2nd_order<double>::type,
  joint_space_2nd_order<double>::type,
  joint_space_2nd_order<double>::type>, euclidean_tuple_distance >;

template class metric_space_tuple< arithmetic_tuple< 
  joint_space_2nd_order<double>::type,
  joint_space_2nd_order<double>::type,
  joint_space_2nd_order<double>::type,
  joint_space_2nd_order<double>::type,
  joint_space_2nd_order<double>::type,
  joint_space_2nd_order<double>::type>, euclidean_tuple_distance >;

template class metric_space_tuple< arithmetic_tuple< 
  joint_space_2nd_order<double>::type,
  joint_space_2nd_order<double>::type,
  joint_space_2nd_order<double>::type,
  joint_space_2nd_order<double>::type,
  joint_space_2nd_order<double>::type,
  joint_space_2nd_order<double>::type,
  joint_space_2nd_order<double>::type>, euclidean_tuple_distance >;

template class metric_space_tuple< arithmetic_tuple< 
  joint_space_2nd_order<double>::type,
  joint_space_2nd_order<double>::type,
  joint_space_2nd_order<double>::type,
  joint_space_2nd_order<double>::type,
  joint_space_2nd_order<double>::type,
  joint_space_2nd_order<double>::type,
  joint_space_2nd_order<double>::type,
  joint_space_2nd_order<double>::type>, euclidean_tuple_distance >;

template class metric_space_tuple< arithmetic_tuple< 
  joint_space_2nd_order<double>::type,
  joint_space_2nd_order<double>::type,
  joint_space_2nd_order<double>::type,
  joint_space_2nd_order<double>::type,
  joint_space_2nd_order<double>::type,
  joint_space_2nd_order<double>::type,
  joint_space_2nd_order<double>::type,
  joint_space_2nd_order<double>::type,
  joint_space_2nd_order<double>::type>, euclidean_tuple_distance >;

template class metric_space_tuple< arithmetic_tuple< 
  joint_space_2nd_order<double>::type,
  joint_space_2nd_order<double>::type,
  joint_space_2nd_order<double>::type,
  joint_space_2nd_order<double>::type,
  joint_space_2nd_order<double>::type,
  joint_space_2nd_order<double>::type,
  joint_space_2nd_order<double>::type,
  joint_space_2nd_order<double>::type,
  joint_space_2nd_order<double>::type,
  joint_space_2nd_order<double>::type>, euclidean_tuple_distance >;

  
// joint_space_0th_order
template class temporal_space< Ndof_0th_order_space<double, 1>::type, time_poisson_topology, spatial_distance_only>;
template class temporal_space< Ndof_0th_order_space<double, 2>::type, time_poisson_topology, spatial_distance_only>;
template class temporal_space< Ndof_0th_order_space<double, 3>::type, time_poisson_topology, spatial_distance_only>;
template class temporal_space< Ndof_0th_order_space<double, 4>::type, time_poisson_topology, spatial_distance_only>;
template class temporal_space< Ndof_0th_order_space<double, 5>::type, time_poisson_topology, spatial_distance_only>;
template class temporal_space< Ndof_0th_order_space<double, 6>::type, time_poisson_topology, spatial_distance_only>;
template class temporal_space< Ndof_0th_order_space<double, 7>::type, time_poisson_topology, spatial_distance_only>;
template class temporal_space< Ndof_0th_order_space<double, 8>::type, time_poisson_topology, spatial_distance_only>;
template class temporal_space< Ndof_0th_order_space<double, 9>::type, time_poisson_topology, spatial_distance_only>;
template class temporal_space< Ndof_0th_order_space<double,10>::type, time_poisson_topology, spatial_distance_only>;

// joint_space_1st_order
template class temporal_space< Ndof_1st_order_space<double, 1>::type, time_poisson_topology, spatial_distance_only>;
template class temporal_space< Ndof_1st_order_space<double, 2>::type, time_poisson_topology, spatial_distance_only>;
template class temporal_space< Ndof_1st_order_space<double, 3>::type, time_poisson_topology, spatial_distance_only>;
template class temporal_space< Ndof_1st_order_space<double, 4>::type, time_poisson_topology, spatial_distance_only>;
template class temporal_space< Ndof_1st_order_space<double, 5>::type, time_poisson_topology, spatial_distance_only>;
template class temporal_space< Ndof_1st_order_space<double, 6>::type, time_poisson_topology, spatial_distance_only>;
template class temporal_space< Ndof_1st_order_space<double, 7>::type, time_poisson_topology, spatial_distance_only>;
template class temporal_space< Ndof_1st_order_space<double, 8>::type, time_poisson_topology, spatial_distance_only>;
template class temporal_space< Ndof_1st_order_space<double, 9>::type, time_poisson_topology, spatial_distance_only>;
template class temporal_space< Ndof_1st_order_space<double,10>::type, time_poisson_topology, spatial_distance_only>;

// joint_space_2nd_order
template class temporal_space< Ndof_2nd_order_space<double, 1>::type, time_poisson_topology, spatial_distance_only>;
template class temporal_space< Ndof_2nd_order_space<double, 2>::type, time_poisson_topology, spatial_distance_only>;
template class temporal_space< Ndof_2nd_order_space<double, 3>::type, time_poisson_topology, spatial_distance_only>;
template class temporal_space< Ndof_2nd_order_space<double, 4>::type, time_poisson_topology, spatial_distance_only>;
template class temporal_space< Ndof_2nd_order_space<double, 5>::type, time_poisson_topology, spatial_distance_only>;
template class temporal_space< Ndof_2nd_order_space<double, 6>::type, time_poisson_topology, spatial_distance_only>;
template class temporal_space< Ndof_2nd_order_space<double, 7>::type, time_poisson_topology, spatial_distance_only>;
template class temporal_space< Ndof_2nd_order_space<double, 8>::type, time_poisson_topology, spatial_distance_only>;
template class temporal_space< Ndof_2nd_order_space<double, 9>::type, time_poisson_topology, spatial_distance_only>;
template class temporal_space< Ndof_2nd_order_space<double,10>::type, time_poisson_topology, spatial_distance_only>;



};

};

#else

namespace ReaK {

namespace pp {

void dummy_joint_space_topologies_externs_2_symbol() { };

};

};

#endif














