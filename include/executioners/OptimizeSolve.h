#pragma once

#include "SolveObject.h"

#include "FormFunction.h"
#include "ExecFlagEnum.h"
#include <petsctao.h>
#include "libmesh/petsc_vector.h"
#include "libmesh/petsc_matrix.h"

class FormFunction;

class OptimizeSolve : public SolveObject
{
public:
  static InputParameters validParams();
  OptimizeSolve(Executioner & ex);

  virtual bool solve() override;

  const FormFunction & getFormFunction() const { return *_form_function; }

  // fixme lynn I wish this were all a struct
  void getTaoSolutionStatus(std::vector<int> & tot_iters,
                            std::vector<double> & gnorm,
                            std::vector<int> & obj_iters,
                            std::vector<double> & cnorm,
                            std::vector<int> & grad_iters,
                            std::vector<double> & xdiff,
                            std::vector<int> & hess_iters,
                            std::vector<double> & f) const;

protected:
  /// Bounds routine
  virtual PetscErrorCode variableBounds(Tao tao);

  /// Objective routine
  virtual Real objectiveFunction();

  /// Gradient routine
  virtual void gradientFunction(libMesh::PetscVector<Number> & gradient);

  /// Hessian routine
  virtual void hessianFunction(libMesh::PetscMatrix<Number> & hessian);

  /// Communicator used for operations
  const libMesh::Parallel::Communicator _my_comm;

  /// List of execute flags for when to solve the system
  const ExecFlagEnum & _solve_on;

  /// Form function defining objective, gradient, and hessian
  FormFunction * _form_function = nullptr;

  /// Tao optimization object
  Tao _tao;

private:
  bool _verbose;
  /// tao solver info
  int _obj_iterate = 0;
  int _grad_iterate = 0;
  int _hess_iterate = 0;
  std::vector<int> _total_iterate_vec;
  std::vector<int> _obj_iterate_vec;
  std::vector<int> _grad_iterate_vec;
  std::vector<int> _hess_iterate_vec;
  std::vector<double> _f_vec;
  std::vector<double> _gnorm_vec;
  std::vector<double> _cnorm_vec;
  std::vector<double> _xdiff_vec;

  /// Here is where we call tao and solve
  PetscErrorCode taoSolve();

  /// output optimization iteration solve data
  static PetscErrorCode monitor(Tao tao, void * ctx);

  void setTaoSolutionStatus(double f, int its, double gnorm, double cnorm, double xdiff);

  ///@{
  /// Function wrappers for tao
  static PetscErrorCode objectiveFunctionWrapper(Tao tao, Vec x, Real * objective, void * ctx);
  static PetscErrorCode hessianFunctionWrapper(Tao tao, Vec x, Mat hessian, Mat pc, void * ctx);
  static PetscErrorCode
  objectiveAndGradientFunctionWrapper(Tao tao, Vec x, Real * objective, Vec gradient, void * ctx);
  static PetscErrorCode variableBoundsWrapper(Tao /*tao*/, Vec xl, Vec xu, void * ctx);
  ///@}

  // fixme lynn these have weird names because Petsc already has macros for TAONTR etc.
  /// Enum of tao solver types
  const enum class TaoSolverEnum {
    NEWTON_TRUST_REGION,
    BUNDLE_RISK_MIN,
    ORTHANT_QUASI_NEWTON,
    QUASI_NEWTON,
    CONJUGATE_GRADIENT,
    NELDER_MEAD,
    BOUNDED_QUASI_NEWTON,
    BOUNDED_CONJUGATE_GRADIENT,
    BOUNDED_QUASI_NEWTON_LINE_SEARCH,
    BOUNDED_NEWTON_LINE_SEARCH,
    BOUNDED_NEWTON_TRUST_REGION,
    GRADIENT_PROJECTION_CONJUGATE_GRADIENT,
  } _tao_solver_enum;

  /// Number of parameters being optimized
  dof_id_type _ndof;

  /// Parameters (solution)
  std::unique_ptr<libMesh::PetscVector<Number>> _parameters;

  /// Hessian (matrix)
  libMesh::PetscMatrix<Number> _hessian;
};
