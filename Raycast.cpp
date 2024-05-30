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

//Получить направление луча по точке на экране
//X, Y - Нормализованные координаты точки на экране (-1; 1)
//ray - массив размера 3 для вывода направления луча
void getRaycast(double x, double y, glm::dvec3 &ray) {
GLfloat mat[16];
//Получить матрицы проекции и обзора
glGetFloatv(GL_MODELVIEW_MATRIX, mat);
glm::dmat4x4 iview_mat = glm::make_mat4(mat);
iview_mat = inverse(iview_mat);
glGetFloatv(GL_PROJECTION_MATRIX, mat);
glm::dmat4x4 iproj_mat = glm::make_mat4(mat);
iproj_mat = inverse(iproj_mat);
//Луч в экранных координатах
glm::dvec4 ray_h(x, y, -1.0, 1.0);
//Луч в обзорных координатах
ray_h = iproj_mat * ray_h;
ray_h.z = -1.0;
ray_h.w = 0.0;
//Луч в мировых координатах
ray = normalize(iview_mat * ray_h);
}

//Получить квадрат ближайшего расстояния от луча до
//центра окружности
//cam - Позиция камеры (точка отсчёта)
//ray - Направление луча
//center - точка центра окружности
//(Массивы размера 3)
double intersectSphere(const glm::dvec3 &cam,
  const glm::dvec3 &ray, const glm::dvec3 &center) {
glm::dvec3 dir = center-cam;
double b = glm::dot(ray, dir);
//Луч должен выходить из камеры, а не из-за неё
if(b<0.0) return -INFINITY;
return b*b-glm::dot(dir, dir);
}

//Получить точную точку пересечения луча с плоскостью
//лицевой грани
//cam - Позиция камеры (точка отсчёта)
//ray - Направление луча
//norm - Нормаль плоскости
//plane_pt - Точка на плоскости грани (любая)
//out - Вывод координат точки точного пересечения при успехе
//(Массивы размера 3)
//dst - Вывод расстояния от камеры до точки пересечения
//Возвращает true при успехе
bool planeIntersectPt(const glm::dvec3 &cam,
  const glm::dvec3 &ray, const glm::dvec3 &norm,
  const glm::dvec3 &plane_pt,
  glm::dvec3 &out, double &dst)    {
double side = glm::dot(ray, norm);
//Если луч и плоскость параллельны или грань не лицевая
if(side >= 0.0) return false;
double d = -glm::dot(plane_pt, norm);
double t = -(glm::dot(cam, norm)+d)/side;
//Луч должен выходить из камеры, а не из-за неё
if(t<0.0) return false;
out = cam + t*ray;
dst = t;
return true;
}

//Получить точную точку пересечения прямой с плоскостью
//pt1, pt2 - точки прямой (Массивы размера 3)
//plane - Коэф. уравнения плоскости (размер 4)
//out - Вывод координат точки точного пересечения
//(Массив размера 3)
void planeIntersectPtAlt(const glm::dvec3 &pt1,
  const glm::dvec3 &pt2, const glm::dvec4 &plane,
  glm::dvec3 &out)   {
glm::dvec3 ray_v = pt2-pt1, norm = plane;
double side = glm::dot(ray_v, norm);
//если прямая лежит в плоскости или параллельна
if(std::fabs(side) < 1e-9) {
  out = pt1;
    return;
}
double t = -(glm::dot(pt1, norm)+plane.w)/side;
out = pt1 + t*ray_v;
}

//Возврашает true, если точка лежит в треугольнике
//plane_pt - Координаты точки (массив размера 3)
//triangle - Массив из 3-х массивов размера 3
//координат вершин треугольника
bool pointInTriangle(const glm::dvec3 &plane_pt,
  const glm::dvec3 *triangle) {
glm::dvec3 sides[3], dv[3];
for(int i = 0; i<3; ++i)    {
  sides[i] = triangle[i] - plane_pt;
}
dv[0] = glm::cross(sides[1], sides[2]);
dv[1] = glm::cross(sides[2], sides[0]);
dv[2] = glm::cross(sides[0], sides[1]);
//если нормали трёх внутренних треугольников
//сонаправлены, то точка лежит в треугольнике
return (glm::dot(dv[0], dv[1]) >= 0.0 &&
  glm::dot(dv[1], dv[2]) >= 0.0);
}

//Получить нормаль к треугольнику
//triangle - Массив из 3-х массивов размера 3
//координат вершин треугольника
//out - Вывод нормали (массив размера 3)
void getNormal(const glm::dvec3 *triangle,
  glm::dvec3 &out)  {
glm::dvec3 a, b;
a = triangle[1] - triangle[0];
b = triangle[2] - triangle[0];
a = glm::cross(a, b);
double norm = glm::length(a);
//Если треугольник вырожден в отрезок или точку
if(norm < 1e-9) {
  out = {0,0,1};
}
else    {
  out = a/norm;
}
}

//Линейная интерполяция нормалей
//pt_* - координаты точки на прямой
//norm_* - нормали вершин на концах отрезка
//out - Вывод нормали в средней точке
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

