//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

// MOOSE includes
#include "SamplerReceiver.h"

/**
 * A Control object for receiving data from a master application Sampler object.
 */
class ControlsReceiver : public SamplerReceiver
{
public:
  static InputParameters validParams();

  ControlsReceiver(const InputParameters & parameters);

protected:
  // this is a copy of SamplerReciever with a new Friend
  friend class OptimizationParameterTransfer;
};
