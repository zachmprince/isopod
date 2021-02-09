[Mesh]
  [mesh_gen]
    type = GeneratedMeshGenerator
    dim = 2
    nx = 10
    ny = 2
    xmin = 0.0
    xmax = 5.0
    ymin = 0.0
    ymax = 1.0
  []
[]

[Variables]
  [disp_x]
  []
  [disp_y]
  []
[]

[Modules/TensorMechanics/Master]
  displacements = 'disp_x disp_y'
  [strain_calculator]
    strain = FINITE
    displacements = 'disp_x disp_y'
  []
[]

[BCs]
  [fixx0]
    type = DirichletBC
    variable = disp_x
    boundary = left
    value = 0.0
  []
  [fixy1]
    type = DirichletBC
    variable = disp_y
    boundary = left
    value = 0.0
  []
[]

[NodalKernels]
  [force_y2]
    type = UserForcingFunctionNodalKernel
    variable = disp_y
    boundary = 'right'
    function = right_function
  []
[]

[Functions]
  [right_function]
    type = ConstantFunction
    value = 1000
  []
[]

[Materials]
  [./elasticity]
    type = ComputeIsotropicElasticityTensor
    youngs_modulus = 10e9
    poissons_ratio = 0.3
  []
  [stress]
    type = ComputeFiniteStrainElasticStress
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
  [data_pt]
    type = MeasuredDataPointSampler
    variable = disp_y
    points = '1.0  0.25 0
              2.0  0.25 0
              3.0  0.25 0
              4.0  0.25 0
              1.0  0.75 0
              2.0  0.75 0
              3.0  0.75 0
              4.0  0.75 0'
    measured_values = '7.14e-06 2.57e-05 5.28e-05 8.57e-05 7.14e-06 2.57e-05 5.28e-05 8.57e-05'
  []
[]

[Postprocessors]
  [p1]
    type = FunctionValuePostprocessor
    function = right_function
  []
[]

[Controls]
  [parameterReceiver]
    type = ControlsReceiver
  []
[]

[Outputs]
  exodus = false
  csv = true
  file_base = 'forward'
[]
