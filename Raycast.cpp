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
 
#pragma hdrstop

#include "Raycast.h"
#include <windows.h>
#include <GL/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cmath>
#include <algorithm>
//----------------------------------------------------------
#pragma package(smart_init)

//�������� ����������� ���� �� ����� �� ������
//X, Y - ��������������� ���������� ����� �� ������ (-1; 1)
//ray - ������ ������� 3 ��� ������ ����������� ����
void getRaycast(double x, double y, glm::dvec3 &ray) {
GLfloat mat[16];
//�������� ������� �������� � ������
glGetFloatv(GL_MODELVIEW_MATRIX, mat);
glm::dmat4x4 iview_mat = glm::make_mat4(mat);
iview_mat = inverse(iview_mat);
glGetFloatv(GL_PROJECTION_MATRIX, mat);
glm::dmat4x4 iproj_mat = glm::make_mat4(mat);
iproj_mat = inverse(iproj_mat);
//��� � �������� �����������
glm::dvec4 ray_h(x, y, -1.0, 1.0);
//��� � �������� �����������
ray_h = iproj_mat * ray_h;
ray_h.z = -1.0;
ray_h.w = 0.0;
//��� � ������� �����������
ray = normalize(iview_mat * ray_h);
}

//�������� ������� ���������� ���������� �� ���� ��
//������ ����������
//cam - ������� ������ (����� �������)
//ray - ����������� ����
//center - ����� ������ ����������
//(������� ������� 3)
double intersectSphere(const glm::dvec3 &cam,
  const glm::dvec3 &ray, const glm::dvec3 &center) {
glm::dvec3 dir = center-cam;
double b = glm::dot(ray, dir);
//��� ������ �������� �� ������, � �� ��-�� ��
if(b<0.0) return -INFINITY;
return b*b-glm::dot(dir, dir);
}

//�������� ������ ����� ����������� ���� � ����������
//������� �����
//cam - ������� ������ (����� �������)
//ray - ����������� ����
//norm - ������� ���������
//plane_pt - ����� �� ��������� ����� (�����)
//out - ����� ��������� ����� ������� ����������� ��� ������
//(������� ������� 3)
//dst - ����� ���������� �� ������ �� ����� �����������
//���������� true ��� ������
bool planeIntersectPt(const glm::dvec3 &cam,
  const glm::dvec3 &ray, const glm::dvec3 &norm,
  const glm::dvec3 &plane_pt,
  glm::dvec3 &out, double &dst)    {
double side = glm::dot(ray, norm);
//���� ��� � ��������� ����������� ��� ����� �� �������
if(side >= 0.0) return false;
double d = -glm::dot(plane_pt, norm);
double t = -(glm::dot(cam, norm)+d)/side;
//��� ������ �������� �� ������, � �� ��-�� ��
if(t<0.0) return false;
out = cam + t*ray;
dst = t;
return true;
}

//�������� ������ ����� ����������� ������ � ����������
//pt1, pt2 - ����� ������ (������� ������� 3)
//plane - ����. ��������� ��������� (������ 4)
//out - ����� ��������� ����� ������� �����������
//(������ ������� 3)
void planeIntersectPtAlt(const glm::dvec3 &pt1,
  const glm::dvec3 &pt2, const glm::dvec4 &plane,
  glm::dvec3 &out)   {
glm::dvec3 ray_v = pt2-pt1, norm = plane;
double side = glm::dot(ray_v, norm);
//���� ������ ����� � ��������� ��� �����������
if(std::fabs(side) < 1e-9) {
  out = pt1;
    return;
}
double t = -(glm::dot(pt1, norm)+plane.w)/side;
out = pt1 + t*ray_v;
}

//���������� true, ���� ����� ����� � ������������
//plane_pt - ���������� ����� (������ ������� 3)
//triangle - ������ �� 3-� �������� ������� 3
//��������� ������ ������������
bool pointInTriangle(const glm::dvec3 &plane_pt,
  const glm::dvec3 *triangle) {
glm::dvec3 sides[3], dv[3];
for(int i = 0; i<3; ++i)    {
  sides[i] = triangle[i] - plane_pt;
}
dv[0] = glm::cross(sides[1], sides[2]);
dv[1] = glm::cross(sides[2], sides[0]);
dv[2] = glm::cross(sides[0], sides[1]);
//���� ������� ��� ���������� �������������
//������������, �� ����� ����� � ������������
return (glm::dot(dv[0], dv[1]) >= 0.0 &&
  glm::dot(dv[1], dv[2]) >= 0.0);
}

//�������� ������� � ������������
//triangle - ������ �� 3-� �������� ������� 3
//��������� ������ ������������
//out - ����� ������� (������ ������� 3)
void getNormal(const glm::dvec3 *triangle,
  glm::dvec3 &out)  {
glm::dvec3 a, b;
a = triangle[1] - triangle[0];
b = triangle[2] - triangle[0];
a = glm::cross(a, b);
double norm = glm::length(a);
//���� ����������� �������� � ������� ��� �����
if(norm < 1e-9) {
  out = {0,0,1};
}
else    {
  out = a/norm;
}
}

//�������� ������������ ��������
//pt_* - ���������� ����� �� ������
//norm_* - ������� ������ �� ������ �������
//out - ����� ������� � ������� �����
void normalLerp(const glm::dvec3 &pt_begin,
  const glm::dvec3 &pt_mid, const glm::dvec3 &pt_end,
  const glm::dvec3 &norm_begin, const glm::dvec3 &norm_end,
  glm::dvec3 &out)    {
glm::dvec3 a = abs(pt_mid-pt_begin),
  b = abs(pt_end-pt_begin);
double t = std::max(b.x, std::max(b.y, b.z));
if(t < 1e-9)    {
  out = norm_begin;
}
else {
  t = std::max(a.x, std::max(a.y, a.z))/t;
  if(t>1.0) out = norm_end;
  else if(t<0.0) out = norm_begin;
  else out = (1.0-t)*norm_begin+t*norm_end;
}
}

