/**
 * \file prismatic_joint.hpp
 *
 * This library declares the KTE models for prismatic joints in 2D and 3D space. These
 * models implement a model of a single degree-of-freedom linear joint (i.e. prismatic joint),
 * allowing no rotation and only displacement along its predefined, fixed axis.
 *
 * \author Mikael Persson, <mikael.s.persson@gmail.com>
 * \date March 2010
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

#ifndef PRISMATIC_JOINT_HPP
#define PRISMATIC_JOINT_HPP

#include "reacting_kte.hpp"
#include "math/motion_jacobians.hpp"

#include <boost/shared_ptr.hpp>

namespace ReaK {

namespace kte {


/**
 * This class implements a prismatic joint in 2D space. A generalized coordinate is used to represent the
 * joint's linear displacement between a base coordinate frame to an end coordinate frame.
 */
class prismatic_joint_2D : public reacting_kte_gen {
  protected:
    boost::shared_ptr< gen_coord<double> > mCoord; ///< Generalized coordinate representing the joint's linear displacement.
    vect<double,2> mAxis; ///< Joint axis, as a fixed vector, in the coordinate system of the base frame.
    boost::shared_ptr< frame_2D<double> > mBase; ///< The coordinate frame at the base of the joint.
    boost::shared_ptr< frame_2D<double> > mEnd; ///< The coordinate frame just after the joint transformations are applied.

    boost::shared_ptr< jacobian_gen_2D<double> > mJacobian; ///< The Jacobian frame produced by this joint.

  public:

    /** Get read-write access to the joint axis mAxis. */
    vect<double,2>& Axis() { return mAxis; };
    /** Get read-only access to the joint axis mAxis. */
    const vect<double,2>& Axis() const { return mAxis; };

    /**
     * Default constructor.
     */
    prismatic_joint_2D(const std::string& aName = "") : reacting_kte_gen(aName), mCoord(), mAxis(), mBase(), mEnd(), mJacobian() { };

    /**
     * Parametrized constructor.
     * \param aName the name of this KTE model.
     * \param aCoord the generalized coordinate associated with the displacement of this joint.
     * \param aAxis the joint axis along which linear displacement occurs, represented in base frame coordinates.
     * \param aBase the coordinate frame at the base of the joint.
     * \param aEnd the coordinate frame just after the joint transformations are applied.
     * \param aJacobian a pointer to contain the Jacobian frame produced by this joint, default value will disable the Jacobian frame's calculation.
     */
    prismatic_joint_2D(const std::string& aName,
		       boost::shared_ptr< gen_coord<double> > aCoord,
		       const vect<double,2>& aAxis,
		       boost::shared_ptr< frame_2D<double> > aBase,
		       boost::shared_ptr< frame_2D<double> > aEnd,
                       boost::shared_ptr< jacobian_gen_2D<double> > aJacobian = boost::shared_ptr< jacobian_gen_2D<double> >()) :
		       reacting_kte_gen(aName),
		       mCoord(aCoord),
		       mAxis(aAxis),
		       mBase(aBase),
		       mEnd(aEnd),
                       mJacobian(aJacobian) { };

    /**
     * Default destructor.
     */
    virtual ~prismatic_joint_2D() { };

    virtual void doMotion(kte_pass_flag aFlag = nothing, boost::shared_ptr<frame_storage> aStorage = boost::shared_ptr<frame_storage>());

    virtual void doForce(kte_pass_flag aFlag = nothing, boost::shared_ptr<frame_storage> aStorage = boost::shared_ptr<frame_storage>());

    virtual void clearForce();

    virtual void applyReactionForce(double aForce);

    virtual void RK_CALL save(serialization::oarchive& A, unsigned int Version) const {
      reacting_kte_gen::save(A,reacting_kte_gen::getStaticObjectType()->TypeVersion());
      A & RK_SERIAL_SAVE_WITH_NAME(mCoord)
        & RK_SERIAL_SAVE_WITH_NAME(mAxis)
        & RK_SERIAL_SAVE_WITH_NAME(mBase)
        & RK_SERIAL_SAVE_WITH_NAME(mEnd)
	& RK_SERIAL_SAVE_WITH_NAME(mJacobian);
    };

    virtual void RK_CALL load(serialization::iarchive& A, unsigned int Version) {
      reacting_kte_gen::load(A,reacting_kte_gen::getStaticObjectType()->TypeVersion());
      A & RK_SERIAL_LOAD_WITH_NAME(mCoord)
        & RK_SERIAL_LOAD_WITH_NAME(mAxis)
        & RK_SERIAL_LOAD_WITH_NAME(mBase)
        & RK_SERIAL_LOAD_WITH_NAME(mEnd)
	& RK_SERIAL_LOAD_WITH_NAME(mJacobian);
    };

