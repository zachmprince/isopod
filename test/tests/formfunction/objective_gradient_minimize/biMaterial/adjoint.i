[Mesh]
  type = GeneratedMesh
  dim = 2
  nx = 20
  ny = 20
  xmax = 2
  ymax = 2
[]

[Variables]
  [temperature]
    order = FIRST
    family = LAGRANGE
  []
[]
[Kernels]
  [heat_conduction]
    type = HeatConduction
    variable = temperature
  []
[]

[DiracKernels]
  [pt]
    type = VectorPostprocessorPointSource
    variable = temperature
    vector_postprocessor = point_source
    x_coord_name = x
    y_coord_name = y
    z_coord_name = z
    value_name = value
  []
[]

[AuxVariables]
  [temperature_forward]
  []
  [T2]
  []
  [T3]
  []
[]
[AuxKernels]
  [T2_funcAux]
    type = FunctionAux
    variable = T2
    function = 'if (x> 1.0 & x <1.5, 1, 0)'
  []
  [T3_funcAux]
    type = ParsedAux
    args = 'T2 temperature_forward'
    function = '-T2*temperature_forward'  #Fixme lynn WHY is there a minus sign?  Should have made examples with blocks instead of trying to use function
    variable = T3
  []
[]


[BCs]
  [left]
    type = NeumannBC
    variable = temperature
    boundary = left
    value = 0
  []
  [right]
    type = NeumannBC
    variable = temperature
    boundary = right
    value = 0
  []
  [bottom]
    type = DirichletBC
    variable = temperature
    boundary = bottom
    value = 0
  []
  [top]
    type = DirichletBC
    variable = temperature
    boundary = top
    value = 0
  []
[]

[Functions]
  [thermo_conduct]
    type = ParsedFunction
    value = 'if (x> 1.0 & x <1.5, h1, h2)'
    vars = 'h1 h2'
    vals = 'p1 p2'
  []
[]

[Materials]
  [steel]
    type = GenericFunctionMaterial
    prop_names = 'thermal_conductivity'
    prop_values = 'thermo_conduct'
    outputs = exodus
  []
  [volumetric_heat]
    type = ADGenericFunctionMaterial
    prop_names = 'volumetric_heat'
    prop_values = '1000'
    outputs = exodus
  []
[]

[Problem]
  type = FEProblem
[]

[Executioner]
  type = Steady
  solve_type = PJFNK
  nl_abs_tol = 1e-8
  nl_rel_tol = 1e-8
  petsc_options_iname = '-ksp_type -pc_type -pc_factor_mat_solver_package'
  petsc_options_value = 'preonly lu       superlu_dist'
[]

[Postprocessors]
  [pp_adjoint_grad]
    type = DiffusionVariableIntegral
    variable = temperature
    variable_forward = T3 #temperature_forward #I think this should be T3
  []
  [p1]
    type = ConstantValuePostprocessor
    value = 1 # true sol = 10
    execute_on = 'linear'
  []
  [p2]
    type = ConstantValuePostprocessor
    value = 5 # true sol = 5
    execute_on = 'linear'
  []
[]

[VectorPostprocessors]
  [point_source]
    type = ParConstantVectorPostprocessor
    vector_names = 'x y z value'
    value = '0.5 0.5 0.5 0.5;
             0.2 0.6 1.4 1.8;
             0   0   0   0;
             10  10  10  10'
  []
  [adjoint_grad]
    type = VectorOfPostprocessors
    postprocessors = 'pp_adjoint_grad'
  []
[]

[Controls]
  [parameterReceiver]
    type = ControlsReceiver
  []
[]

[Outputs]
  console = true
  exodus = true
  file_base = 'adjoint'
[]
