/********************************************************************************
 * Copyright (c) 2024 Pleshkov Maksim
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0, or the Apache License, Version 2.0
 * which is available at https://www.apache.org/licenses/LICENSE-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0 OR Apache-2.0
 ********************************************************************************/

#ifndef STLLoadableFigH
#define STLLoadableFigH

#include "figure3d_unit.h"
//----------------------------------------------------------

class STLLoadableFig : public Figure3d_unit {
public:
  STLLoadableFig(const AppGLContext *glContext,
    const char* filename);
  STLLoadableFig(const STLLoadableFig &other) :
    Figure3d_unit(other) {}
  Figure3d_unit* VirtualCopy() const override   {
    return new STLLoadableFig(*this);
  }
  STLLoadableFig(STLLoadableFig&&) = default;
};
#endif
