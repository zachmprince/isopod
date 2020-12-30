//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "ElementIntegralVariablePostprocessor.h"

class DiffusionVariableIntegral;

template <>
InputParameters validParams<DiffusionVariableIntegral>();

class DiffusionVariableIntegral : public ElementIntegralVariablePostprocessor
{
public:
  static InputParameters validParams();

  DiffusionVariableIntegral(const InputParameters & parameters);

  virtual Real getValue() override;

protected:
  virtual Real computeQpIntegral() override;
  const VariableValue & _variable_grad_x;
  const VariableValue & _variable_grad_y;
  const VariableValue & _variable_grad_z;
  //  /// Holds the forward solution gradient at the current quadrature points
  //  const VariableGradient & _grad_other_var;
  //
  //  /// gradient of the shape function
  const VariablePhiGradient & _grad_phi;
  //
  /// gradient of the test function
  //  const VariableTestGradient & _grad_test;
};
