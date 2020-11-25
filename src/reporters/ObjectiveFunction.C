//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "ObjectiveFunction.h"

InputParameters
ObjectiveFunction::validParams()
{
  InputParameters params = GeneralReporter::validParams();
  params.addClassDescription("Objective function for inverse optimization.");
  params.addRequiredParam<std::vector<std::string>>(
      "parameter_names", "Parameter names being controlled in forward problem");
  params.addParam<std::vector<Real>>("parameter_values", "Initial values for parameters");
  return params;
}

ObjectiveFunction::ObjectiveFunction(const InputParameters & parameters)
  : GeneralReporter(parameters),
    _my_comm(MPI_COMM_SELF),
    _parameters(_my_comm),
    _gradient(_my_comm),
    _hessian(_my_comm),
    _parameter_names(declareValueByName<std::vector<std::string>>(
        "parameter_names", getParam<std::vector<std::string>>("parameter_names"))),
    _parameter_values(declareValueByName<std::vector<Real>>(
        "parameter_values",
        isParamValid("parameter_values") ? getParam<std::vector<Real>>("parameter_values")
                                         : std::vector<Real>(_parameter_names.size(), 0.0))),
    _nParams(_parameter_names.size())
{
  _parameters.init(_nParams);
  _parameters = _parameter_values;

  _gradient.init(_nParams);
  _hessian.init(/*global_rows =*/_nParams,
                /*global_cols =*/_nParams,
                /*local_rows =*/_nParams,
                /*local_cols =*/_nParams,
                /*block_diag_nz =*/_nParams,
                /*block_off_diag_nz =*/0);
}

void
ObjectiveFunction::initializePetscVectors()
{
}

void
ObjectiveFunction::setParameters(const libMesh::PetscVector<Number> & x)
{
  _parameters = x;

  std::vector<Real> transfer;

  _parameters.localize(transfer);

  for (auto & value : transfer)
    std::cout << value << std::endl;

  mooseAssert(transfer.size() == _parameter_values.size(),
              "Receiving more values than declared in parameter_values of the ObjectiveFunction");
  _parameter_values = transfer;
}
