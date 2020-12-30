//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "DiffusionVariableIntegral.h"

#include "Assembly.h"
#include "MooseVariableFE.h"
#include "MooseVariableScalar.h"
#include "SubProblem.h"
#include "NonlinearSystem.h"

#include "libmesh/threads.h"
#include "libmesh/quadrature.h"

registerMooseObject("isopodApp", DiffusionVariableIntegral);

defineLegacyParams(DiffusionVariableIntegral);

InputParameters
DiffusionVariableIntegral::validParams()
{
  InputParameters params = ElementIntegralVariablePostprocessor::validParams();
  params.addRequiredCoupledVar("variable_grad_x", "fixme lynn grad x");
  params.addRequiredCoupledVar("variable_grad_y", "fixme lynn grad y");
  params.addRequiredCoupledVar("variable_grad_z", "fixme lynn grad z");
  return params;
}

DiffusionVariableIntegral::DiffusionVariableIntegral(const InputParameters & parameters)
  : ElementIntegralVariablePostprocessor(parameters),
    _variable_grad_x(coupledValue("variable_grad_x")),
    _variable_grad_y(coupledValue("variable_grad_y")),
    _variable_grad_z(coupledValue("variable_grad_z")),

    _grad_phi(_assembly.gradPhi())
//        _gradient(coupledGradient("gradient_variable")),
//    _forward_sol_name(getParam<VariableName>("forward_sol")),
//    _forward_sol(_subproblem.getStandardVariable(_tid, _forward_sol_name)),
//    _grad_phi(_assembly.gradPhi(_forward_sol)),
//    _grad_other_var(_other_var.gradSln()),
//    _grad_phi(_other_var.gradSln()),
//    _grad_test(_other_var.gradSln())
{
}

Real
DiffusionVariableIntegral::getValue()
{
  return ElementIntegralPostprocessor::getValue();
}

Real
DiffusionVariableIntegral::computeQpIntegral()
{

  //  std::cout << "_variable_grad: " << _variable_grad_x[_qp] << "  " << _variable_grad_y[_qp] << "
  //  "
  //            << _variable_grad_z[_qp] << std::endl;
  //  std::cout << _adjoint_sol.size() << std::endl;
  //  // std::cout<<_grad_u[0]<<std::endl;
  //  // std::cout<<_grad_test[0][0]<<std::endl;
  //  // std::cout<<_grad_test[0][0]*_grad_u[0]<<std::endl;
  //  std::cout << std::endl;
  Real sum = 0;
  //  for (unsigned int i = 0; i < _grad_phi.size(); ++i)
  //  {
  //    sum += _grad_phi[i][_qp][0] * _variable_grad_x[_qp];
  //    sum += _grad_phi[i][_qp][1] * _variable_grad_y[_qp];
  //    sum += _grad_phi[i][_qp][2] * _variable_grad_z[_qp];
  //  }
  // definately wrong.
  RealVectorValue var_grad(_variable_grad_x[_qp], _variable_grad_y[_qp], _variable_grad_z[_qp]);
  for (unsigned int i = 0; i < _grad_phi.size(); ++i)
  {
    std::cout << "gradPHi: " << _grad_phi[i][_qp] << std::endl;
    std::cout << "var_grad:  " << var_grad << std::endl;
    sum += _grad_phi[i][_qp] * var_grad;
  }
  std::cout << "sum:  " << sum << std::endl;
  return -_u[_qp] * sum;
}
