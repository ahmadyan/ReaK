/**
 * \file discrete_linear_sss_concept.hpp
 * 
 * This library defines a number of traits and concept classes related to the definition 
 * of a discrete-time linear state-space system, with different system types such as a 
 * linear-time-invariant (LTI), linear-time-varying (LTV), and linearized. The main 
 * characteristic of such systems is that they can provide system matrices (A,B,C,D), 
 * whether they are independent of state and time (LTI), independent of state (LTV), 
 * or a linear approximation based on the state and time (Linearized).
 * 
 * \author Sven Mikael Persson <mikael.s.persson@gmail.com>
 * \date May 2011
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

#ifndef DISCRETE_LINEAR_SSS_CONCEPT_HPP
#define DISCRETE_LINEAR_SSS_CONCEPT_HPP


#include <boost/config.hpp>
#include <boost/concept_check.hpp>

#include "discrete_sss_concept.hpp"
#include "linear_ss_system_concept.hpp"

namespace ReaK {

namespace ctrl {

/**
 * This traits class defines the traits of a discrete-time linear state-space system.
 * This traits class only includes the traits that are not already included in the 
 * discrete_sss_traits class.
 * \tparam DiscreteSystem The discrete-time state-space system whose traits are sought.
 */
template <typename DiscreteSystem>
struct discrete_linear_sss_traits {
  typedef typename DiscreteSystem::matrixA_type matrixA_type;
  typedef typename DiscreteSystem::matrixB_type matrixB_type;
  typedef typename DiscreteSystem::matrixC_type matrixC_type;
  typedef typename DiscreteSystem::matrixD_type matrixD_type;
  
};

/**
 * This concept class defines the requirement for a system to be able to provide 
 * linear system matrices for an LTI system, that is, the matrices are independent 
 * of time or state or input.
 * 
 * Valid expression:
 * 
 * sys.get_linear_blocks(A,B,C,D);  The system matrices can be obtained without providing a time or state.
 */
struct DiscreteLTISystemType {
  template <typename System, typename Point, typename Input, typename Time, 
            typename A_t, typename B_t, typename C_t, typename D_t>
  void constraints(const System& sys, const Point&, const Input&, const Time&,  
		   A_t& A, B_t& B, C_t& C, D_t& D) {
    sys.get_linear_blocks(A, B, C, D);
  };
};

/**
 * This concept class defines the requirement for a system to be able to provide 
 * linear system matrices for an LTV system, that is, the matrices are independent 
 * of state or input, but dependent on time. Note that an LTI system is a subset of 
 * an LTV system.
 * 
 * Valid expression:
 * 
 * sys.get_linear_blocks(A,B,C,D,t);  The system matrices can be obtained without providing a state.
 */
struct DiscreteLTVSystemType {
  template <typename System, typename Point, typename Input, typename Time, 
            typename A_t, typename B_t, typename C_t, typename D_t>
  void constraints(const System& sys, const Point&, const Input&, const Time& t, 
		   A_t& A, B_t& B, C_t& C, D_t& D) {
    sys.get_linear_blocks(A, B, C, D, t);
  };
};

/**
 * This concept class defines the requirement for a system to be able to provide 
 * linear system matrices for a linearized system, that is, the matrices are dependent 
 * on time, state and-or input, but dependent on time. Note that an LTI system and LTV 
 * system are subsets of a linearized system.
 * 
 * Valid expression:
 * 
 * sys.get_linear_blocks(A,B,C,D,t,p,u);  The system matrices can be obtained by providing a time, state and input.
 */
struct DiscreteLinearizedSystemType {
  template <typename System, typename Point, typename Input, typename Time, 
            typename A_t, typename B_t, typename C_t, typename D_t>
  void constraints(const System& sys, const Point& p, const Input& u, const Time& t, 
		   A_t& A, B_t& B, C_t& C, D_t& D) {
    sys.get_linear_blocks(A, B, C, D, t, p, u);
  };
};
  
  
/**
 * This concept class defines the requirements for a discrete-time state-space system to be 
 * considered a linear system, as used in ReaK::ctrl. This concept class depends on a helping
 * concept class which is given as SystemType which defines what kind of linear system it is 
 * (DiscreteLTISystemType, DiscreteLTVSystemType or DiscreteLinearizedSystemType).
 * 
 * Required concepts:
 * 
 * The state-space system should model DiscreteSSSConcept.
 * 
 * Valid expressions:
 * 
 * sys_type.constraints(sys, p, u, t, A, B, C, D);  The system should comply to the constraints of the SystemType concept class.
 * 
 * p = A * p + B * u;  The next state can be obtained by linear transformation of the state and input using the system matrices.
 * 
 * y = C * p + D * u;  The output can be obtained by linear transformation of the state and input using the system matrices.
 * 
 * \tparam DiscreteSystem The discrete-time state-space system to be tested for linearity.
 * \tparam SystemType The concept class that tests the system-type, can be either DiscreteLTISystemType, DiscreteLTVSystemType or DiscreteLinearizedSystemType.
 */
template <typename DiscreteSystem, typename SystemType = DiscreteLTISystemType >
struct DiscreteLinearSSSConcept {
  DiscreteSystem sys;
  SystemType sys_type;
  typename discrete_sss_traits<DiscreteSystem>::point_type p;
  typename discrete_sss_traits<DiscreteSystem>::time_type t;
  typename discrete_sss_traits<DiscreteSystem>::input_type u;
  typename discrete_sss_traits<DiscreteSystem>::output_type y;
  
  typename discrete_linear_sss_traits<DiscreteSystem>::matrixA_type A;
  typename discrete_linear_sss_traits<DiscreteSystem>::matrixB_type B;
  typename discrete_linear_sss_traits<DiscreteSystem>::matrixC_type C;
  typename discrete_linear_sss_traits<DiscreteSystem>::matrixD_type D;
  
  void constraints() {
    boost::function_requires< DiscreteSSSConcept<DiscreteSystem> >();
    
    sys_type.constraints(sys, p, u, t, A, B, C, D);
    p = A * p + B * u;
    y = C * p + D * u;
  };
  
};




};

};

#endif








