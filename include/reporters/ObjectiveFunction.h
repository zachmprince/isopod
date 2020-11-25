//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "GeneralReporter.h"
#include "libmesh/petsc_vector.h"
#include "libmesh/petsc_matrix.h"

/**
 * Report the time and iteration information for the simulation.
 */
class ObjectiveFunction : public GeneralReporter
{
public:
  static InputParameters validParams();
  ObjectiveFunction(const InputParameters & parameters);
  virtual void initialize() override {}
  virtual void finalize() override {}
  virtual void execute() override {}

  /**
   * Function to initialize petsc vectors from vpp data
   */
  void initializePetscVectors();

  /**
   * Function to set parameters.
   * This is the first function called in objective/gradient/hessian routine
   */
  void setParameters(const libMesh::PetscVector<Number> & x);

  /**
   * Function to compute objective.
   * This is the last function called in objective routine
   */
  virtual Real computeObjective() = 0;

  /**
   * Function to compute gradient.
   * This is the last call of the gradient routine.
   */
  virtual void computeGradient()
  {
    mooseError("Gradient function has not been defined for form function type ", _type);
  }

  /**
   * Function to compute gradient.
   * This is the last call of the hessian routine.
   */
  virtual void computeHessian()
  {
    mooseError("Hessian function has not been defined for form function type ", _type);
  }

  /**
   * Function to retrieve current parameters
   */
  dof_id_type getNumberOfParameters() const { return _nParams; }

  /**
   * Function to retrieve current parameters
   */
  libMesh::PetscVector<Number> & getParameters() { return _parameters; }
  const libMesh::PetscVector<Number> & getParameters() const { return _parameters; }

  /**
   * Function to retrieve last computed gradient
   */
  libMesh::PetscVector<Number> & getGradient() { return _gradient; }
  const libMesh::PetscVector<Number> & getGradient() const { return _gradient; }

  /**
   * Function to retrieve last computed gradient
   */
  libMesh::PetscMatrix<Number> & getHessian() { return _hessian; }
  const libMesh::PetscMatrix<Number> & getHessian() const { return _hessian; }

  /**
   * Get communicator used for matrices
   */
  const libMesh::Parallel::Communicator & getComm() const { return _my_comm; }

protected:
  /// Communicator used for operations
  const libMesh::Parallel::Communicator _my_comm;

  /// Parameters
  libMesh::PetscVector<Number> _parameters;

  /// Gradient
  libMesh::PetscVector<Number> _gradient;

  /// Hessian
  libMesh::PetscMatrix<Number> _hessian;

  /// parameter names reporter data
  std::vector<std::string> & _parameter_names;

  /// parameter values reporter has
  std::vector<Real> & _parameter_values;

  /// Number of parameters
  dof_id_type _nParams;
};
