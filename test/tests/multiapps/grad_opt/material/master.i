[StochasticTools]
[]

[Mesh]
  type = GeneratedMesh
  dim = 2
  nx = 20
  ny = 20
  xmax = 2
  ymax = 2
[]

[FormFunction]
  type = ObjectiveGradientMinimize
  parameter_names = 'p1'
  num_values = '1'
  initial_condition = '10'
  misfit_name = 'misfit'
  adjoint_data_name = 'adjoint'
[]

[Executioner]
  type = Optimize
  tao_solver = taolmvm
  # petsc_options_iname = '-tao_gatol'
  # petsc_options_value = '1e-4'
  # petsc_options_iname = '-tao_fd_gradient -tao_fd_delta -tao_gatol'
  # petsc_options_value = 'true 0.0001 1e-4'
   petsc_options_iname='-tao_max_it -tao_fd_test -tao_test_gradient -tao_fd_gradient -tao_fd_delta -tao_gatol'
   petsc_options_value='3 true true false 0.0001 0.0001'
  verbose = true
[]

[MultiApps]
  [forward]
    type = OptimizeFullSolveMultiApp
    input_files = forward.i
    execute_on = "FORWARD"
    clone_master_mesh = true
    ignore_solve_not_converge = true #false
  []
  [adjoint]
    type = OptimizeFullSolveMultiApp
    input_files = adjoint.i
    execute_on = "ADJOINT"
    clone_master_mesh = true
    ignore_solve_not_converge = false
  []
[]

[AuxVariables]
  [temperature_forward]
  []
  [grad_Tx]
    order = CONSTANT
    family = MONOMIAL
  []
  [grad_Ty]
    order = CONSTANT
    family = MONOMIAL
  []
  [grad_Tz]
    order = CONSTANT
    family = MONOMIAL
  []
[]

[Transfers]
  [toforward]
    type = OptimizationParameterTransfer
    multi_app = forward
    value_names = 'p1'
    parameters = 'Postprocessors/p1/value'
    to_control = parameterReceiver
  []
  [fromforward]
    type = MultiAppReporterTransfer
    multi_app = forward
    direction = from_multiapp
    from_reporters = 'data_pt/temperature_difference data_pt/temperature'
    to_reporters = 'FormFunction/misfit receiver/measured'
  []
  [fromforwardMesh]
    type = MultiAppCopyTransfer
    multi_app = forward
    direction = from_multiapp
    source_variable = 'temperature'
    variable = 'temperature_forward'
  []
  [fromforwardMeshgradx]#we dont need these, just for checkign adjoint
    type = MultiAppCopyTransfer
    multi_app = forward
    direction = from_multiapp
    source_variable = 'grad_Tx'
    variable = 'grad_Tx'
  []
  [fromforwardMeshgrady]
    type = MultiAppCopyTransfer
    multi_app = forward
    direction = from_multiapp
    source_variable = 'grad_Ty'
    variable = 'grad_Ty'
  []
  [fromforwardMeshgradz]
    type = MultiAppCopyTransfer
    multi_app = forward
    direction = from_multiapp
    source_variable = 'grad_Tz'
    variable = 'grad_Tz'
  []

  [toadjoint_misfit]
    type = MultiAppReporterTransfer
    multi_app = adjoint
    direction = to_multiapp
    from_reporters = 'FormFunction/misfit'
    to_reporters = 'point_source/value'
  []
  [toAdjointMesh]
    type = MultiAppCopyTransfer
    multi_app = adjoint
    direction = to_multiapp
    source_variable = 'temperature_forward'
    variable = 'temperature_forward'
  []
  [toAdjointMeshgradx]
    type = MultiAppCopyTransfer
    multi_app = adjoint
    direction = to_multiapp
    source_variable = 'grad_Tx'
    variable = 'grad_Tx'
  []
  [toAdjointMeshgrady]
    type = MultiAppCopyTransfer
    multi_app = adjoint
    direction = to_multiapp
    source_variable = 'grad_Ty'
    variable = 'grad_Ty'
  []
  [toAdjointMeshgradz]
    type = MultiAppCopyTransfer
    multi_app = adjoint
    direction = to_multiapp
    source_variable = 'grad_Tz'
    variable = 'grad_Tz'
  []
  [fromadjoint]
    type = MultiAppReporterTransfer
    multi_app = adjoint
    direction = from_multiapp
    from_reporters = 'adjoint_grad/adjoint_grad' # what is the naming convention for this
    to_reporters = 'FormFunction/adjoint'
  []
[]

[Reporters]
  [receiver]
    type = ConstantReporter
    real_vector_names = measured
    real_vector_values = '0 0 0 0'
   []
[]

[Outputs]
  file_base = 'master'
  console = true
  csv=true
  exodus = true
  execute_on = NONLINEAR
[]
