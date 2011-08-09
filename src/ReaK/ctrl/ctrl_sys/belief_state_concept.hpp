/**
 * \file belief_state_concept.hpp
 * 
 * This library defines a number of concept and traits class templates related to 
 * the definition of a belief state and the transitions between them. Belief states 
 * are the foundation of estimation theory. A belief state represents the estimate 
 * of a state of a system by representing the state as a probability distribution of 
 * the state (e.g. a gaussian belief state would hold the mean state and the covariance
 * of that estimate).
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

#ifndef BELIEF_STATE_CONCEPT_HPP
#define BELIEF_STATE_CONCEPT_HPP

#include <boost/concept_check.hpp>
#include "state_vector_concept.hpp"
#include "covariance_concept.hpp"
#include "state_space_sys_concept.hpp"

namespace ReaK {

namespace ctrl {

/** This namespace includes a tag that tells whether a belief representation is unimodal or multimodal */
namespace belief_distribution {
  /** This tag tells whether a belief representation is unimodal or multimodal */
  enum tag {
    unimodal = 1,
    multimodal
  };
};

/** This namespace includes a tag that tells whether a belief representation is gaussian or point-based (and possibly other in the future) */
namespace belief_representation {
  /** This tag tells whether a belief representation is gaussian or point-based (and possibly other in the future) */
  enum tag {
    gaussian = 1,
    point_based
  };
};


/**
 * This traits class defines the traits that describe a belief-state.
 * \tparam BeliefState The belief-state type for which the traits are sought.
 */
template <typename BeliefState>
struct belief_state_traits {
  
  /** The state-vector type, should model StateVectorConcept. */
  typedef typename BeliefState::state_type state_type;
  /** The scalar type which is arithmetically compatible with the state vector. */
  typedef typename BeliefState::scalar_type scalar_type;
  /** The sampler type for the belief state, a sampler can produce random state-vectors using the probability distribution of the belief-state. */
  typedef typename BeliefState::random_sampler_type random_sampler_type;
  /** The probability distribution function type for the belief state, a pdf can give the probability of a state-vector using the probability distribution of the belief-state. */
  typedef typename BeliefState::pdf_type pdf_type;
  
  /** This constant defines the distribution tag of the belief-state. */
  BOOST_STATIC_CONSTANT(belief_distribution::tag, distribution = BeliefState::distribution);
  /** This constant defines the representation tag of the belief-state. */
  BOOST_STATIC_CONSTANT(belief_representation::tag, representation = BeliefState::representation);
  
};


/**
 * This concept class checks that a belief-state models the required concept as used in ReaK::ctrl.
 * 
 * Require concepts:
 * 
 * state-vector type associated to the belief-state should model StateVectorConcept.
 * 
 * Valid expressions:
 * 
 * p = b.get_pdf();  The PDF can be obtained from the belief-state.
 * 
 * s = p(v);  The probability of a state-vector can be obtained from the PDF functor.
 * 
 * v = b.get_most_likely_state();  The most likely state can be obtained from the belief-state.
 * 
 * rs = b.get_random_sampler();  The belief-state sampler can be obtained from the belief-state.
 * 
 * v = rs();  A state vector can be obtained, randomly, from the belief-state sampler.
 * 
 * \tparam BeliefState The belief-state type to be tested against the belief-state concept.
 */
template <typename BeliefState>
struct BeliefStateConcept {
  BeliefState b;
  typename belief_state_traits<BeliefState>::state_type v;
  typename belief_state_traits<BeliefState>::scalar_type s;
  typename belief_state_traits<BeliefState>::pdf_type p;
  typename belief_state_traits<BeliefState>::random_sampler_type rs;
  
  void constraints() {
    boost::function_requires< StateVectorConcept< typename belief_state_traits<BeliefState>::state_type > >();
    
    p = b.get_pdf();
    s = p(v);
    v = b.get_most_likely_state();
    rs = b.get_random_sampler();
    v = rs();
  };
  
};


/**
 * This meta-function evaluates if a belief-state models the BeliefStateConcept. This meta-function
 * does not attempt to instantiate the BeliefStateConcept class because that would break Sfinae 
 * functionality. Instead, if one creates a new belief-state type, one should specialize this 
 * meta-function to return true.
 * \tparam BeliefState The belief-state type to be tested.
 */
template <typename BeliefState>
struct is_belief_state {
  BOOST_STATIC_CONSTANT(bool, value = false);
  typedef is_belief_state<BeliefState> type;
};


/**
 * This class template defines the traits that a continuous belief-state should have. A 
 * continuous belief state is characterized by the fact that the state variables are continuous
 * (as opposed to discrete).
 * \tparam ContBeliefState
 */
template <typename ContBeliefState>
struct continuous_belief_state_traits {
  
