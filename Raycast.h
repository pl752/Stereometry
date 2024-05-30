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

//Получить направление луча по точке на экране
//X, Y - Нормализованные координаты точки на экране (-1; 1)
//ray - вывод направления луча
void getRaycast(double x, double y, glm::dvec3 &ray);

//Получить квадрат ближайшего расстояния от луча до
//центра окружности
//cam - Позиция камеры (точка отсчёта)
//ray - Направление луча
//center - точка центра окружности
double intersectSphere(const glm::dvec3 &cam,
  const glm::dvec3 &ray, const glm::dvec3 &center);

//Получить точную точку пересечения луча с плоскостью
//лицевой грани
//cam - Позиция камеры (точка отсчёта)
//ray - Направление луча
//norm - Нормаль плоскости
//plane_pt - Точка на плоскости грани (любая)
//out - Вывод координат точки точного пересечения при успехе
//dst - Вывод расстояния от камеры до точки пересечения
//Возвращает true при успехе
bool planeIntersectPt(const glm::dvec3 &cam,
  const glm::dvec3 &ray, const glm::dvec3 &norm,
  const glm::dvec3 &plane_pt,
  glm::dvec3 &out, double &dst);

//Получить точную точку пересечения прямой с плоскостью
//pt1, pt2 - точки прямой
//plane - Коэф. уравнения плоскости
//out - Вывод координат точки точного пересечения
void planeIntersectPtAlt(const glm::dvec3 &pt1,
  const glm::dvec3 &pt2, const glm::dvec4 &plane,
  glm::dvec3 &out);

//Возврашает true, если точка лежит в треугольнике
//plane_pt - Координаты точки
//triangle - Массив из 3-х точек треугольника
//координат вершин треугольника
bool pointInTriangle(const glm::dvec3 &plane_pt,
  const glm::dvec3 *triangle);

//Получить нормаль к треугольнику
//triangle - Массив из 3-х точек треугольника
//координат вершин треугольника
//out - Вывод нормали
void getNormal(const glm::dvec3 *triangle, glm::dvec3 &out);

//Линейная интерполяция нормалей
//pt_* - координаты точки на прямой
//norm_* - нормали вершин на концах отрезка
//out - Вывод нормали в средней точке
void normalLerp(const glm::dvec3 &pt_begin,
  const glm::dvec3 &pt_mid, const glm::dvec3 &pt_end,
  const glm::dvec3 &norm_begin, const glm::dvec3 &norm_end,
  glm::dvec3 &out);
#endif
