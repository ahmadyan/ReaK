/**
 * \file vect_distance_metrics.hpp
 * 
 * This library defines the distance metrics classes that work on points of a topology that are readable vectors.
 * All the classes satisfy the DistanceMetricConcept.
 * 
 * \author Sven Mikael Persson <mikael.s.persson@gmail.com>
 * \date October 2011
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

#ifndef REAK_VECT_DISTANCE_METRICS_HPP
#define REAK_VECT_DISTANCE_METRICS_HPP

#include "base/serializable.hpp"

#include <cmath>
#include "lin_alg/vect_concepts.hpp"

#include "path_planning/metric_space_concept.hpp"

namespace ReaK {

namespace pp {
  



/**
 * This class is a Manhattan distance metric functor which models the DistanceMetricConcept.
 * This class will simply apply the Manhattan norm to the point-difference vectors in the 
 * given topology (assuming it models the MetricSpaceConcept, and that the point-differences model the ReadableVectorConcept).
 */
struct manhattan_distance_metric : public serialization::serializable {
  
  manhattan_distance_metric() { };
  
  template <typename Topology>
  manhattan_distance_metric(const Topology&) { };
  
  /** 
   * This function returns the norm of a difference between two points on a topology.
   * \tparam PointDiff The point-difference-type, should model the ReadableVectorConcept.
   * \tparam Topology The topology.
   * \param a The point-difference.
   * \return The norm of the difference between two points on a topology.
   */
  template <typename PointDiff, typename Topology>
  typename boost::enable_if< ReaK::is_readable_vector<PointDiff>,
  double >::type operator()(const PointDiff& a, const Topology&) const {
    typedef typename ReaK::vect_traits<PointDiff>::size_type SizeType;
    using std::fabs;
    double result = 0.0;
    for(SizeType i = 0; i < a.size(); ++i)
      result += fabs(a[i]);
    return result;
  };
  
  /** 
   * This function returns the distance between two points on a topology.
   * \tparam Point The point-type.
   * \tparam Topology The topology.
   * \param a The first point.
   * \param b The second point.
   * \param s The topology or space on which the points lie.
   * \return The distance between two points on a topology.
   */
  template <typename Point, typename Topology>
  double operator()(const Point& a, const Point& b, const Topology& s) const {
    return this->operator()(s.difference(a, b),s);
  };
      
/*******************************************************************************
                   ReaK's RTTI and Serialization interfaces
*******************************************************************************/
    
  virtual void RK_CALL save(serialization::oarchive& A, unsigned int) const { };

  virtual void RK_CALL load(serialization::iarchive& A, unsigned int) { };

  RK_RTTI_MAKE_ABSTRACT_1BASE(manhattan_distance_metric,0xC2410001,1,"manhattan_distance_metric",serialization::serializable)
};

typedef manhattan_distance_metric norm1_distance_metric;


/**
 * This class is a Euclidean distance metric functor which models the DistanceMetricConcept.
 * This class will simply apply the Euclidean norm to the point-difference vectors in the 
 * given topology (assuming it models the MetricSpaceConcept, and that the point-differences model the ReadableVectorConcept).
 */
struct euclidean_distance_metric : public serialization::serializable {
  
  euclidean_distance_metric() { };
  
  template <typename Topology>
  euclidean_distance_metric(const Topology&) { };
  
  /** 
   * This function returns the norm of a difference between two points on a topology.
   * \tparam PointDiff The point-difference-type, should model the ReadableVectorConcept.
   * \tparam Topology The topology.
   * \param a The point-difference.
   * \return The norm of the difference between two points on a topology.
   */
  template <typename PointDiff, typename Topology>
  typename boost::enable_if< ReaK::is_readable_vector<PointDiff>,
  double >::type operator()(const PointDiff& a, const Topology&) const {
    typedef typename ReaK::vect_traits<PointDiff>::size_type SizeType;
    typedef typename ReaK::vect_traits<PointDiff>::value_type ValueType;
    ValueType result = ValueType(0);
    for(SizeType i = 0; i < a.size(); ++i)
      result += a[i] * a[i];
    using std::sqrt;
    return sqrt(result);
  };
  
  /** 
   * This function returns the distance between two points on a topology.
   * \tparam Point The point-type.
   * \tparam Topology The topology.
   * \param a The first point.
   * \param b The second point.
   * \param s The topology or space on which the points lie.
   * \return The distance between two points on a topology.
   */
  template <typename Point, typename Topology>
  double operator()(const Point& a, const Point& b, const Topology& s) const {
    return this->operator()(s.difference(a, b),s);
  };
      
/*******************************************************************************
                   ReaK's RTTI and Serialization interfaces
*******************************************************************************/
    
  virtual void RK_CALL save(serialization::oarchive& A, unsigned int) const { };

  virtual void RK_CALL load(serialization::iarchive& A, unsigned int) { };