  /** The state-vector type, should model StateVectorConcept. */
  typedef typename ContBeliefState::state_type state_type;
  /** The state-difference type, as defined by state_vector_traits. */
  typedef typename ContBeliefState::state_difference_type state_difference_type;
  /** The size type, as defined by state_vector_traits. */
  typedef typename ContBeliefState::size_type size_type;
  /** The scalar type which is arithmetically compatible with the state vector. */
  typedef typename ContBeliefState::scalar_type scalar_type;
  /** The sampler type for the belief state, a sampler can produce random state-vectors using the probability distribution of the belief-state. */
  typedef typename ContBeliefState::random_sampler_type random_sampler_type;
  /** The probability distribution function type for the belief state, a pdf can give the probability of a state-vector using the probability distribution of the belief-state. */
  typedef typename ContBeliefState::pdf_type pdf_type;
  
  /** The covariance matrix type which can represent the covariance of the belief-state, should model CovarianceMatrixConcept. */
  typedef typename ContBeliefState::covariance_type covariance_type;
  
};


/**
 * This concept class checks that a continuous belief-state models the required concept as used in ReaK::ctrl.
 * 
 * Require concepts:
 * 
 * state-vector type associated to the belief-state should model StateVectorConcept.
 * 
 * covariance type associated to the belief-state should model CovarianceMatrixConcept.
 * 
 * the belief-state should model BeliefStateConcept.
 * 
 * Valid expressions (in addition to those of BeliefStateConcept):
 * 
 * v = b.get_mean_state();  The mean-state can be obtained from the belief-state.
 * 
 * c = b.get_covariance();  The covariance matrix can be obtained from the belief-state.
 * 
 * s = diff(v,v) * ( c.get_inverse_matrix() * diff(v,v) );  The covariance matrix can be multiplied with the state-difference type.
 * 
 * b.set_mean_state(v);  The mean-state can be set for the belief-state.
 * 
 * b.set_covariance(c);  The covariance matrix can be set for the belief-state.
 *
 * \tparam ContBeliefState The continuous belief-state type to be tested against the continuous belief-state concept.
 */
template <typename ContBeliefState>
struct ContinuousBeliefStateConcept {
  ContBeliefState b;
  typename continuous_belief_state_traits<ContBeliefState>::state_type v;
  typename continuous_belief_state_traits<ContBeliefState>::scalar_type s;
  typename continuous_belief_state_traits<ContBeliefState>::covariance_type c;
  
  void constraints() {
    boost::function_requires< BeliefStateConcept<ContBeliefState> >();
    boost::function_requires< CovarianceMatrixConcept< typename continuous_belief_state_traits<ContBeliefState>::covariance_type > >();
    v = b.get_mean_state();
    c = b.get_covariance();
    
    s = diff(v,v) * ( c.get_inverse_matrix() * diff(v,v) );
    
    b.set_mean_state(v);
    b.set_covariance(c);
  };
  
};


/**
 * This meta-function evaluates if a belief-state models the ContinuousBeliefStateConcept. This meta-function
 * does not attempt to instantiate the ContinuousBeliefStateConcept class because that would break Sfinae 
 * functionality. Instead, if one creates a new continuous belief-state type, one should specialize this 
 * meta-function to return true.
 * \tparam BeliefState The continuous belief-state type to be tested.
 */
template <typename BeliefState>
struct is_continuous_belief_state {
  BOOST_STATIC_CONSTANT(bool, value = false);
  typedef is_continuous_belief_state<BeliefState> type;
};




/**
 * This traits class defines the traits that a belief transfer function should have. A 
 * belief transfer function can be used to model the transmission of a belief-state from 
 * time to time.
 * \tparam BeliefTransfer The belief transfer function for which the traits are sought.
 */
template <typename BeliefTransfer>
struct belief_transfer_traits {
  /** The belief-state type associated to the transfer function */
  typedef typename BeliefTransfer::belief_state belief_state;
  /** The state-space system type associated to the transfer function */
  typedef typename BeliefTransfer::state_space_system state_space_system;
  /** The time type associated to the transfer function */
  typedef typename BeliefTransfer::time_type time_type;
  /** The time-difference type associated to the transfer function */
  typedef typename BeliefTransfer::time_difference_type time_difference_type;
};


/**
 * This concept class defines the requirements for a type to be a belief transfer function type. A 
 * belief transfer function can be used to model the transmission of a belief-state from 
 * time to time.
 * 
 * Required concepts:
 * 
 * The associated belief state should model BeliefStateConcept.
 * 
 * The associated state-space system should model SSSystemConcept.
 * 
 * Valid expressions:
 * 
 * dt = f.get_time_step();  The time-step of the transfer function can be obtained.
 * 
 * sys = f.get_ss_system();  The state-space system can be obtained from the transfer function.
 * 
 * b = f.get_next_belief(b, t, u, y);  The next belief state (b) can be obtained from the current belief-state (b), current time (t), current input (u), and next measurement (y).
 *
 * \tparam BeliefTransfer The belief transfer function type to be checked.
 */
