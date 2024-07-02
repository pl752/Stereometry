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

#ifndef figure3d_unitH
#define figure3d_unitH

#include "AppGLContext.h"
#include "GLtypes.h"
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <vector>

#define M_SQRT3 1.7320508075688772935274463415059

//��������� ������� ������������ ������� ������
//dest_ind - ����� � ������ ��������
//n*(k-2) �������������
//src_ind - ������ k ��������� ������
//point_num - ���-�� ������ ��������� ������ (k)
//face_num - ���-�� ��������� ������ (n)
void _Simple_triang_fan(GLuint *dest_ind,
  const GLuint *src_ind, GLuint point_num,
  GLuint face_num);

//������� ����������� ����� ��������� ������
class Figure3d_unit {
private:
protected:
  //�������� ��������� (OpenGL) (�� ��������� ���������)
  std::weak_ptr<const AppGLContext> glContext;
  double sphere_radius = 1.0; //������ ������������ �����
  glm::dvec3 Offset; //������� ������� ������ ������
  glm::dvec3 Offset_old; //������� ������� ������ ������
  glm::dvec3 Scale; //������� ������� ������
  glm::dvec3 Scale_old; //������� ������� ������
  //������ ��������� ������
  std::vector<double> vertices_arr;
  //������ �������� ���������� ������
  std::vector<double> face_normals_arr;
  //������ �������� ������ (������� �������)
  std::vector<double> vert_normals_arr;
  //������ �������� ������ ����������� ������
  std::vector<GLuint> indices_arr;
  //������ �������� ������, ������������ ��������� �������
  std::vector<GLuint> flat_face_arr;
  //������ ��� glDrawArrays (gl<1.5)
  mutable std::vector<GLfloat> draw_arr;
  GLuint _n_d_face = 0; //���-�� ������������� ������
  GLuint _n_d_vertex = 0; //���-�� ������
  //������ ��� glDrawArrays (gl>=1.5)
  mutable GLuint _vbo = 0;
  //���� ������������� �������� draw_arr
  mutable bool _draw_buf_upd_flag = true;
  //���� ���������� ����������� ��������
  bool no_vec_normals = false;
  //����������� ���������� ������ ����� ���������������
  void _Calc_Scale();
  //����������� ���������� ������ ����� ��������
  void _Calc_Offset();
  //����������� ���������� draw_arr ��� vbo
  void _PrepareBuffers() const;
  //���������� ������ �������� ������
  void _Calc_Tri_Normals();
  //���������� ������ ������������ �����
  void _Calc_Bound_Sphere();
public:
  //����������� � ��������� � ���������
  Figure3d_unit(const std::weak_ptr<const AppGLContext>&);
  virtual ~Figure3d_unit();
  //�����������
  Figure3d_unit(const Figure3d_unit &other);
  Figure3d_unit& operator=(const Figure3d_unit &other);
  virtual Figure3d_unit* VirtualCopy() const   {
  return new Figure3d_unit(*this);
  }
  //�����������
  Figure3d_unit(Figure3d_unit&&) = default;
  Figure3d_unit& operator=(Figure3d_unit&&) = default;
  //�������� �������� ���������
  void ReplaceContext(const
    std::weak_ptr<const AppGLContext>& other);
  //������ ������� ������
  void Resize(double x, double y, double z);
  //��������� ������ (������ XYZ) �������
  void Rotate(double x, double y, double z);
  //������ ������� ������ ������
  void Move(double x, double y, double z);
  inline void Resize(const double *v)  {
  Resize(v[0],v[1],v[2]);
  }
  inline void Rotate(const double *v)  {
  Rotate(v[0],v[1],v[2]);
  }
  inline void Move(const double *v)    {
  Move(v[0],v[1],v[2]);
  }
  //�������� ������� ������ ������ � ������
  void GetCenterPos(double *v) const;
  //�������� ������� ������ � ������
  void GetScale(double *v) const;
  //�������� ������ ������������ ����������
  double GetSphRadius() const {return sphere_radius;};
  //������� ��������� ������� � ������������
  //������������� ������� �������
  void MakePermanentlyFlat();
  //��������� ����������� � ������������ ������
  //cam - ������� ������ (����� �������)
  //ray - ����������� ����
  //���������� true ��� ������� �����������
  bool SphereIntersect(const glm::dvec3 &cam,
    const glm::dvec3 &ray) const;
  //�������� ������ ����������� � �������
  //cam - ������� ������ (����� �������)
  //ray - ����������� ����
  //norm - ������� ���������
  //plane_pt - ����� �� ��������� ����� (�����)
  //out - ����� ��������� ����� �������
  //����������� ��� ������
  //(������� ������� 3)
  //dst - ����� ���������� �� ������ �� ����� �����������
  //���������� true ��� ������
  bool IntersectPoint(const glm::dvec3 &cam,
    const glm::dvec3 &ray,
    glm::dvec3 &out, double &dst) const;
  //��������� ��������� ������
  virtual void Draw() const;
  //��������� ������� ������ ����������
  //plane - ����. ��������� ������� ��������� (������ 4)
  void PerformCut(const glm::dvec4 &plane);
};

#endif
