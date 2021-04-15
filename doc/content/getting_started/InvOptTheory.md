# Inverse Optimization Theory

Inverse optimization is a mathematical framework to infer model parameters by minimizing the misfit between the experimental and simulation observables.  In this work, our model is a partial differential equation (PDE) describing the physics of the experiment.  We solve our physics PDE using the finite element method as implemented in MOOSE.  The physics of our problem constrains our optimization algorithm.  A PDE-constrained inverse optimization framework is formulated as an abstract optimization problem (ref2):

\begin{equation}\label{eq:optimization}
   \min_{\bm{p}} J\left(\bm{u},\bm{p}\right);\quad~\textrm{subject to}~\bm{g}\left(\bm{u},\bm{p}\right)=\bm{0}
\end{equation}

where $J(\bm{u},\bm{p})$ is our objective function, which is a scalar measure of the misfit between experimental and simulated responses, along with any regularization ref12.  The constraint, $\bm{g}\left(\bm{u},\bm{p}\right)=\bm{0}$, consists of the PDEs governing the multiphysics phenomena simulated by MOOSE (e.g. coupled heat and elasticity equations), $\bm{p}$ contains model parameters (e.g. material properties or loads) and $\bm{u}$ contains simulated responses (e.g. temperature and displacement fields).  The equations in [eq:optimization] appear simple on the outset but are extremely difficult to solve. The solution space can span millions of degrees of freedom and the parameter space can also be very large. Finally, the PDEs can be highly nonlinear, time-dependent and tightly coupling complex phenomena across multiple physics.