template <typename BeliefTransfer>
struct BeliefTransferConcept {
  BeliefTransfer f;
  typedef typename belief_transfer_traits< BeliefTransfer >::belief_state BeliefState;
  typedef typename belief_transfer_traits< BeliefTransfer >::state_space_system StateSpaceSystem;
  typedef typename belief_transfer_traits< BeliefTransfer >::time_type TimeType;
  typedef typename belief_transfer_traits< BeliefTransfer >::time_difference_type TimeDiffType;
  typename ss_system_traits<SSSystem>::input_type u;
  typename ss_system_traits<SSSystem>::output_type y;
  BeliefState b;
  StateSpaceSystem sys;
  TimeType t;
  TimeDiffType dt;

  void constraints() {
    boost::function_requires< BeliefStateConcept< BeliefState > >();
    boost::function_requires< SSSystemConcept< StateSpaceSystem > >();

    dt = f.get_time_step();
    sys = f.get_ss_system();
    b = f.get_next_belief(b, t, u, y);
  };
};

/**
 * This meta-function evaluates if a belief transfer models the BeliefTransferConcept. This meta-function
 * does not attempt to instantiate the BeliefTransferConcept class because that would break Sfinae 
 * functionality. Instead, if one creates a new belief transfer type, one should specialize this 
 * meta-function to return true.
 * \tparam BeliefTransfer The belief transfer function type to be tested.
 */
template <typename BeliefTransfer>
struct is_belief_transfer {
  BOOST_STATIC_CONSTANT(bool, value = false);
  typedef is_belief_transfer<BeliefTransfer> type;
};







/**
 * This concept class defines the requirements for a type to be a belief predictor function type. A 
 * belief predictor function can be used to model the prediction of a belief-state from 
 * time to time.
 * 
 * Required concepts:
 * 
 * The associated belief state should model BeliefStateConcept.
 * 
 * The associated state-space system should model SSSystemConcept.
 * 
 * The belief predictor should also model BeliefTransferConcept.
 * 
 * Valid expressions:
 * 
 * b = f.predict_belief(b, t, u);  The next predicted belief state (b) can be obtained from the current belief-state (b), current time (t), and current input (u).
 * 
 * b = f.prediction_to_ML_belief(b, t, u);  The most-likely updated belief-state can be obtained from the predicted belief-state (b), at time (t) with the current input (u).
 * 
 * b = f.predict_ML_belief(b, t, u);  The next most-likely updated belief-state (b) can be obtained from the current belief-state (b), current time (t), and current input (u).
 *
 * \tparam BeliefPredictor The belief predictor function type to be checked.
 */
template <typename BeliefPredictor>
struct BeliefPredictorConcept {
  BeliefPredictor f;
  typedef typename belief_transfer_traits< BeliefPredictor >::belief_state BeliefState;
  typedef typename belief_transfer_traits< BeliefPredictor >::state_space_system StateSpaceSystem;
  typedef typename belief_transfer_traits< BeliefPredictor >::time_type TimeType;
  typedef typename belief_transfer_traits< BeliefPredictor >::time_difference_type TimeDiffType;
  typename ss_system_traits<StateSpaceSystem>::input_type u;
  typename ss_system_traits<StateSpaceSystem>::output_type y;
  BeliefState b;
  StateSpaceSystem sys;
  TimeType t;
  TimeDiffType dt;

  void constraints() {
    boost::function_requires< BeliefStateConcept< BeliefState > >();
    boost::function_requires< SSSystemConcept< StateSpaceSystem > >();
    boost::function_requires< BeliefTransferConcept< BeliefPredictor > >();

    b = f.predict_belief(b, t, u);
    b = f.prediction_to_ML_belief(b, t, u);
    b = f.predict_ML_belief(b, t, u);
  };
};

/**
 * This meta-function evaluates if a belief predictor models the BeliefPredictorConcept. This meta-function
 * does not attempt to instantiate the BeliefPredictorConcept class because that would break Sfinae 
 * functionality. Instead, if one creates a new belief predictor type, one should specialize this 
 * meta-function to return true.
 * \tparam BeliefTransfer The belief predictor function type to be tested.
 */
template <typename BeliefPredictor>
struct is_belief_predictor {
  BOOST_STATIC_CONSTANT(bool, value = false);
  typedef is_belief_predictor<BeliefPredictor> type;
};




};

};

#endif













