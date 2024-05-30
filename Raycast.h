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

#ifndef RaycastH
#define RaycastH

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

//�������� ����������� ���� �� ����� �� ������
//X, Y - ��������������� ���������� ����� �� ������ (-1; 1)
//ray - ����� ����������� ����
void getRaycast(double x, double y, glm::dvec3 &ray);

//�������� ������� ���������� ���������� �� ���� ��
//������ ����������
//cam - ������� ������ (����� �������)
//ray - ����������� ����
//center - ����� ������ ����������
double intersectSphere(const glm::dvec3 &cam,
  const glm::dvec3 &ray, const glm::dvec3 &center);

//�������� ������ ����� ����������� ���� � ����������
//������� �����
//cam - ������� ������ (����� �������)
//ray - ����������� ����
//norm - ������� ���������
//plane_pt - ����� �� ��������� ����� (�����)
//out - ����� ��������� ����� ������� ����������� ��� ������
//dst - ����� ���������� �� ������ �� ����� �����������
//���������� true ��� ������
bool planeIntersectPt(const glm::dvec3 &cam,
  const glm::dvec3 &ray, const glm::dvec3 &norm,
  const glm::dvec3 &plane_pt,
  glm::dvec3 &out, double &dst);

//�������� ������ ����� ����������� ������ � ����������
//pt1, pt2 - ����� ������
//plane - ����. ��������� ���������
//out - ����� ��������� ����� ������� �����������
void planeIntersectPtAlt(const glm::dvec3 &pt1,
  const glm::dvec3 &pt2, const glm::dvec4 &plane,
  glm::dvec3 &out);

//���������� true, ���� ����� ����� � ������������
//plane_pt - ���������� �����
//triangle - ������ �� 3-� ����� ������������
//��������� ������ ������������
bool pointInTriangle(const glm::dvec3 &plane_pt,
  const glm::dvec3 *triangle);

//�������� ������� � ������������
//triangle - ������ �� 3-� ����� ������������
//��������� ������ ������������
//out - ����� �������
void getNormal(const glm::dvec3 *triangle, glm::dvec3 &out);

//�������� ������������ ��������
//pt_* - ���������� ����� �� ������
//norm_* - ������� ������ �� ������ �������
//out - ����� ������� � ������� �����
void normalLerp(const glm::dvec3 &pt_begin,
  const glm::dvec3 &pt_mid, const glm::dvec3 &pt_end,
  const glm::dvec3 &norm_begin, const glm::dvec3 &norm_end,
  glm::dvec3 &out);
#endif
