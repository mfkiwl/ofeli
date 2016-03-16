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

                    Implementation of class 'Reconstruction'

  ==============================================================================*/


#include "shape_functions/Line2.h"
#include "shape_functions/Triang3.h"
#include "shape_functions/Quad4.h"
#include "shape_functions/Tetra4.h"
#include "post/Reconstruction.h"

namespace OFELI {

void Reconstruction::P0toP1(const Vect<real_t>& u,
                                  Vect<real_t>& v)
{
   size_t nb_dof = u.getNbDOF();
   v.setSize(_theMesh->getNbNodes(),nb_dof);
   v = 0;
   _M.setSize(_theMesh->getNbNodes());
   _M = 0;
   mesh_elements(*_theMesh) {
      size_t n = element_label;
      try {
         if (_theMesh->getDim()==1 && The_element.getShape()==LINE) {
            real_t a = Line2(the_element).getLength();
            size_t n1=The_element(1)->n(), n2=The_element(2)->n();
            _M(n1) += a; _M(n2) += a;
            for (size_t k=1; k<=nb_dof; k++) {
               v(n1,k) += a*u(n,k);
               v(n2,k) += a*u(n,k);
            }
         }
         else if (_theMesh->getDim()==2 && The_element.getShape()==TRIANGLE) {
            real_t a = Triang3(the_element).getArea();
            size_t n1=The_element(1)->n(), n2=The_element(2)->n(), n3=The_element(3)->n();
            _M(n1) += a; _M(n2) += a; _M(n3) += a;
            for (size_t k=1; k<=nb_dof; k++) {
               v(n1,k) += a*u(n,k);
               v(n2,k) += a*u(n,k);
               v(n3,k) += a*u(n,k);
            }
         }
         else if (_theMesh->getDim()==2 && The_element.getShape()==QUADRILATERAL) {
            Quad4 q(the_element);
	    q.setLocal(Point<double>(0.,0.));
            real_t a = fabs(q.getDet());
            size_t n1=The_element(1)->n(), n2=The_element(2)->n(),
                   n3=The_element(3)->n(), n4=The_element(4)->n();
            _M(n1) += a; _M(n2) += a;
            _M(n3) += a; _M(n4) += a;
            for (size_t k=1; k<=nb_dof; k++) {
               v(n1,k) += a*u(n,k);
               v(n2,k) += a*u(n,k);
               v(n3,k) += a*u(n,k);
               v(n4,k) += a*u(n,k);
            }
         }
         else if (_theMesh->getDim()==3 && The_element.getShape()==TETRAHEDRON) {
            real_t a = fabs(Tetra4(the_element).getVolume());
            size_t n1=The_element(1)->n(), n2=The_element(2)->n(),
                   n3=The_element(3)->n(), n4=The_element(4)->n();
            _M(n1) += a; _M(n2) += a;
            _M(n3) += a; _M(n4) += a;
            for (size_t k=1; k<=nb_dof; k++) {
               v(n1,k) += a*u(n,k);
               v(n2,k) += a*u(n,k);
               v(n3,k) += a*u(n,k);
               v(n4,k) += a*u(n,k);
            }
         }
         else
            THROW_RT("P0toP1(...): Not valid for element: " + itos(element_label));
      }
      CATCH("Reconstruction");
   }
   mesh_nodes(*_theMesh) {
      for (size_t k=1; k<=nb_dof; k++)
         v(node_label,k) /= _M(node_label);
   }
}


void Reconstruction::DP1toP1(const Vect<real_t>& u,
                                   Vect<real_t>& v)
{
   _M.setSize(_theMesh->getNbNodes());
   _M = 0;
   size_t nb_dof = u.getNbDOF();
   v.setSize(_theMesh->getNbNodes(),nb_dof);
   v = 0;
   mesh_elements(*_theMesh) {
      size_t n = element_label;
      try {
         if (_theMesh->getDim()==2 && the_element->getShape()==TRIANGLE) {
            real_t a = OFELI_THIRD*Triang3(the_element).getArea();
            real_t b = 0.25*a;
            size_t n1 = The_element(1)->n(), n2 = The_element(2)->n(), n3 = The_element(3)->n();
            _M.add(n1,a);
            _M.add(n2,a);
            _M.add(n3,a);
            v.add(n1,b*(2*u(n,1)+u(n,2)+u(n,3)));
            v.add(n2,b*(u(n,1)+2*u(n,2)+u(n,3)));
            v.add(n3,b*(u(n,1)+u(n,2)+2*u(n,3)));
         }
         else
            THROW_RT("DP1toP1(...): Not valid for element:" + itos(n));
      }
      CATCH("Reconstruction");
   }
   mesh_nodes(*_theMesh)
      v(node_label) /= _M(node_label);
}

} /* namespace OFELI */