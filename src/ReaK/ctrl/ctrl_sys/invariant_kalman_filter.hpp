
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

#ifndef INVARIANT_KALMAN_FILTER_HPP
#define INVARIANT_KALMAN_FILTER_HPP

#include "belief_state_concept.hpp"
#include "discrete_linear_sss_concept.hpp"
#include "invariant_system_concept.hpp"

#include <boost/utility/enable_if.hpp>
#include <math/vect_concepts.hpp>
#include <math/mat_alg.hpp>
#include <math/mat_cholesky.hpp>

#include <boost/static_assert.hpp>
#include "covariance_concept.hpp"

namespace ReaK {

namespace ctrl {


template <typename InvariantSystem, 
          typename BeliefState, 
	  typename SystemNoiseCovariance,
	  typename MeasurementNoiseCovariance>
typename boost::enable_if_c< is_continuous_belief_state<BeliefState>::value &&
                             (belief_state_traits<BeliefState>::representation == belief_representation::gaussian) &&
                             (belief_state_traits<BeliefState>::distribution == belief_distribution::unimodal),
void >::type invariant_kalman_filter_step(const InvariantSystem& sys,
					  BeliefState& b,
					  const typename discrete_sss_traits<InvariantSystem>::input_type& u,
					  const typename discrete_sss_traits<InvariantSystem>::output_type& z,
					  const SystemNoiseCovariance& Q,
					  const MeasurementNoiseCovariance& R,
					  typename discrete_sss_traits<InvariantSystem>::time_type t = 0) {
  //here the requirement is that the system models a linear system which is at worse a linearized system
  // - if the system is LTI or LTV, then this will result in a basic Kalman Filter (KF) update
  // - if the system is linearized, then this will result in an Extended Kalman Filter (EKF) update
  boost::function_requires< DiscreteLinearSSSConcept< InvariantSystem, DiscreteLinearizedSystemType > >();
  boost::function_requires< InvariantSystemConcept<InvariantSystem> >();
  boost::function_requires< ContinuousBeliefStateConcept<BeliefState> >();

  typedef typename discrete_sss_traits<InvariantSystem>::point_type StateType;
  typedef typename discrete_sss_traits<InvariantSystem>::point_difference_type StateDiffType;
  typedef typename discrete_sss_traits<InvariantSystem>::output_type OutputType;
  typedef typename continuous_belief_state_traits<BeliefState>::covariance_type CovType;
  typedef typename covariance_mat_traits< CovType >::matrix_type MatType;
  typedef typename mat_traits<MatType>::value_type ValueType;
  
  typename discrete_linear_sss_traits<InvariantSystem>::matrixA_type A;
  typename discrete_linear_sss_traits<InvariantSystem>::matrixB_type B;
  typename discrete_linear_sss_traits<InvariantSystem>::matrixC_type C;
  typename discrete_linear_sss_traits<InvariantSystem>::matrixD_type D;
  
  typename invariant_system_traits<InvariantSystem>::invariant_frame_type W;
  typename invariant_system_traits<InvariantSystem>::output_error_type e;
  
  StateType x = b.get_mean_state();
  MatType P = b.get_covariance().get_matrix();
  sys.get_linear_blocks(A, B, C, D, t, x, u);
  
  x = sys.get_next_state(x,u,t);
  P = ( A * P * transpose(A)) + Q.get_matrix();
  
  sys.get_output_error(e, t + sys.get_time_step(), x, u, z);
  sys.get_invariant_frame(W, t + sys.get_time_step(), x);
  
  mat< ValueType, mat_structure::rectangular, mat_alignment::column_major > CP = C * P;
  mat< ValueType, mat_structure::symmetric > S(CP * transpose(C) + R.get_matrix());
  linsolve_Cholesky(S,CP);
  mat< ValueType, mat_structure::rectangular, mat_alignment::row_major > K = transpose_move(CP);
   
  b.set_mean_state( x + W * K * e );
  b.set_covariance( CovType( MatType( (mat< ValueType, mat_structure::identity>(K.get_row_count()) - K * C) * P ) ) );
};




};

};


#endif









