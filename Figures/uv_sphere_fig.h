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

#ifndef uv_sphere_figH
#define uv_sphere_figH

#include "figure3d_unit.h"
//Класс глобуса (тело вращения)
class UV_Sphere_fig : public Figure3d_unit  {
protected:
  void _Calculate_verts(GLuint _nu_slices,
    GLuint _nv_slices);
  void _Calculate_inds(GLuint _nu_slices,
    GLuint _nv_slices);
public:
  //Конструктор с кол-вом сечений вращения (гор. и верт.)
  UV_Sphere_fig(const
    std::weak_ptr<const AppGLContext>& glContext,
    GLuint u_slices, GLuint v_slices);
  UV_Sphere_fig(const UV_Sphere_fig &other) :
    Figure3d_unit(other) {}
  UV_Sphere_fig& operator=(const UV_Sphere_fig &other) {
    Figure3d_unit::operator=(other);
  return *this;
  }
  Figure3d_unit* VirtualCopy() const  override   {
    return new UV_Sphere_fig(*this);
  }
  UV_Sphere_fig(UV_Sphere_fig&&) = default;
};

#endif