Optimization problems can be solved using either global (gradient-free) or local (gradient-based) approaches (ref1). Global approaches require a large number of iterations compared to gradient-based approaches (e.g. conjugate gradient or Newton-type methods), making the latter more suitable to problems with a large parameter space and computationally expensive models.  Isopod uses the `PETSc TAO` optimization library to solve [eq:optimization]. See [PETSc TAO](https://www.mcs.anl.gov/petsc/documentation/taosolvertable.html) for a list of `TAO` gradient and gradient-free optimization solvers available in isopod.  Optimization libraries, like `TAO`, require access to functions for computing the objective ($J$), gradient $\left(\mathrm{d}J/\mathrm{d}\bm{u}\right)$ and Hessian

\begin{equation}\label{eq:hessian}
\mathbf H_f= \begin{bmatrix}
\dfrac{\partial^2 J}{\partial p_1^2} & \dfrac{\partial^2 J}{\partial p_1\,\partial p_2} & \cdots & \dfrac{\partial^2 J}{\partial p_1\,\partial p_n} \\[2.2ex]
\dfrac{\partial^2 J}{\partial p_2\,\partial p_1} & \dfrac{\partial^2 J}{\partial p_2^2} & \cdots & \dfrac{\partial^2 J}{\partial p_2\,\partial p_n} \\[2.2ex]
\vdots & \vdots & \ddots & \vdots \\[2.2ex]
\dfrac{\partial^2 J}{\partial p_n\,\partial p_1} & \dfrac{\partial^2 J}{\partial p_n\,\partial p_2} & \cdots & \dfrac{\partial^2 J}{\partial p_n^2}
\end{bmatrix}
\end{equation}

or a function to take the action of the Hessian on a vector.  An objective function measuring the misfit or distance between the simulation and experimental data usually has the form

\begin{equation}\label{eq:objective}
J(\bm{u}, \bm{p}) = \frac{1}{2}\int |\bm{u} - \bm{\bar u}|^2 d\Omega + \frac{\rho}{2}\int \bm{p}^2 d\Omega
\end{equation}

where the first integral is an L2 norm or euclidean distance between the experimental solution, $\bm{\bar u}$, and the simulated solution, $\bm{u}$.  The second integral provides regularization on the parameters, $\bm{p}$, for ill-posed problems where $\rho$ controls the amount of regularization.

Gradient-free optimization solvers only require a callable function to solve for the objective given in [eq:objective].  Solving for the objective only requires solving a forward problem to determine $\bm{u}$ and then plugging that into [eq:objective] to determine $J$.  The forward problem is defined as the FEM model of the experiment which the analyst should have already made before attempting to perform optimization.  The parameters that go into the forward problem (e.g. pressure distributions on sidesets or material properties) are adjusted by the optimization solver and the forward problem is recomputed.  This process continues until $J$ is below some user defined threshold. The basic gradient-free solver available in `TAO` is the simplex or [Nelder-Mead](https://en.wikipedia.org/wiki/Nelder%E2%80%93Mead_method) method.  Gradient-free optimization solvers are robust and straight forward to use.  Unfortunately, their computational cost scales with the number of parameters and when the forward model is a computationally expensive FEM model, gradient-free approaches quickly become too computationally expensive.

Gradient-based optimization algorithms require fewer iterations but require functions to solve for the gradient vector and sometimes Hessians matrix, [eq:hessian].  `TAO` has `petsc_options` to evaluate finite difference based gradients and Hessians by solving the objective function multiple times with perturbed parameters, which requires multiple solves of the forward problem too.  Finite difference gradients and Hessians are great for testing an optimization approach but also become too computationally expensive for realistic problems.

Given the large parameter space, we resort to the adjoint method for gradient computation; unlike finite difference approaches, the computational cost of adjoint methods is independent of the number of parameters (ref18).  In the adjoint method, the gradient, i.e. the total derivative $\mathrm{d}J/\mathrm{d}\bm{p}$, is computed as,

\begin{equation}\label{eq:adjointGrad}
\frac{\mathrm{d}J}{\mathrm{d}\bm{p}} = \frac{\partial J}{\partial\bm{p}}+\bm{\lambda}\frac{\partial\bm{g}}{\partial\bm{p}}
\end{equation}

where $\partial J/\partial\bm{p}$ accounts for the regularization in [eq:objective] and $\bm{\lambda}$ is the adjoint variable solved for from the adjoint equation

\begin{equation}\label{eq:adjoint}
\left(\frac{\partial\bm{g}}{\partial\bm{u}}\right)^{\intercal} \bm{\lambda}= \left(\frac{\partial J}{\partial\bm{u}}\right)^{\intercal}
\end{equation}

where $\left(\partial\bm{g}/\partial\bm{u}\right)^{\intercal}$ is the adjoint of the Jacobian for the original forward problem, $\bm{g}$, and $\left(\partial J/\partial\bm{u}\right)^{\intercal}$ is a body force like term that accounts for the misfit between the computed and experimental data.  Thus, the solution to [eq:adjoint] has the same complexity as the solution to the forward problem.  

The remaining step for computing the gradient in [eq:adjointGrad] is to compute $\partial\bm{g}/\partial\bm{p}$, the derivative of the PDE with respect to the parameter vector.  The form this term takes is dependent on the physics (e.g. mechanics or heat conduction) and the parameter being optimized (e.g. force inversion versus material inversion).  This term can be quite tedious and error prone to implement and is one of the biggest barriers to implementing optimization protocols in different physics.  In what follows, we will derive the adjoint equation for steady state heat conduction and the gradient term for both force and material inversion.  

## Adjoint Problem for Steady State Heat Conduction

In this section, we are interested in solving the following PDE-constrained optimization problem from [eq:optimization] for steady state [heat conduction](/heat_conduction/index.md):

\begin{equation} \label{eqn:optimization_problem}
	\begin{aligned}
	& \min_{\bm{p}}
	& & J\left(T,\bm{p}\right) = \frac{1}{2} \sum_{i=1}^{N} \left( T_i - \bar{T}_i \right)^2 \\
	& \text{subject to}
	& & g\left(T,\bm{p}\right)=\nabla^{\intercal} \cdot \kappa \nabla T + f_b =0, & \text{in}~\Omega \\
\end{aligned}
\end{equation}
where $J$ is our objective function from [eq:objective] without regularization, $f_b$ is the distributed heat flux, $T$ is our experimental temperature field being compared to our simulation temperature at discrete locations, $\bar{T}_i$.  Other forms for the objective function are possible such as different norms or different types of measurements that may require integration over a line or volume.

We also have the following boundary conditions for our PDE,

\begin{equation} \label{eqn:optimization_bcs}
	\begin{aligned}
	& & & T = T_D, &\text{on}~\Omega_D, \\
	& & & \left( \kappa \nabla T \right)^{\intercal} \cdot \boldsymbol{n} = G\left(T\right), &\text{on}~\Omega_R. \\
\end{aligned}
\end{equation}

where $\boldsymbol{n}$ is the normal vector, $\Omega_D$ is the Dirichlet boundary, and $\Omega_R$ is the Robin or mixed boundary. Common  cases for $G\left(T\right)$ are:

\begin{equation}
\begin{aligned}\label{eq:robin_bc_types}
   &\text{Neumann:     }~G\left(T\right) = G = f_n \\
   &\text{Convection:  }~G\left(T\right) = h (T - T_{\infty}) \\
\end{aligned}
\end{equation}
where $h$ is the heat transfer coefficient, see [ConvectiveHeatFluxBC.md], and $f_n$ is independent of $T$.
The objective function can also be expressed in a integral form for the case of point measurements as follows
\begin{equation}  \label{eqn:objective_integral}
	J = \frac{1}{2} \sum_{i=1}^{N} \int_{\Omega} \delta(x - x_i) \left( T - \bar{T}_i \right)^2\text{d}\Omega.
\end{equation}


The Lagrangian functional of this problem is
\begin{equation} \label{eqn:lagrangian}
\begin{aligned}
	\mathcal{L}(T, \bm{p}, \lambda) & = J + \int_{\Omega}  \left(  \nabla^{\intercal} \cdot \kappa \nabla T + f_b  \right) \lambda~\text{d}\Omega \\
					        & = J +  \int_{\Omega}  \left(  f_b \lambda \right) ~\text{d}\Omega+
					        		     \int_{\Omega}  \left(  \nabla^{\intercal} \cdot \kappa \nabla T \right) \lambda~\text{d}\Omega.
\end{aligned}
\end{equation}
The last term in [eqn:lagrangian] can be evaluated by
\begin{equation}
\begin{aligned}
	 \int_{\Omega}  \left(  \nabla^{\intercal} \cdot \kappa \nabla T \right) \lambda~\text{d}\Omega & = \int_{\partial\Omega} \lambda \left( \kappa \nabla T  \right)^{\intercal}\cdot \boldsymbol{n}~\text{d}S - \int_{\Omega} \left(  \kappa\nabla \lambda \right)^{\intercal}\cdot \nabla T~\text{d}\Omega\\
	 & = \int_{\partial\Omega} \lambda \left( \kappa \nabla T \right)^{\intercal}\cdot \boldsymbol{n}~\text{d}S -  \left. \left( \kappa \nabla \lambda  \right)^{\intercal} \cdot \boldsymbol{n} \cdot T \right\vert_{\partial \Omega}  + \int_{\Omega} \nabla^{\intercal}\cdot \kappa \nabla \lambda \cdot T ~\text{d}\Omega.
\end{aligned}
\end{equation}
By substituting the above and [eqn:objective_integral] into [eqn:lagrangian], we have
\begin{equation} \label{eqn:lagrangian_fn}
\begin{aligned}
	\mathcal{L}(T, \bm{p}, \lambda) =& \frac{1}{2} \sum_{i=1}^{N} \int_{\Omega} \delta(x - x_i) \left( T - \bar{T} \right)^2\text{d}\Omega +  \int_{\Omega}  \left(  f \lambda \right) ~\text{d}\Omega  \\
						&+  \int_{\partial\Omega} \lambda \left( \kappa \nabla T \right)^{\intercal}\cdot \boldsymbol{n}~\text{d}S -  \left. \left( \kappa \nabla \lambda  \right)^{\intercal} \cdot \boldsymbol{n} \cdot T \right\vert_{\partial \Omega} + \int_{\Omega} \nabla^{\intercal}\cdot \kappa \nabla \lambda \cdot T ~\text{d}\Omega.
\end{aligned}
\end{equation}
Here, the $\lambda$ is the Lagrange multiplier field known as the adjoint state or costate variable. Taking variation of $\mathcal{L}$ with respect to $T$, we have
\begin{equation}
\begin{aligned}
	\mathcal{L}_T = & \sum_{i=1}^{N} \int_{\Omega} \delta (x - x_i)(T - \bar{T}) \delta T~\text{d}\Omega
			           +  \int_{\partial\Omega} \lambda \left( \kappa \nabla \delta T \right)^{\intercal}\cdot \boldsymbol{n}~\text{d}S \\
			         & -  \left. \left( \kappa \nabla \lambda  \right)^{\intercal} \cdot \boldsymbol{n} \cdot \delta T \right\vert_{\partial \Omega}
			           + \int_{\Omega} \nabla^{\intercal}\cdot \kappa \nabla \lambda \cdot \delta T ~\text{d}\Omega. \\
\end{aligned}
\end{equation}
After grouping terms, we have
\begin{equation} \label{eqn:lagrangian_fn1}
\begin{aligned}
\mathcal{L}_T = &  \int_{\Omega} \left(  \nabla^{\intercal}\cdot \kappa \nabla \lambda \cdot +  \sum_{i=1}^{N} \delta (x - x_i)(T - \bar{T}) \right)\delta T~\text{d}\Omega \\
			      & +  \int_{\partial\Omega} \lambda \left( \kappa \nabla \delta T \right)^{\intercal}\cdot \boldsymbol{n}~\text{d}S
			       -  \left. \left( \kappa \nabla \lambda  \right)^{\intercal} \cdot \boldsymbol{n} \cdot \delta T \right\vert_{\partial \Omega}. \\
\end{aligned}
\end{equation}
Note that admissible $\delta T$ for this typical boundary value problem constraint problem, as shown in [eqn:optimization_problem], will need to satisfy (fixme Dewen  why is this?)
\begin{equation}
\begin{aligned}
	& \delta T = 0, \text{on}~\Omega_D, \\
	& \left( \kappa \nabla \delta T \right)^{\intercal} \cdot \boldsymbol{n} = 0, \text{on}~\Omega_R. \\
\end{aligned}
\end{equation}
Therefore, [eqn:lagrangian_fn1] can be further simplified as
\begin{equation} \label{eqn:lagrangian_fn2}
\begin{aligned}
\mathcal{L}_T = &  \int_{\Omega} \left(  \nabla^{\intercal}\cdot \kappa \nabla \lambda \cdot +  \sum_{i=1}^{N} \delta (x - x_i)(T - \bar{T}) \right)\delta T~\text{d}\Omega \\
			      & +  \int_{\partial\Omega_D} \lambda \left( \kappa \nabla \delta T \right)^{\intercal}\cdot \boldsymbol{n}~\text{d}S
			       -  \left. \left( \kappa \nabla \lambda  \right)^{\intercal} \cdot \boldsymbol{n} \cdot \delta T \right\vert_{\partial \Omega_N}. \\
\end{aligned}
\end{equation}

Stationarity of $\mathcal{L}$ would require $\mathcal{L}_T = 0$ for all admissible $\delta T$. Therefore, we end up with the adjoint problem
\begin{equation} \label{eqn:adjoint_problem}
\boxed{
\begin{aligned}
\nabla^{\intercal}\cdot \kappa \nabla \lambda +  \sum_{i=1}^{N} \delta (x - x_i)(T - \bar{T}) &=0, ~\text{in}~\Omega,\\
\lambda  &= 0, ~\text{on}~\partial \Omega_D, \\
\left( \kappa \nabla \lambda  \right)^{\intercal} \cdot \boldsymbol{n} & = 0, ~\text{on}~{\partial \Omega_N}. \\
\end{aligned}
}
\end{equation}

Solving [eqn:adjoint_problem] comes down to adjusting the boundary conditions and load vector from the forward problem and re-solving.  

Comparing terms of [eqn:adjoint_problem] with [eq:adjoint] we see that

\begin{equation}\label{eq:adjoint}
\begin{aligned}
\left(\frac{\partial\bm{g}}{\partial\bm{u}}\right)^{\intercal} &= \nabla^{\intercal}\cdot \kappa \nabla \\
\left(\frac{\partial J}{\partial\bm{u}}\right)^{\intercal} &= \sum_{i=1}^{N} \delta (x - x_i)(T - \bar{T})\\
\end{aligned}
\end{equation}

The final term required to compute the gradient $\mathrm{d}J/\mathrm{d}\bm{p}$ from $\lambda$ in [eq:adjointGrad] is $\partial\bm{g}/\partial\bm{p}$ which will be derived in the next sections for different types of force inversion and material inversion.

## Gradient for Force Inversion

In this section we will present force inversion for steady state heat conduction given in [eqn:optimization_problem].  For all of these examples, measurement data is taken at specific locations where the objective function can be represented by [eqn:objective_integral].  We will present the discrete forms of our PDE which most closely matches the implementation in MOOSE.  
The discrete form of the PDE constraint for steady state heat conduction in [eqn:optimization_problem] is given as
\begin{equation}\label{eq:galerkin}
\begin{aligned}
\hat{g}&=\bm{K}\hat{T}-\hat{f}=0\\
\bm{K}&=\int_{\Omega} \nabla^{\intercal} \text{N} \cdot \kappa \nabla \text{N}~\text{d}\Omega\\
\hat{T}&=\int_{\Omega} \text{N}\cdot T(x)~\text{d}\Omega\\
\hat{f}&=\int_{\Omega} \text{N}\cdot f(x)~\text{d}\Omega\\
\end{aligned}
\end{equation}
where $T(x)$ and $f(x)$ are continous fields that are multiplied by our interpolation or shape functions, $\text{N}$, and integrated over an element to give nodal the nodal values, $\hat{T}$ and $\hat{f}$.  $\bm{K}$ is the Jacobian matrix.  We are assuming a Galerkin form for our discretized PDE by making our test and trial functions both $\text{N}$.

We will first consider load parameterization of body loads, $f_b$ where the gradient is given as
\begin{equation}\label{eq:bodyLoads}
\begin{aligned}
\hat{f}_b &= \int \text{N}\cdot f_b(x)~\text{d}\Omega\\
\frac{\partial \hat{g}}{\partial\bm{p}} &= \frac{\partial \hat{f}_b}{\partial\bm{p}} = \int \text{N}\cdot\frac{\partial f(x)}{\partial\bm{p}}~\text{d}\Omega\\
\end{aligned}
\end{equation}

The gradient term requires the derivative of $f_b$ to be integrated over the volume $\Omega$.  For the case where we are parameterizing the intensity of point heat sources, the gradient term is given by
\begin{equation}\label{eq:pointLoad}
\begin{aligned}
\hat{f}_b &= \sum_{i=1}^N\int \text{N}\cdot\delta\left(x-x_i \right)p_i d\Omega\\
\frac{\partial \hat{g}}{\partial\bm{p}} &= \frac{\partial \hat{f}_b}{\partial\bm{p}} = \sum_{i=1}^N\int \text{N}\cdot\delta\left(x-x_i \right)d\Omega\\
\end{aligned}
\end{equation}
which makes the gradient equal to one at the locations the load is applied.

For the next case, we will parameterize a heat flux boundary condition that is a function of the coordinates, $f(x)$, given by
\begin{equation}\label{eq:neumannBC}
\begin{aligned}
\hat{f}_n &= \int_{\partial\Omega_D} \text{N}\cdot f(x)~\text{d}S\\
\frac{\partial \hat{g}}{\partial\bm{p}} &= \frac{\partial \hat{f}_n}{\partial\bm{p}} = \int_{\partial\Omega_R} \text{N}\cdot\frac{\partial f(x)}{\partial p}~\text{d}S\\
\end{aligned}
\end{equation}

where the derivative of $f_n$ needs to be integrated over the surface $\partial \Omega_R$. For instance, if we have a linearly varying heat flux, [eq:neumannBC] would be
\begin{equation}\label{eq:neumannBC}
\begin{aligned}
f(x)&=p_1x+p_0\\
\frac{\partial \hat{g}}{\partial\bm{p}} &= \frac{\partial \hat{g}}{\partial{p_0}}+\frac{\partial \hat{g}}{\partial{p_1}} \\
&= \int_{\partial\Omega_R} \text{N}\cdot \left(x+1\right)~\text{d}S\\
\end{aligned}
\end{equation}
The optimization problem becomes nonlinear when the parameters do not drop out of the gradient term, for instance
\begin{equation}\label{eq:neumannBC}
\begin{aligned}
f(x)&=p_1^2x+p_0\\
\frac{\partial \hat{g}}{\partial\bm{p}} &= \int_{\partial\Omega_R} \text{N}\left(p_1x+1\right)~\text{d}S\\
\end{aligned}
\end{equation}
where now, the gradient is still a function of the parameter $p_1$, making this much more difficult for the optimization solver.

## Gradient for Convective Boundary Conditions

For this case we are parameterizing the convective heat transfer coefficient, $h$, for the convection boundary condition from [eq:robin_bc_types] where $G(T)=h \left(T - T_{\infty}\right)$.  This is a material inversion problem where the integral is only over the boundary.  The gradient term is given by
\begin{equation}\label{eq:convectiveBC}
\begin{aligned}
\hat{f}_c &= \int_{\partial\Omega_R} \text{N}\cdot h \left(T - T_{\infty}\right)~\text{d}S\\
\frac{\partial \hat{g}}{\partial\bm{p}} &= \frac{\partial \hat{f}_c}{\partial h} = \int_{\partial\Omega_R} \text{N}\cdot \left(T-T_{\infty} \right)~\text{d}S\\
\end{aligned}
\end{equation}

This requires the solution from the forward problem $T$ to be included in the integral over the boundary $\partial \Omega_R$.

## Gradient for Material Inversion

In material inversion, $\kappa$ is being parameterized.  Material inversion requires the derivative of $\text{K}$ in [eq:pointLoad] leading to
\begin{equation}\label{eq:galerkin}
\begin{aligned}
\frac{\partial \hat{g}}{\partial\bm{p}} &= \frac{\partial \hat{g}}{\partial \kappa}=\frac{\partial\text{K}}{\partial\kappa}\hat{T}\\
&=\int_{\Omega} \nabla^{\intercal} \text{N} \cdot \kappa \nabla \text{N}\hat{T}~\text{d}\Omega\\
&=\int_{\Omega} \nabla^{\intercal} \text{N} \cdot \nabla\hat{T}~\text{d}\Omega\\
\end{aligned}
\end{equation}
where $\nabla\text{N}\hat{T}=\nabla\hat{T}$ was used in the last line.  Material inversion is a nonlinear optimization problem since $\hat{T}$ shows up in our gradient term, making the gradient dependent on the solution to the forward problem.