    RK_RTTI_MAKE_CONCRETE_1BASE(prismatic_joint_2D,0xC2100005,1,"prismatic_joint_2D",reacting_kte_gen)

};

/**
 * This class implements a prismatic joint in 3D space. A generalized coordinate is used to represent the
 * joint's linear displacement between a base coordinate frame to an end coordinate frame.
 */
class prismatic_joint_3D : public reacting_kte_gen {
  protected:
    boost::shared_ptr< gen_coord<double> > mCoord; ///< Generalized coordinate representing the joint's linear displacement.
    vect<double,3> mAxis; ///< Joint axis, as a fixed vector, in the coordinate system of the base frame.
    boost::shared_ptr< frame_3D<double> > mBase; ///< The coordinate frame at the base of the joint.
    boost::shared_ptr< frame_3D<double> > mEnd; ///< The coordinate frame just after the joint transformations are applied.

    boost::shared_ptr< jacobian_gen_3D<double> > mJacobian; ///< The Jacobian frame produced by this joint.

  public:

    /** Get read-write access to the joint axis mAxis. */
    vect<double,3>& Axis() { return mAxis; };
    /** Get read-only access to the joint axis mAxis. */
    const vect<double,3>& Axis() const { return mAxis; };

    /**
     * Default constructor.
     */
    prismatic_joint_3D(const std::string& aName = "") : reacting_kte_gen(aName), mCoord(), mAxis(), mBase(), mEnd(), mJacobian() { };

    /**
     * Parametrized constructor.
     * \param aName the name of this KTE model.
     * \param aCoord the generalized coordinate associated with the displacement of this joint.
     * \param aAxis the joint axis along which linear displacement occurs, represented in base frame coordinates.
     * \param aBase the coordinate frame at the base of the joint.
     * \param aEnd the coordinate frame just after the joint transformations are applied.
     * \param aJacobian a pointer to contain the Jacobian frame produced by this joint, default value will disable the Jacobian frame's calculation.
     */
    prismatic_joint_3D(const std::string& aName,
		       boost::shared_ptr< gen_coord<double> > aCoord,
		       const vect<double,3>& aAxis,
		       boost::shared_ptr< frame_3D<double> > aBase,
		       boost::shared_ptr< frame_3D<double> > aEnd,
                       boost::shared_ptr< jacobian_gen_3D<double> > aJacobian = boost::shared_ptr< jacobian_gen_3D<double> >()) :
		       reacting_kte_gen(aName),
		       mCoord(aCoord),
		       mAxis(aAxis),
		       mBase(aBase),
		       mEnd(aEnd),
                       mJacobian(aJacobian){ };

    /**
     * Default destructor.
     */
    virtual ~prismatic_joint_3D() { };

    virtual void doMotion(kte_pass_flag aFlag = nothing, boost::shared_ptr<frame_storage> aStorage = boost::shared_ptr<frame_storage>());

    virtual void doForce(kte_pass_flag aFlag = nothing, boost::shared_ptr<frame_storage> aStorage = boost::shared_ptr<frame_storage>());

    virtual void clearForce();

    virtual void applyReactionForce(double aForce);

    virtual void RK_CALL save(serialization::oarchive& A, unsigned int) const {
      reacting_kte_gen::save(A,reacting_kte_gen::getStaticObjectType()->TypeVersion());
      A & RK_SERIAL_SAVE_WITH_NAME(mCoord)
        & RK_SERIAL_SAVE_WITH_NAME(mAxis)
        & RK_SERIAL_SAVE_WITH_NAME(mBase)
        & RK_SERIAL_SAVE_WITH_NAME(mEnd)
	& RK_SERIAL_SAVE_WITH_NAME(mJacobian);
    };

    virtual void RK_CALL load(serialization::iarchive& A, unsigned int) {
      reacting_kte_gen::load(A,reacting_kte_gen::getStaticObjectType()->TypeVersion());
      A & RK_SERIAL_LOAD_WITH_NAME(mCoord)
        & RK_SERIAL_LOAD_WITH_NAME(mAxis)
        & RK_SERIAL_LOAD_WITH_NAME(mBase)
        & RK_SERIAL_LOAD_WITH_NAME(mEnd)
	& RK_SERIAL_LOAD_WITH_NAME(mJacobian);
    };

    RK_RTTI_MAKE_CONCRETE_1BASE(prismatic_joint_3D,0xC2100006,1,"prismatic_joint_3D",reacting_kte_gen)

};




};

};


#endif









