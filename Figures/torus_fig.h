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

#ifndef torus_figH
#define torus_figH

#include "figure3d_unit.h"
//Класс бублика (тело вращения)
class Torus_fig : public Figure3d_unit  {
protected:
  void _Calculate_verts(
    GLuint _nu_slices, GLuint _nv_slices,
    double _u_radius, double _v_radius);
  void _Calculate_inds(
    GLuint _nu_slices, GLuint _nv_slices);
public:
  //Конструктор с кол-вом сечений вращения и радиусами
  //(гор и верт)
  Torus_fig(const AppGLContext *glContext,
    GLuint u_slices, GLuint v_slices,
    double u_rad, double v_rad);
  Torus_fig(const Torus_fig &other) :
    Figure3d_unit(other) {}
  Torus_fig& operator=(const Torus_fig &other) {
    Figure3d_unit::operator=(other);
    return *this;
  }
  Figure3d_unit* VirtualCopy() const override   {
    return new Torus_fig(*this);
  }
  Torus_fig(Torus_fig&&) = default;
};

#endif
