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

#ifndef cone_figH
#define cone_figH

#include "figure3d_unit.h"
//Класс конуса (тело вращения)
class Cone_fig : public Figure3d_unit  {
protected:
  void _Calculate_verts(GLuint _n_slices);
  void _Calculate_inds(GLuint _n_slices);
public:
  //Конструктор с кол-вом сечений вращения
  Cone_fig(const AppGLContext *glContext, GLuint slices);
  Cone_fig(const Cone_fig &other) : Figure3d_unit(other) {}
  Cone_fig& operator=(const Cone_fig &other) {
    Figure3d_unit::operator=(other);
    return *this;
  }
  Figure3d_unit* VirtualCopy() const override   {
    return new Cone_fig(*this);
  }
  Cone_fig(Cone_fig&&) = default;
};

#endif
