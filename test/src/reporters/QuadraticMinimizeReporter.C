#include "QuadraticMinimizeReporter.h"

registerMooseObject("isopodApp", QuadraticMinimizeReporter);

InputParameters
QuadraticMinimizeReporter::validParams()
{
  InputParameters params = ObjectiveFunction::validParams();
  params.addRequiredParam<Real>("objective", "Desired value of objective function.");
  params.addRequiredParam<VectorPostprocessorName>("measured_vpp",
                                                   "VectorPostprocessor of measured data.");
  return params;
}

QuadraticMinimizeReporter::QuadraticMinimizeReporter(const InputParameters & parameters)
  : ObjectiveFunction(parameters),
    _result(getParam<Real>("objective")),
    _measured_values(getVectorPostprocessorValue("measured_vpp", "values", false))
{
}

Real
QuadraticMinimizeReporter::computeObjective()
{
  Real val = _result;
  for (dof_id_type i = 0; i < _nParams; ++i)
  {
    Real tmp = _parameters(i) - _measured_values[i];
    val += tmp * tmp;
  }

  return val;
}

void
QuadraticMinimizeReporter::computeGradient()
{
  for (dof_id_type i = 0; i < _nParams; ++i)
    _gradient.set(i, 2.0 * (_parameters(i) - _measured_values[i]));
  _gradient.close();
}

void
QuadraticMinimizeReporter::computeHessian()
{
  _hessian.zero();
  for (dof_id_type i = 0; i < _nParams; ++i)
    _hessian.set(i, i, 2.0);
  _hessian.close();
}
