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

#include "cylinder_fig.h"
#include "generators.h"
#include <cmath>
#include <cstring>
#include <numeric>
#include <memory>
//----------------------------------------------------------
#pragma package(smart_init)

//Конструктор с кол-вом сечений вращения
Cylinder_fig::Cylinder_fig(const AppGLContext *glContext,
  GLuint slices) : Figure3d_unit(glContext)  {
_n_d_face = slices * 4;
_n_d_vertex = slices * 2 + 2;
sphere_radius = M_SQRT2;
_Calculate_verts(slices);
_Calculate_inds(slices);
_Calc_Tri_Normals();
}

//Функция для расчёта начального массива вершин
void Cylinder_fig::_Calculate_verts(GLuint _n_slices)   {
vertices_arr.resize(_n_d_vertex*3);
vert_normals_arr.resize(_n_d_vertex*3);
auto disk = std::make_unique<double[]>(_n_slices<<1);
gen_disk(disk.get(), _n_slices);
std::memset(vertices_arr.data(), 0, sizeof(double)*5);
//Центральные точки окружностей
vertices_arr[2] = -1.0;
vertices_arr[5] = 1.0;
size_t ind = 6, ind_d = 0;
//Нижнее основание цилиндра
for(GLuint i=0; i<_n_slices; ++i)  {
  vertices_arr[ind++] = disk[ind_d++];
  vertices_arr[ind++] = disk[ind_d++];
  vertices_arr[ind++] = -1.0;
}
ind_d = 0;
//Верхнее основание цилиндра
for(GLuint i=0; i<_n_slices; ++i)  {
  vertices_arr[ind++] = disk[ind_d++];
  vertices_arr[ind++] = disk[ind_d++];
  vertices_arr[ind++] = 1.0;
}

std::memset(vert_normals_arr.data(), 0, sizeof(double)*5);
//Центральные точки окружностей
vert_normals_arr[2] = -1.0;
vert_normals_arr[5] = 1.0;
ind = 6, ind_d = 0;
//Нижнее основание цилиндра
for(GLuint i=0; i<_n_slices; ++i)  {
  vert_normals_arr[ind++] = disk[ind_d++];
  vert_normals_arr[ind++] = disk[ind_d++];
  vert_normals_arr[ind++] = 0.0;
}
ind_d = 0;
//Верхнее основание цилиндра
for(GLuint i=0; i<_n_slices; ++i)  {
  vert_normals_arr[ind++] = disk[ind_d++];
  vert_normals_arr[ind++] = disk[ind_d++];
  vert_normals_arr[ind++] = 0.0;
}
}

//Функция для расчёта начального массива индексов
//вершин граней
void Cylinder_fig::_Calculate_inds(GLuint _n_slices)    {
indices_arr.resize(_n_d_face*3);
size_t ind = 0;
//Нижнее основание цилиндра
for(GLuint i = 0; i<_n_slices; ++i)    {
  indices_arr[ind++] = 0;
  indices_arr[ind++] = i+3;
  indices_arr[ind++] = i+2;
}
indices_arr[ind-2] = 2;
//Верхнее основание цилиндра
for(GLuint i = _n_slices; i<2*_n_slices; ++i)    {
  indices_arr[ind++] = 1;
  indices_arr[ind++] = i+2;
  indices_arr[ind++] = i+3;
}
indices_arr[ind-1] = 2+_n_slices;
//Боковая поверхность цилиндра
for(GLuint i = 0; i<_n_slices; ++i) {
  indices_arr[ind++] = i+_n_slices+2;
  indices_arr[ind++] = i+2;
  indices_arr[ind++] = i+3;
}
indices_arr[ind-1] = 2;
for(GLuint i = 0; i<_n_slices; ++i) {
  indices_arr[ind++] = i+3;
  indices_arr[ind++] = i+3+_n_slices;
  indices_arr[ind++] = i+2+_n_slices;
}
indices_arr[ind-2] = 2+_n_slices;
indices_arr[ind-3] = 2;

flat_face_arr.resize(2*_n_slices);
std::iota(flat_face_arr.begin(),
  flat_face_arr.end(), 0);
}

//Extern секция для динамической библиотеки
#ifdef DLL
//Включение экспорта функции в зависимости от платформы
#ifdef _WIN32
#define DLLSO __declspec(dllexport)
#else
#define DLLSO __attribute__((visibility("default")))
#endif

extern "C" {
DLLSO void* NewCylinder(const void* glContext,
  unsigned int slices)    {
return new Cylinder_fig((AppGLContext*)glContext, slices);
}
}
#endif

