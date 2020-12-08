//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "ElementIntegralPostprocessor.h"

class DiffusionVariableIntegral;

template <>
InputParameters validParams<DiffusionVariableIntegral>();

class DiffusionVariableIntegral : public ElementIntegralPostprocessor
                                  // public MooseVariableInterface<Real>
{
public:
  static InputParameters validParams();

  DiffusionVariableIntegral(const InputParameters & parameters);

protected:
  virtual Real computeQpIntegral() override;

  /// Name of variable
  const VariableName _var_name;

  /// Regular MooseVariable
  MooseVariable & _var;

  /// Holds the solution at current quadrature points
  const VariableValue & _u;

  /// Holds the solution gradient at the current quadrature points
  const VariableGradient & _grad_u;

  /// gradient of the shape function
  const VariablePhiGradient & _grad_phi;

  /// gradient of the test function
  const VariableTestGradient & _grad_test;

  const Real & _scale_factor;
};
