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
  InputParameters params = ElementIntegralPostprocessor::validParams();

  params.addCoupledVar("adjoint_sol", "Solution from the forward problem");
  params.addRequiredParam<VariableName>(
      "forward_sol", "Solution from the adjoint problem");
  params.addClassDescription("Integrates a function times variable over elements");
  return params;
}

// _grad_test(_var.gradPhi()),
//     _phi(_assembly.phi(_var)),
//     _grad_phi(_assembly.gradPhi(_var)),
//     _u(_is_implicit ? _var.sln() : _var.slnOld()),
//     _grad_u(_is_implicit ? _var.gradSln() : _var.gradSlnOld())

DiffusionVariableIntegral::DiffusionVariableIntegral(const InputParameters & parameters)
  : ElementIntegralPostprocessor(parameters),
    // MooseVariableInterface<Real>(this,
    //                              false,
    //                              "variable",
    //                              Moose::VarKindType::VAR_ANY,
    //                              Moose::VarFieldType::VAR_FIELD_STANDARD),
    _adjoint_sol(coupledValue("adjoint_sol")),
    _forward_sol_name(getParam<VariableName>("forward_sol")),
    _forward_sol(_subproblem.getStandardVariable(_tid, _forward_sol_name)),
    _u(_forward_sol.sln()),
    _grad_u(_forward_sol.gradSln()),
    _grad_phi(_assembly.gradPhi(_forward_sol)),
    _grad_test(_forward_sol.gradPhi())
{
  // addMooseVariableDependency(&mooseVariableField());
}

Real
DiffusionVariableIntegral::computeQpIntegral()
{
  mooseAssert(_grad_phi.size() == _grad_u.size(), "sizes do not match");

  std::cout<<_adjoint_sol.size()<<std::endl;
  // std::cout<<_grad_u[0]<<std::endl;
  // std::cout<<_grad_test[0][0]<<std::endl;
  // std::cout<<_grad_test[0][0]*_grad_u[0]<<std::endl;
  std::cout<<std::endl;
  Real sum = 0;
  for (unsigned int i = 0; i < _grad_phi.size(); ++i)
    sum += _grad_test[i][_qp] * _grad_u[i];
  return _adjoint_sol[_qp] * sum;
}
