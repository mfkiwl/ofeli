/*==============================================================================

                                    O  F  E  L  I

                           Object  Finite  Element  Library

  ==============================================================================

   Copyright (C) 1998 - 2023 Rachid Touzani

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

                            Implementation of class 'Fct'

  ==============================================================================*/

#include "io/Fct.h"
#include <ostream>
#include <iostream>
#include "io/exprtk.hpp"

namespace OFELI {

Fct::Fct()
    : name("f"), nb_var(0), _p(nullptr), _st(nullptr), _ex(nullptr), _xvar(nullptr),
      exp_ok(false), var_ok(false), err(1)
{
   error_message = "No error in function evaluation.";
}


Fct::Fct(const string& exp)
    : name("f"), nb_var(0), _p(nullptr), _st(nullptr), _ex(nullptr), _xvar(nullptr),
      exp_ok(false), var_ok(false), err(1)
{
   error_message = "No error in function evaluation.";
   set(exp);
}


Fct::Fct(const string&         exp,
         const vector<string>& v)
    : name("f"), nb_var(0), _p(nullptr), _st(nullptr), _ex(nullptr), _xvar(nullptr),
      exp_ok(false), var_ok(false), err(1)
{
   error_message = "No error in function evaluation.";
   set(exp,v);
}


Fct::Fct(const string& exp,
         const string& v)
    : name("f"), nb_var(0), _p(nullptr), _st(nullptr), _ex(nullptr), _xvar(nullptr),
      exp_ok(false), var_ok(false), err(1)
{
   error_message = "No error in function evaluation.";
   set(exp,v);
}


Fct::Fct(const string&         n,
         const string&         exp,
         const vector<string>& v)
    : name(n), nb_var(0), _p(nullptr), _st(nullptr), _ex(nullptr), _xvar(nullptr),
      exp_ok(false), var_ok(false), err(1)
{
   error_message = "No error in function evaluation.";
   set(exp,v);
}


Fct::~Fct()
{
   if (_p!=nullptr)
      delete _p;
   if (_ex!=nullptr)
      delete _ex;
   if (_st!=nullptr)
      delete _st;
   if (_xvar!=nullptr)
      delete _xvar;
}


void Fct::add_constants()
{
   _st->add_constant("pi",OFELI_PI);
   _st->add_constant("e",OFELI_E);
   _st->add_constants();
}


string Fct::getErrorMessage()
{
   return error_message;
}


int Fct::set(const string&         n,
             const string&         exp,
             const vector<string>& v,
             int                   opt)
{
   name = n;
   return set(exp,v,opt);
}


int Fct::getErrorCode() const
{
   return 1-err;
}


int Fct::set(const string& exp,
             const string& v,
             int           opt)
{
   if (_p!=nullptr)
      delete _p;
   _p = new exprtk::parser<real_t>;
   if (_st!=nullptr)
      delete _st;
   _st = new exprtk::symbol_table<real_t>;
   if (_ex!=nullptr)
      delete _ex;
   _ex = new exprtk::expression<real_t>;
   exp_ok = var_ok = true;
   nb_var = 1;
   expr = exp;
   var.push_back(v);
   if (_xvar!=nullptr)
      delete _xvar;
   _xvar = new vector<real_t>(1);
   add_constants();
   _st->add_variable(var[0],(*_xvar)[0]);
   _ex->register_symbol_table(*_st);
   err = _p->compile(exp,*_ex);
   if (err==0) {
     error_message = _p->error();
      if (opt==0)
         std::cout << "Error: " << error_message << ", Expression: " << expr << std::endl;
   }
   return 1-err;
}


int Fct::set(const string& exp,
             int           opt)
{
   if (_p!=nullptr)
      delete _p;
   _p = new exprtk::parser<real_t>;
   if (_st!=nullptr)
      delete _st;
   _st = new exprtk::symbol_table<real_t>;
   if (_ex!=nullptr)
      delete _ex;
   _ex = new exprtk::expression<real_t>;
   exp_ok = var_ok = true;
   nb_var = 4;
   expr = exp;
   var.push_back("x");
   var.push_back("y");
   var.push_back("z");
   var.push_back("t");
   if (_xvar!=nullptr)
      delete _xvar;
   _xvar = new vector<real_t>(4);
   for (size_t i=0; i<4; ++i) {
      _st->add_variable(var[i],(*_xvar)[i]);
   }
   add_constants();
   _ex->register_symbol_table(*_st);
   err = _p->compile(exp,*_ex);
   if (err==0) {
      error_message = _p->error();
      if (opt==0)
         std::cout << "Error: " << error_message << ", Expression: " << expr << std::endl;
   }
   return 1-err;
}


int Fct::set(const string&         exp,
             const vector<string>& v,
             int                   opt)
{
   if (_p!=nullptr)
      delete _p;
   _p = new exprtk::parser<real_t>;
   if (_st!=nullptr)
      delete _st;
   _st = new exprtk::symbol_table<real_t>;
   if (_ex!=nullptr)
      delete _ex;
   _ex = new exprtk::expression<real_t>;
   exp_ok = var_ok = true;
   nb_var = v.size();
   expr = exp;
   if (_xvar!=nullptr)
      delete _xvar;
   for (auto it=std::begin(v); it!=std::end(v); ++it)
      var.push_back(*it);
   _xvar = new vector<real_t>(nb_var);
   add_constants();
   for (size_t i=0; i<nb_var; ++i)
      _st->add_variable(var[i],(*_xvar)[i]);
   _ex->register_symbol_table(*_st);
   err = _p->compile(exp,*_ex);
   if (err==0) {
      error_message = _p->error();
      if (opt==0)
         std::cout << "Error: " << error_message << ", Expression: " << expr << std::endl;
   }
   return 1-err;
}


real_t Fct::D(real_t x)
{
   (*_xvar)[0] = x;
   return exprtk::derivative(*_ex,(*_xvar)[0]);
}


real_t Fct::D(const vector<real_t>& x,
              size_t                i)
{
   *_xvar = x;
   if (i<=nb_var)
      return exprtk::derivative(*_ex,(*_xvar)[i-1]);
   else
      return 0.;
}


int Fct::check()
{
   vector<string> v;
   if (exprtk::collect_variables(expr,v))
      return 0;
   return 1;
}


real_t Fct::operator()(real_t x)
{
   (*_xvar)[0] = x;
   return _ex->value();
}


real_t Fct::operator()(real_t x,
                       real_t y)
{
   (*_xvar)[0] = x, (*_xvar)[1] = y;
   return _ex->value();
}


real_t Fct::operator()(real_t x,
                       real_t y,
                       real_t z)
{
   (*_xvar)[0] = x, (*_xvar)[1] = y, (*_xvar)[2] = z;
   return _ex->value();
}


real_t Fct::operator()(real_t x,
                       real_t y,
                       real_t z,
                       real_t t)
{
   (*_xvar)[0] = x, (*_xvar)[1] = y, (*_xvar)[2] = z, (*_xvar)[3] = t;
   return _ex->value();
}


real_t Fct::operator()(const Point<real_t>& x)
{
   (*_xvar)[0] = x.x, (*_xvar)[1] = x.y, (*_xvar)[2] = x.z;
   return _ex->value();
}


real_t Fct::operator()(const Point<real_t>& x,
                       real_t               t)
{
   (*_xvar)[0] = x.x, (*_xvar)[1] = x.y, (*_xvar)[2] = x.z, (*_xvar)[3] = t;
   return _ex->value();
}


real_t Fct::operator()(const vector<real_t>& x)
{
   *_xvar = x;
   return _ex->value();
}

  
std::ostream& operator<<(std::ostream& s,
                         const Fct&    f)
{
   s << "Name of function: " << f.name << std::endl;
   s << "Definition: " << f.expr << std::endl;
   s << "List of variables: ";
   for (size_t i=0; i<f.nb_var-1; ++i)
      s << f.var[i] << ", ";
   s << f.var[f.nb_var-1] << std::endl;
   return s;
}

} /* namespace OFELI */
