[Mesh]
  type = GeneratedMesh
  dim = 2
  nx = 10
  ny = 10
  xmax = 1
  ymax = 1.4
[]

[Variables]
  [temperature]
    order = FIRST
    family = LAGRANGE
  []
[]

[AuxVariables]
  [saved_t]
    order = FIRST
    family = LAGRANGE
  []
[]

[Kernels]
  [heat_conduction]
    type = HeatConduction
    variable = temperature
    save_in = saved_t
  []
[]

[DiracKernels]
  [pt]
    type = VectorPostprocessorPointSource
    variable = temperature
    vector_postprocessor = point_source
    x_coord_name = 'x'
    y_coord_name = 'y'
    z_coord_name = 'z'
    value_name = 'value'
  []
[]

[BCs]
  [left]
    type = DirichletBC
    variable = temperature
    boundary = left
    value = 300
  []
  [right]
    type = DirichletBC
    variable = temperature
    boundary = right
    value = 300
  []
  [bottom]
    type = DirichletBC
    variable = temperature
    boundary = bottom
    value = 300
  []
  [top]
    type = DirichletBC
    variable = temperature
    boundary = top
    value = 300
  []
[]

[Materials]
  [steel]
    type = GenericConstantMaterial
    prop_names = thermal_conductivity
    prop_values = 5
  []
[]

[Executioner]
  type = Steady
  solve_type = PJFNK
  nl_abs_tol = 1e-6
  nl_rel_tol = 1e-8
  petsc_options_iname = '-pc_type -pc_hypre_type'
  petsc_options_value = 'hypre boomeramg'
[]

[VectorPostprocessors]
  [point_source]
    type = ConstantVectorPostprocessor
    vector_names = 'x y z value'
    value = '0.35 0.4 0.7;
             0.20 1.0 0.56;
             0 0 0;
             6392 5570 4033'
  []
  [data_pt]
    type = MeasuredDataPointSampler
    variable = temperature
    points = '0.5 0.28 0
              0.55 0.56 0
              0.55 0.82 0
              0.5 1.0 0'
    measured_values = '280 315 321 327'
  []
[]

[Outputs]
  console = true
  exodus = true
  file_base = 'forward'
[]
