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

                         Implementation of class 'Prescription'

  ==============================================================================*/

#include "io/Prescription.h"
#include "shape_functions/Line2.h"
#include "mesh/MeshUtil.h"
#include "equations/AbsEqua.h"
#include "io/XMLParser.h"

namespace OFELI {

Prescription::Prescription()
{
}


Prescription::Prescription(      Mesh&   mesh,
                           const string& file)
{
   _theMesh = &mesh;
   _file = file;
}


Prescription::~Prescription() { }


int Prescription::get(int           type,
                      Vect<real_t>& v,
                      real_t        time,
                      size_t        dof)
{
   _data[0] = _data[1] = _data[2] = 0.0;
   _data[3] = time;
   PrescriptionPar par;
   par.dof = dof;
   int ret;
   v = 0;

   XMLParser p(_file,*_theMesh,XMLParser::PRESCRIBE);
   p.get(type,_p);
   for (size_t k=0; k<_p.size(); k++) {
      try {
         if ((ret=PARSE(_p[k].fct,"x,y,z,t")) != -1)
            THROW_RT("get(int,Vect<real_t>,real_t,size_t): Error in expression parsing.");
      }
      CATCH("Prescription");
      if (dof) {
         if (type==BOUNDARY_CONDITION)
            get_boundary_condition(v,k,dof);
         else if (type==BOUNDARY_FORCE)
            get_boundary_force(v,k,dof);
         else if (type==BODY_FORCE)
            get_body_force(v,k,dof);
         else if (type==INITIAL_FIELD)
            get_initial(v,k,dof);
         else if (type==POINT_FORCE)
            get_point_force(v,k,dof);
         else
            ;
      }
      else {
         if (type==BOUNDARY_CONDITION)
            get_boundary_condition(v,k);
         else if (type==BOUNDARY_FORCE)
            get_boundary_force(v,k);
         else if (type==BODY_FORCE)
            get_body_force(v,k);
         else if (type==INITIAL_FIELD)
            get_initial(v,k);
         else if (type==POINT_FORCE)
            get_point_force(v,k);
      }
   }
   return 0;
}


void Prescription::get_point_force(Vect<real_t>& v,
                                   size_t        k)
{
   int err;
   bool p = _p[k].bx || _p[k].by || _p[k].bz;
   mesh_nodes(*_theMesh) {
      _data[0] = the_node->getX();
      _data[1] = the_node->getY();
      _data[2] = the_node->getZ();
      for (size_t i=1; i<=The_node.getNbDOF(); i++) {
         real_t e = 0;
         if (_p[k].bx)
            e = (_p[k].x-_data[0])*(_p[k].x-_data[0]);
         if (_p[k].by)
            e += (_p[k].y-_data[1])*(_p[k].y-_data[1]);
         if (_p[k].bz)
            e += (_p[k].z-_data[2])*(_p[k].z-_data[2]);
         if (i==_p[k].dof && sqrt(e)<OFELI_EPSMCH && p) {
	    v(node_label,i) = EVAL(_data);
            try {
               if ((err=EVAL_ERR))
                  THROW_RT("get_point_force(Vect<real_t>,size_t): Error in expression evaluation.");
            }
            CATCH("Prescription");
         }
      }
   }
}


void Prescription::get_point_force(Vect<real_t>& v,
                                   size_t        k,
                                   size_t        dof)
{
   int err;
   mesh_nodes(*_theMesh) {
      real_t x = _data[0] = The_node.getX();
      real_t y = _data[1] = The_node.getY();
      real_t z = _data[2] = The_node.getZ();
      real_t e = 0;
      if (_p[k].bx)
         e += (_p[k].x-x)*(_p[k].x-x);
      if (_p[k].by)
         e += (_p[k].y-y)*(_p[k].y-y);
      if (_p[k].bz)
         e += (_p[k].z-z)*(_p[k].z-z);
      if (sqrt(e)<OFELI_EPSMCH && _p[k].dof==dof) {
         v(node_label,dof) = EVAL(_data);
         try {
            if ((err=EVAL_ERR))
               THROW_RT("get_point_force(Vect<real_t>,size_t,size_t): Error in expression evaluation.");
         }
         CATCH("Prescription");
      }
   }
}


void Prescription::get_boundary_condition(Vect<real_t>& v,
                                          size_t        k,
                                          size_t        dof)
{
   int err;
   mesh_nodes(*_theMesh) {
      _data[0] = the_node->getX();
      _data[1] = the_node->getY();
      _data[2] = the_node->getZ();
      if (the_node->getCode(dof)==_p[k].code) {
         v(node_label,dof) = EVAL(_data);
         try {
            if ((err=EVAL_ERR))
               THROW_RT("get_boundary_condition(Vect<real_t>,size_t,size_t): Error in expression evaluation.");
         }
         CATCH("Prescription");
      }
   }
}


void Prescription::get_boundary_condition(Vect<real_t>& v,
                                          size_t        k)
{
   size_t l=0;
   int err;
   mesh_nodes(*_theMesh) {
      _data[0] = the_node->getX();
      _data[1] = the_node->getY();
      _data[2] = the_node->getZ();
      for (size_t i=1; i<=the_node->getNbDOF(); i++) {
         l++;
         if (the_node->getCode(_p[k].dof)==_p[k].code && i==_p[k].dof) {
            v(l) = EVAL(_data);
            try {
               if ((err=EVAL_ERR))
                  THROW_RT("get_boundary_condition(Vect<real_t>,size_t): Error in expression evaluation.");
            }
            CATCH("Prescription");
         }
      }
   }
}


void Prescription::get_boundary_force(Vect<real_t>& v,
                                      size_t        k,
                                      size_t        dof)
{
   int err;
   size_t i;
   mesh_sides(*_theMesh) {
      if (The_side.getCode(_p[k].dof)==_p[k].code && (_p[k].dof==dof||dof==0)) {
         _data[0] = _data[1] = _data[2] = 0;
         size_t n = The_side.getNbNodes();
         for (i=0; i<n; i++) {
            the_node = The_side(i+1);
            _data[0] = the_node->getX();
            _data[1] = the_node->getY();
            _data[2] = the_node->getZ();
         }
         _data[0] /= n, _data[1] /= n, _data[2] /= n;
         real_t z = EVAL(_data);
         for (i=1; i<=n; i++)
            v(The_side(i)->n()) = z;
         try {
            if ((err=EVAL_ERR))
               THROW_RT("get_boundary_force(Vect<real_t>,size_t,size_t): Error in expression evaluation.");
         }
         CATCH("Prescription");
      }
   }
}


void Prescription::get_boundary_force(Vect<real_t>& v,
                                      size_t        k)
{
   int err;
   mesh_sides(*_theMesh) {
      for (size_t j=1; j<=The_side.getNbDOF(); j++) {
         if (The_side.getCode(_p[k].dof)==_p[k].code && j==_p[k].dof) {
            _data[0] = _data[1] = _data[2] = 0;
            size_t n = The_side.getNbNodes();
            for (size_t i=1; i<=n; i++) {
	       the_node = The_side(i);
               _data[0] = the_node->getX();
               _data[1] = the_node->getY();
               _data[2] = the_node->getZ();
            }
            _data[0] /= n, _data[1] /= n, _data[2] /= n;
            v(The_side.getDOF(j)) = EVAL(_data);
            try {
                 if ((err=EVAL_ERR))
                    THROW_RT("get_boundary_force(Vect<real_t>,size_t): Error in expression evaluation.");
            }
            CATCH("Prescription");
         }
      }
   }
}


void Prescription::get_initial(Vect<real_t>& v,
                               size_t        k,
                               size_t        dof)
{
   int err;
   mesh_nodes(*_theMesh) {
      if (_p[k].dof==dof || dof==0) {
         _data[0] = the_node->getX();
         _data[1] = the_node->getY();
         _data[2] = the_node->getZ();
         v(node_label) = EVAL(_data);
         try {
            if ((err=EVAL_ERR))
               THROW_RT("get_initial(Vect<real_t>,size_t,size_t): Error in expression evaluation.");
         }
         CATCH("Prescription");
      }
   }
}


void Prescription::get_initial(Vect<real_t>& v,
                               size_t        k)
{
   size_t i;
   int err;
   mesh_nodes(*_theMesh) {
      _data[0] = the_node->getX();
      _data[1] = the_node->getY();
      _data[2] = the_node->getZ();
      for (i=1; i<=the_node->getNbDOF(); i++) {
         if (i==_p[k].dof) {
            v(the_node->getDOF(_p[k].dof)) = EVAL(_data);
            try {
               if ((err=EVAL_ERR))
                  THROW_RT("get_initial(Vect<real_t>,size_t): Error in expression evaluation.");
            }
            CATCH("Prescription");
         }
      }
   }
}


void Prescription::get_body_force(Vect<real_t>& v,
                                  size_t        k,
                                  size_t        dof)
{
   int err;
   mesh_nodes(*_theMesh) {
      if (_p[k].dof==dof || dof==0) {
         _data[0] = the_node->getX();
         _data[1] = the_node->getY();
         _data[2] = the_node->getZ();
         v(node_label) = EVAL(_data);
         try {
            if ((err=EVAL_ERR))
               THROW_RT("get_body_force(Vect<real_t>,size_t,size_t): Error in expression evaluation.");
         }
         CATCH("Prescription");
      }
   }
}


void Prescription::get_body_force(Vect<real_t>& v,
                                  size_t        k)
{
   size_t l=0;
   int err;
   mesh_nodes(*_theMesh) {
      _data[0] = the_node->getX();
      _data[1] = the_node->getY();
      _data[2] = the_node->getZ();
      for (size_t i=1; i<=the_node->getNbDOF(); i++) {
         l++;
         if (i==_p[k].dof) {
            v(l) = EVAL(_data);
            try {
               if ((err=EVAL_ERR))
                  THROW_RT("get_body_force(Vect<real_t>,size_t): Error in expression evaluation.");
            }
            CATCH("Prescription");
         }
      }
   }
}


} /* namespace OFELI */