  RK_RTTI_MAKE_ABSTRACT_1BASE(euclidean_distance_metric,0xC2410002,1,"euclidean_distance_metric",serialization::serializable)
};

typedef euclidean_distance_metric norm2_distance_metric;


/**
 * This class is a Infinity-norm distance metric functor which models the DistanceMetricConcept.
 * This class will simply apply the Infinity-norm to the point-difference vectors in the 
 * given topology (assuming it models the MetricSpaceConcept, and that the point-differences model the ReadableVectorConcept).
 */
struct inf_norm_distance_metric : public serialization::serializable {
  
  inf_norm_distance_metric() { };
  
  template <typename Topology>
  inf_norm_distance_metric(const Topology&) { };
  
  /** 
   * This function returns the norm of a difference between two points on a topology.
   * \tparam PointDiff The point-difference-type, should model the ReadableVectorConcept.
   * \tparam Topology The topology.
   * \param a The point-difference.
   * \return The norm of the difference between two points on a topology.
   */
  template <typename PointDiff, typename Topology>
  typename boost::enable_if< ReaK::is_readable_vector<PointDiff>,
  double >::type operator()(const PointDiff& a, const Topology&) const {
    typedef typename ReaK::vect_traits<PointDiff>::size_type SizeType;
    using std::fabs;
    double result = 0.0;
    for(SizeType i = 0; i < a.size(); ++i) {
      if(result < fabs(a[i]))
	result = fabs(a[i]);
    };
    return result;
  };
  
  /** 
   * This function returns the distance between two points on a topology.
   * \tparam Point The point-type.
   * \tparam Topology The topology.
   * \param a The first point.
   * \param b The second point.
   * \param s The topology or space on which the points lie.
   * \return The distance between two points on a topology.
   */
  template <typename Point, typename Topology>
  double operator()(const Point& a, const Point& b, const Topology& s) const {
    return this->operator()(s.difference(a, b),s);
  };
      
/*******************************************************************************
                   ReaK's RTTI and Serialization interfaces
*******************************************************************************/
    
  virtual void RK_CALL save(serialization::oarchive& A, unsigned int) const { };

  virtual void RK_CALL load(serialization::iarchive& A, unsigned int) { };

  RK_RTTI_MAKE_ABSTRACT_1BASE(inf_norm_distance_metric,0xC2410003,1,"inf_norm_distance_metric",serialization::serializable)
};


/**
 * This class is a Euclidean distance metric functor which models the DistanceMetricConcept.
 * This class will simply apply the Euclidean norm to the point-difference vectors in the 
 * given topology (assuming it models the MetricSpaceConcept, and that the point-differences model the ReadableVectorConcept).
 */
struct p_norm_distance_metric : public serialization::serializable {
  
  int p_value;
  
  p_norm_distance_metric(int aP = 2) : p_value(aP) { };
  
  template <typename Topology>
  p_norm_distance_metric(const Topology&, int aP = 2) : p_value(aP) { };
  
  /** 
   * This function returns the norm of a difference between two points on a topology.
   * \tparam PointDiff The point-difference-type, should model the ReadableVectorConcept.
   * \tparam Topology The topology.
   * \param a The point-difference.
   * \return The norm of the difference between two points on a topology.
   */
  template <typename PointDiff, typename Topology>
  typename boost::enable_if< ReaK::is_readable_vector<PointDiff>,
  double >::type operator()(const PointDiff& a, const Topology&) const {
    typedef typename ReaK::vect_traits<PointDiff>::size_type SizeType;
    using std::pow;
    using std::fabs;
    double result = 0.0;
    for(SizeType i = 0; i < a.size(); ++i)
      result += pow(fabs(a[i]),p_value);
    return pow(result,1.0 / p_value);
  };
  
  /** 
   * This function returns the distance between two points on a topology.
   * \tparam Point The point-type.
   * \tparam Topology The topology.
   * \param a The first point.
   * \param b The second point.
   * \param s The topology or space on which the points lie.
   * \return The distance between two points on a topology.
   */
  template <typename Point, typename Topology>
  double operator()(const Point& a, const Point& b, const Topology& s) const {
    return this->operator()(s.difference(a, b),s);
  };
      
/*******************************************************************************
                   ReaK's RTTI and Serialization interfaces
*******************************************************************************/
    
  virtual void RK_CALL save(serialization::oarchive& A, unsigned int) const { 
    A & RK_SERIAL_SAVE_WITH_NAME(p_value);
  };

  virtual void RK_CALL load(serialization::iarchive& A, unsigned int) { 
    A & RK_SERIAL_LOAD_WITH_NAME(p_value);
  };

  RK_RTTI_MAKE_ABSTRACT_1BASE(p_norm_distance_metric,0xC2410004,1,"p_norm_distance_metric",serialization::serializable)
};


};

};


#endif


