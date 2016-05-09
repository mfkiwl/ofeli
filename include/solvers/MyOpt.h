/*==============================================================================

                                    O  F  E  L  I

                           Object  Finite  Element  Library

  ==============================================================================

   Copyright (C) 1998 - 2016 Rachid Touzani

   This file is part of OFELI.

   OFELI is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   OFELI is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with OFELI. If not, see <http://www.gnu.org/licenses/>.

  ==============================================================================

        Template Class 'MyOpt' for user specified optimization function

  ==============================================================================*/


#ifndef __MY_OPT_H
#define __MY_OPT_H

#include "OFELI_Config.h"
#include "mesh/Mesh.h"
#include "linear_algebra/Vect.h"


namespace OFELI {
/*!
 *  \addtogroup OFELI
 *  @{
 */

/*! \file MyOpt.h
 *  \brief Definition file for abstract class MyOpt.
 */

/*! \class MyOpt
 *  \ingroup Solver
 *  \brief Abstract class to define by user specified optimization function
 *
 * The user has to implement a class that inherits from the present one where
 * the virtual functions are implemented.
 */

class MyOpt
{

 public:

/// \brief Default Constructor
    MyOpt() { }

/// \brief Constructor using mesh instance
/// @param mesh Reference to Mesh instance
    MyOpt(const Mesh& mesh)
    {
       _theMesh = &mesh;
    }

/// \brief Destructor
    virtual ~MyOpt() { }

/** \brief Virtual member function to define objective
 *  @param [in] x Vector of optimization variables
 *  @return Value of objective
 */
    virtual real_t Objective(const Vect<real_t>& x) = 0;

/** \brief Virtual member function to define gradient vector of objective
 *  @param [in] x Vector of optimization variables
 *  @param [out] g Gradient vector
 */
    virtual void Gradient(const Vect<real_t>& x,
                          Vect<real_t>&       g)
    { }

 protected:

#ifndef DOXYGEN_SHOULD_SKIP_THIS
   const Mesh *_theMesh;
#endif /* DOXYGEN_SHOULD_SKIP_THIS */
};

/*! @} End of Doxygen Groups */
} /* namespace OFELI */

#endif