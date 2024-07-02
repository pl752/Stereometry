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

#ifndef ArrayFigH
#define ArrayFigH

#include "figure3d_unit.h"
//----------------------------------------------------------

//������, ����������� ��������� ������ � ��������
//(���-�� ������ � ����� ���������)
class ArrayFig : public Figure3d_unit   {
public:
  ArrayFig(const
    std::weak_ptr<const AppGLContext>& glContext,
    const double *verts, const GLuint *inds,
    GLuint num_verts, GLuint num_inds, GLuint vert_per_face);
  ArrayFig(const ArrayFig &other) :
    Figure3d_unit(other) {}
  Figure3d_unit* VirtualCopy() const override   {
    return new ArrayFig(*this);
  }
  ArrayFig(ArrayFig&&) = default;

  static ArrayFig* NewTetrahedron(const
  std::weak_ptr<const AppGLContext>& glContext);
  static ArrayFig* NewCube(const
  std::weak_ptr<const AppGLContext>& glContext);
  static ArrayFig* NewOctahedron(const
  std::weak_ptr<const AppGLContext>& glContext);
  static ArrayFig* NewDodecahedron(const
  std::weak_ptr<const AppGLContext>& glContext);
  static ArrayFig* NewIcosahedron(const
  std::weak_ptr<const AppGLContext>& glContext);
  static ArrayFig* NewRhombicdodecahedron(const
  std::weak_ptr<const AppGLContext>& glContext);
};
#endif
