# This test performs a load inversion optimization for a mechanics problem

# The forward problem is a cantilever beam with a fixed end at x = 0 and
# uniform loading applied using a NodalKernel at x = 5. The experimental data
# includes Y displacements at eight points through the beam.

# Lessons learnt from this problem:
# 1. Scale objective function when it is close to zero
# 2. Make sure app is reset after every forward solve
#    (MultiApps/forward/reset_app = true)
# 3. Provide bounds so that your forward solve doesn't go haywire
# 4. Make sure initial condition is inside the bounds (now hard coded)

[StochasticTools]
[]

[FormFunction]
  type = ObjectiveMinimize
  parameter_names = 'p1'
  num_values = '1'
  misfit_name = 'misfit'
  lower_bounds = '800'
  upper_bounds = '1500'
  scale_factor = 1e18 # scaling objective function to make sure it is not ~ 0
[]

[Executioner]
  type = Optimize
  tao_solver = taonm
  petsc_options_iname = '-tao_gftol'
  petsc_options_value = '1e-12'
  verbose = true
[]

[MultiApps]
  [forward]
    type = OptimizeFullSolveMultiApp
    input_files = forward.i
    execute_on = "FORWARD"
    reset_app = true
  []
[]

[Transfers]
  [toforward]
    type = OptimizationParameterTransfer
    multi_app = forward
    value_names = 'p1'
    parameters = 'Functions/right_function/value'
    to_control = parameterReceiver
  []
  [fromforward]
    type = MultiAppReporterTransfer
    multi_app = forward
    direction = from_multiapp
    from_reporters = 'data_pt/disp_y_difference data_pt/disp_y'
    to_reporters = 'FormFunction/misfit receiver/measured'
  []
[]

[Reporters]
  [receiver]
    type = ConstantReporter
    real_vector_names = measured
    real_vector_values = '0 0 0 0 0 0 0 0'
   []
[]

[Outputs]
  csv=true
[]
