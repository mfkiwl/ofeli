/*==============================================================================

                                    O  F  E  L  I

                          Object  Finite  Element  Library

  ==============================================================================

   Copyright (C) 1998 - 2022 Rachid Touzani

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

                       Definition of class LinearSolver
       to select iterative or direct solver for a linear system of equations

  ==============================================================================*/

#ifndef __LINEAR_SOLVER_H
#define __LINEAR_SOLVER_H

#include <string>
#include <iostream>
using std::ostream;
using std::endl;

#include <iomanip>
using std::setw;

#include "OFELI_Config.h"

#if defined (USE_PETSC)
#include "linear_algebra/petsc/PETScMatrix.h"
#elif defined (USE_EIGEN)
#include "linear_algebra/Matrix.h"
#include <Eigen/IterativeLinearSolvers>
using namespace Eigen;
#else
#include "util/util.h"
#include "io/output.h"
#include "solvers/CG.h"
#include "solvers/CGS.h"
#include "solvers/BiCG.h"
#include "solvers/BiCGStab.h"
#include "solvers/GMRes.h"
#endif
#include "OFELIException.h"

namespace OFELI {
/*!
 *  \addtogroup OFELI
 *  @{
 */

#if defined(USE_PETSC)
template<class T_> class PETScMatrix;
#else
template<class T_> class Matrix;
template<class T_> class SpMatrix;
template<class T_> class SkMatrix;
template<class T_> class SkSMatrix;
template<class T_> class TrMatrix;
template<class T_> class BMatrix;
template<class T_> class DMatrix;
template<class T_> class DSMatrix;
#endif

//-----------------------------------------------------------------------------
// Class LinearSolver
//-----------------------------------------------------------------------------

/*! \class LinearSolver
 *  \ingroup Solver
 *  \brief Class to solve systems of linear equations by iterative methods
 */

template<class T_> class LinearSolver
{

 public:

#if defined (USE_EIGEN)
    typedef Eigen::Matrix<T_,Eigen::Dynamic,1> VectorX;
#endif

/// \brief Default Constructor.
/// \details Initializes default parameters and pointers to 0.
    LinearSolver() : _fact(0), _max_it(1000), _matrix_set(0),
                     _s(DIRECT_SOLVER), _p(DIAG_PREC), _toler(sqrt(OFELI_EPSMCH)),
                     _x(nullptr), _b(nullptr), _A(nullptr)
    { }

/** \brief Constructor with iteration parameters
 *  @param [in] max_it Maximal number of iterations
 *  @param [in] tolerance Tolerance for convergence (measured in relative weighted 2-Norm) in input,
 *  effective discrepancy in output.
 *
 * \author Rachid Touzani
 * \copyright GNU Lesser Public License
 */
    LinearSolver(int    max_it,
                 real_t tolerance) : _fact(0), _max_it(max_it),
                                     _matrix_set(0), _s(DIRECT_SOLVER), _p(DIAG_PREC),
                                     _toler(tolerance), _x(nullptr), _b(nullptr), _A(nullptr)
    { }

/** \brief Constructor using matrix, right-hand side and solution vector
 *  @param [in] A Reference to instance of class SpMatrix
 *  @param [in] b Vect instance that contains the right-hand side
 *  @param [in,out] x Vect instance that contains initial guess on input and solution on output
 */
    LinearSolver(SpMatrix<T_>&   A,
                 const Vect<T_>& b,
                 Vect<T_>&       x) : _fact(0), _max_it(1000), _matrix_set(1),
                                      _s(CG_SOLVER), _p(DIAG_PREC), _toler(sqrt(OFELI_EPSMCH)),
                                      _x(&x), _b(&b), _A(&A)
    { }

/** \brief Constructor using skyline-stored matrix, right-hand side and solution vector
 *  @param [in] A SkMatrix instance that contains matrix
 *  @param [in] b Vect instance that contains the right-hand side
 *  @param [in,out] x Vect instance that contains initial guess on input and solution on output
 */
    LinearSolver(SkMatrix<T_>&   A,
                 const Vect<T_>& b,
                 Vect<T_>&       x) : _fact(0), _max_it(1000), _matrix_set(1),
                                      _s(DIRECT_SOLVER), _p(DIAG_PREC), _toler(sqrt(OFELI_EPSMCH)),
                                      _x(&x), _b(&b), _A(&A)
    { }

/** \brief Constructor using a tridiagonal matrix, right-hand side and solution vector
 *  @param [in] A TrMatrix instance that contains matrix
 *  @param [in] b Vect instance that contains the right-hand side
 *  @param [in,out] x Vect instance that contains initial guess on input and solution on output
 */
    LinearSolver(TrMatrix<T_>&   A,
                 const Vect<T_>& b,
                 Vect<T_>&       x) : _fact(0), _max_it(1000), _matrix_set(1),
                                      _s(DIRECT_SOLVER), _p(DIAG_PREC), _toler(sqrt(OFELI_EPSMCH)),
                                      _x(&x), _b(&b), _A(&A)
    { }

/** \brief Constructor using a banded matrix, right-hand side and solution vector
 *  @param [in] A BMatrix instance that contains matrix
 *  @param [in] b Vect instance that contains the right-hand side
 *  @param [in,out] x Vect instance that contains initial guess on input and solution on output
 */
    LinearSolver(BMatrix<T_>&    A,
                 const Vect<T_>& b,
                 Vect<T_>&       x) : _fact(0), _max_it(1000), _matrix_set(1),
                                      _s(DIRECT_SOLVER), _p(DIAG_PREC), _toler(sqrt(OFELI_EPSMCH)),
                                      _x(&x), _b(&b), _A(&A)
    { }

/** \brief Constructor using a dense matrix, right-hand side and solution vector
 *  @param [in] A DMatrix instance that contains matrix
 *  @param [in] b Vect instance that contains the right-hand side
 *  @param [in,out] x Vect instance that contains initial guess on input and solution on output
 */
    LinearSolver(DMatrix<T_>&    A,
                 const Vect<T_>& b,
                 Vect<T_>&       x) : _fact(0), _max_it(1000), _matrix_set(1),
                                      _s(DIRECT_SOLVER), _p(DIAG_PREC), _toler(sqrt(OFELI_EPSMCH)),
                                      _x(&x), _b(&b), _A(&A)
    { }

/** \brief Constructor using a dense symmetric matrix, right-hand side and solution vector
 *  @param [in] A DSMatrix instance that contains matrix
 *  @param [in] b Vect instance that contains the right-hand side
 *  @param [in,out] x Vect instance that contains initial guess on input and solution on output
 */
    LinearSolver(DSMatrix<T_>&   A,
                 const Vect<T_>& b,
                 Vect<T_>&       x) : _fact(0), _max_it(1000), _matrix_set(1),
                                      _s(DIRECT_SOLVER), _p(DIAG_PREC), _toler(sqrt(OFELI_EPSMCH)),
                                      _x(&x), _b(&b), _A(&A)
    { }

/** \brief Constructor using skyline-stored symmetric matrix, right-hand side and solution vector
 *  @param [in] A SkMatrix instance that contains matrix
 *  @param [in] b Vect instance that contains the right-hand side
 *  @param [in,out] x Vect instance that contains initial guess on input and solution on output
 */
    LinearSolver(SkSMatrix<T_>&  A,
                 const Vect<T_>& b,
                 Vect<T_>&       x) : _fact(0), _max_it(1000), _matrix_set(1),
                                      _s(DIRECT_SOLVER), _p(DIAG_PREC), _toler(sqrt(OFELI_EPSMCH)),
                                      _x(&x), _b(&b), _A(&A)
    { }

/** \brief Constructor using matrix, right-hand side
 *  @param [in] A SkMatrix instance that contains matrix
 *  @param [in] b Vect instance that contains the right-hand side
 *  @param [in,out] x Vect instance that contains the initial guess on input and solution on output
 */
    LinearSolver(SkMatrix<T_>& A,
                 Vect<T_>&     b,
                 Vect<T_>&     x) : _fact(0), _max_it(1000), _matrix_set(1),
                                    _s(DIRECT_SOLVER), _p(DIAG_PREC), _toler(sqrt(OFELI_EPSMCH)),
                                    _x(&x), _b(&b), _A(&A)
    { }

/// Destructor
    virtual ~LinearSolver() { }

/// \brief Set Maximum number of iterations
/// \details Default value is 1000
    void setMaxIter(int m) { _max_it = m; }

/// \brief Set tolerance value
    void setTolerance(real_t tol) { _toler = tol; }

/// \brief Set solution vector
    void setSolution(Vect<T_>& x) { _x = &x; }

/// \brief Set right-hand side vector
    void setRHS(Vect<T_>& b) { _b = &b; }

/// \brief Set matrix in the case of a pointer to Matrix
/// @param [in] A Pointer to abstract Matrix class
    void setMatrix(OFELI::Matrix<T_>* A)
    {
       _A = A;
       _matrix_set = 1;
       _fact = 0;
    }

/// \brief Set matrix in the case of a pointer to matrix
/// @param [in] A Pointer to abstract Matrix class
    void setMatrix(SpMatrix<T_>& A)
    {
       _A = &A;
       _matrix_set = 1;
    }

/// \brief Set matrix in the case of a skyline matrix
/// @param [in] A %Matrix as instance of class SkMatrix
    void setMatrix(SkMatrix<T_>& A)
    {
       _A = &A;
       _matrix_set = 1;
       _fact = 0;
    }

/** \brief Set matrix, right-hand side and initial guess
 *  @param [in] A Reference to matrix as a SpMatrix instance
 *  @param [in] b Vector containing right-hand side
 *  @param [in,out] x Vector containing initial guess on input and solution on output
 */
    void set(SpMatrix<T_>&   A,
             const Vect<T_>& b,
             Vect<T_>&       x)
    {
       _A = &A;
       _matrix_set = 1;
       _b = &b;
       _x = &x;
    }

/** \brief Set solver and preconditioner
 *  @param [in] s Solver identification parameter.
 *  To be chosen in the enumeration variable Iteration:\n 
 *  <tt>DIRECT_SOLVER</tt>, <tt>CG_SOLVER</tt>, <tt>CGS_SOLVER</tt>, <tt>BICG_SOLVER</tt>,
 *  <tt>BICG_STAB_SOLVER</tt>, <tt>GMRES_SOLVER</tt>, <tt>QMR_SOLVER</tt>
 *  @param [in] p Preconditioner identification parameter. By default, the diagonal
 *  preconditioner is used.
 *  To be chosen in the enumeration variable Preconditioner:\n
 *  <tt>IDENT_PREC</tt>, <tt>DIAG_PREC</tt>, <tt>SSOR_PREC</tt>, <tt>ILU_PREC</tt> [Default:
 *  <tt>ILU_PREC</tt>]
 *  @note The argument <tt>p</tt> has no effect if the solver is <tt>DIRECT_SOLVER</tt>
 */
    void setSolver(Iteration      s, 
                   Preconditioner p=DIAG_PREC) { _s = s; _p = p; }

/// \brief Return solver code
    Iteration getSolver() const { return _s; }

/// \brief Return solver preconditioner
    Preconditioner getPreconditioner() const { return _p; }

/** \brief Solve equations using system data, prescribed solver and preconditioner
 *  @param [in] A Reference to matrix as a SpMatrix instance
 *  @param [in] b Vector containing right-hand side
 *  @param [in,out] x Vector containing initial guess on input and solution on output
 *  @param [in] s Solver identification parameter
 *  To be chosen in the enumeration variable Iteration:\n 
 *  <tt>DIRECT_SOLVER</tt>, <tt>CG_SOLVER</tt>, <tt>CGS_SOLVER</tt>, <tt>BICG_SOLVER</tt>,
 *  <tt>BICG_STAB_SOLVER</tt>, <tt>GMRES_SOLVER</tt>, <tt>QMR_SOLVER</tt> [Default: <tt>CGS_SOLVER</tt>]
 *  @param [in] p Preconditioner identification parameter.
 *  To be chosen in the enumeration variable Preconditioner:\n
 *  <tt>IDENT_PREC</tt>, <tt>DIAG_PREC</tt>, <tt>SSOR_PREC</tt>, <tt>ILU_PREC</tt>,
 *  <tt>DILU_PREC</tt> [Default: <tt>DIAG_PREC</tt>]
 *  @remark The argument <tt>p</tt> has no effect if the solver is <tt>DIRECT_SOLVER</tt>
 *  @warning If the library <tt>eigen</tt> is used, only the preconditioners
 *  <tt>IDENT_PREC</tt>, <tt>DIAG_PREC</tt> and <tt>ILU_PREC</tt> are available.
 */
    int solve(SpMatrix<T_>&   A,
              const Vect<T_>& b,
              Vect<T_>&       x,
              Iteration       s,
              Preconditioner  p=DIAG_PREC)
    {
       set(A,b,x);
       setSolver(s,p);
       return solve();
    }

/** \brief Solve equations using prescribed solver and preconditioner
 *  @param [in] s Solver identification parameter
 *  To be chosen in the enumeration variable Iteration:\n 
 *  <tt>DIRECT_SOLVER</tt>, <tt>CG_SOLVER</tt>, <tt>CGS_SOLVER</tt>, <tt>BICG_SOLVER</tt>,
 *  <tt>BICG_STAB_SOLVER</tt>, <tt>GMRES_SOLVER</tt>, <tt>QMR_SOLVER</tt> [Default: <tt>CGS_SOLVER</tt>]
 *  @param [in] p Preconditioner identification parameter.
 *  To be chosen in the enumeration variable Preconditioner:\n
 *  <tt>IDENT_PREC</tt>, <tt>DIAG_PREC</tt>, <tt>SSOR_PREC</tt>, <tt>DILU_PREC</tt>,
 *  <tt>ILU_PREC</tt> [Default: <tt>DIAG_PREC</tt>]
 *  @note The argument <tt>p</tt> has no effect if the solver is <tt>DIRECT_SOLVER</tt>
 */
    int solve(Iteration      s,
              Preconditioner p=DIAG_PREC)
    {
       setSolver(s,p);
       return solve();
    }

/** \brief Solve equations all arguments must have been given by other member functions
 *  \details Solver and preconditioner parameters must have been set by function setSolver.
 *  Otherwise, default values are set.
 */
    int solve()
    {
       int ret=0;
       _nb_it = 0;
       if (!_matrix_set)
          throw OFELIException("In LinearSolver::solve(): No matrix has been defined.");
       if (_A->isDiagonal()) {
          if (_x) {
             for (size_t i=1; i<=_A->getNbRows(); i++)
                (*_x)(i) = (*_b)(i)/(*_A)(i,i);
          }
          else {
             for (size_t i=1; i<=_A->getNbRows(); i++)
                (*_x)(i) /= (*_A)(i,i);
          }
          return ret;
       }

#if defined (USE_EIGEN)
       SpMatrix<T_> &A = MAT(SpMatrix<T_>,_A);
       VectorX x;
       if (_s==CG_SOLVER) {
          switch (_p) {

             case IDENT_PREC:
                {
                   ConjugateGradient<SparseMatrix<T_>,Lower,IdentityPreconditioner> im;
                   im.setTolerance(_toler);
                   im.compute(A.getEigenMatrix());
                   x = im.solve(VectorX(*_b));
                   ret = im.iterations();
                   break;
                }

             case DIAG_PREC:
                {
                   ConjugateGradient<SparseMatrix<T_>,Lower,DiagonalPreconditioner<T_> > im;
                   im.setTolerance(_toler);
                   im.compute(A.getEigenMatrix());
                   x = im.solve(VectorX(*_b));
                   ret = im.iterations();
                   break;
                }

             case ILU_PREC:
                {
                   ConjugateGradient<SparseMatrix<T_>,Lower,IncompleteLUT<T_> > im;
                   im.setTolerance(_toler);
                   im.compute(A.getEigenMatrix());
                   x = im.solve(VectorX(*_b));
                   ret = im.iterations();
                   break;
                }

             default:
                throw OFELIException("In LinearSolver::solve(Iteration,Preconditioner): "
                                     "This preconditioner is not available in the eigen library.");
                break;
          }
       }
       else if (_s==BICG_STAB_SOLVER) {
          if (_p==IDENT_PREC) {
             Eigen::BiCGSTAB<SparseMatrix<T_>,IdentityPreconditioner> im;
             im.setTolerance(_toler);
             im.compute(A.getEigenMatrix());
             x = im.solve(VectorX(*_b));
             ret = im.iterations();
          }
          else if (_p==DIAG_PREC) {
             Eigen::BiCGSTAB<SparseMatrix<T_>,DiagonalPreconditioner<T_> > im;
             im.setTolerance(_toler);
             im.compute(A.getEigenMatrix());
             x = im.solve(VectorX(*_b));
             ret = im.iterations();
          }
          else if (_p==ILU_PREC) {
             Eigen::BiCGSTAB<SparseMatrix<T_>,IncompleteLUT<T_> > im;
             im.setTolerance(_toler);
             im.compute(A.getEigenMatrix());
             x = im.solve(VectorX(*_b));
             ret = im.iterations();
         }
         else
             throw OFELIException("In LinearSOlver::solve(Iteration,Preconditioner): "
                                  "This preconditioner is not available in the eigen library.");
       }
       else
          throw OFELIException("In LinearSolver::solve(Iteration,Preconditioner): "
                               "This iterative solver is not available in the eigen library.");
       _x->setSize(x.size(),1,1);
       *_x = x;
#else
       switch (_s) {

          case DIRECT_SOLVER:
             if (_fact)
                _A->Factor();
            _nb_it = _A->solve(*_b,*_x);
             break;

          case CG_SOLVER:
             _nb_it = CG(_A,_p,*_b,*_x,_max_it,_toler);
             break; 

          case BICG_SOLVER:
             _nb_it = BiCG(_A,_p,*_b,*_x,_max_it,_toler);
             break;

          case CGS_SOLVER:
             _nb_it = CGS(_A,_p,*_b,*_x,_max_it,_toler);
             break;

          case BICG_STAB_SOLVER:
             _nb_it = BiCGStab(_A,_p,*_b,*_x,_max_it,_toler);
             break;

          case GMRES_SOLVER:
             _nb_it = GMRes(_A,_p,*_b,*_x,_b->size()/5,_max_it,_toler);
             break;
       }
#endif
       return _nb_it;
    }

/// Factorize matrix
    void setFact() { _fact = 1; }

/// Do not factorize matrix
    void setNoFact() { _fact = 0; }

/// Get number of performed iterations
    int getNbIter() const { return _nb_it; }

 private:

   int            _fact, _max_it, _nb_it, _matrix_set;
   Iteration      _s;
   Preconditioner _p;
   real_t         _toler;
   Vect<T_>       *_x;
   const Vect<T_> *_b;
   Matrix<T_>     *_A;
};

/*! @} End of Doxygen Groups */
} /* namespace OFELI */

#endif
