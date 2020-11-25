[StochasticTools]
[]

[FormFunction]
  type = QuadraticMinimize
  parameter_vpp = 'results'
  measured_vpp = 'measurements'
  objective = 1.0
[]
[Reporters]
  [objective_function]
    type = QuadraticMinimizeReporter
    parameter_names = 'param_0 param_1 param_2'
    parameter_values = '5 8 1'
    measured_vpp = 'measurements'
    objective = 1.0
  []
[]

[VectorPostprocessors]
  [results]
    type = OptimizationParameterVectorPostprocessor
    parameters = 'param_0 param_1 param_2'
    intial_values = '5 8 1'
  []
  [measurements]
    type = ConstantVectorPostprocessor
    value = '1 2 3'
    outputs = none
    vector_names = 'values'
  []
[]

[Executioner]
  type = Optimize
  tao_solver = TAOCG
  petsc_options_iname='-tao_gatol -tao_cg_delta_max'
  petsc_options_value='1e-4 1e-2'
  solve_on = none
  verbose = true
[]

[Outputs]
  csv = true
[]
