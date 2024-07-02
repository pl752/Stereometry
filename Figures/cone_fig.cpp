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

#include "cone_fig.h"
#include "generators.h"
#include <cstring>
#include <numeric>
#include <cmath>
#include <memory>
//----------------------------------------------------------
#pragma package(smart_init)

#ifndef M_SQRT1_2
constexpr double M_SQRT1_2 = M_SQRT2/2.0;
#endif

//Конструктор с кол-вом сечений вращения
Cone_fig::Cone_fig(const
  std::weak_ptr<const AppGLContext>& glContext,
  GLuint slices)
  : Figure3d_unit(glContext)  {
_n_d_face = slices * 2;
_n_d_vertex = slices * 2 + 1;
_Calculate_verts(slices);
_Calculate_inds(slices);
_Calc_Tri_Normals();
}

//Функция для расчёта начального массива вершин
//Верхняя вершина и окружность с центром
void Cone_fig::_Calculate_verts(GLuint _n_slices)   {
vertices_arr.resize(_n_d_vertex*3);
vert_normals_arr.resize(_n_d_vertex*3);
auto disk = std::make_unique<double[]>(_n_slices<<1);
gen_disk(disk.get(), _n_slices);
std::memset(vertices_arr.data(), 0,
  (_n_slices+1)*sizeof(double)*3);

size_t ind = 5, ind_d = 0;
for(GLuint i=0; i<_n_slices; ++i)  {
  vertices_arr[ind] = 1.0;
  ind += 3;
}
ind -= 2;
for(GLuint i=0; i<_n_slices; ++i)  {
  vertices_arr[ind++] = disk[ind_d++];
  vertices_arr[ind++] = disk[ind_d++];
  vertices_arr[ind++] = 0.0;
}

vert_normals_arr[0] = 0.0;
vert_normals_arr[1] = 0.0;
vert_normals_arr[2] = -1.0;
//Константы для числа повторов и смещения для расчёта
//вершинных нормалей
const size_t ind_off = 3*_n_slices,
  cnt_1 = 3*(_n_slices-1);
ind = 3+ind_off, ind_d = 0;
for(GLuint i=0; i<_n_slices; ++i)  {
  vert_normals_arr[ind++] = M_SQRT1_2*disk[ind_d++];
  vert_normals_arr[ind++] = M_SQRT1_2*disk[ind_d++];
  vert_normals_arr[ind++] = M_SQRT1_2;
}
ind = 3;
for(GLuint i=0; i<cnt_1; ++i)  {
  vert_normals_arr[ind] = (vert_normals_arr[ind+ind_off] +
  vert_normals_arr[ind+ind_off+3])/2.0;
  ind++;
}
ind = ind_off;
for(int i=0; i<3; ++i)  {
  vert_normals_arr[ind] = (vert_normals_arr[ind+ind_off] +
  vert_normals_arr[ind+3])/2.0;
  ind++;
}
}

//Функция для расчёта начального массива индексов
//вершин граней
void Cone_fig::_Calculate_inds(GLuint _n_slices)    {
indices_arr.resize(_n_d_face*3);
size_t ind = 0;
//Диск основания
for(GLuint i = 0; i<_n_slices; ++i)    {
  indices_arr[ind++] = 0;
  indices_arr[ind++] = _n_slices+2+i;
  indices_arr[ind++] = _n_slices+1+i;
}
indices_arr[ind-2] = _n_slices+1;
//Боковая поверхность
for(GLuint i = 0; i<_n_slices; ++i)    {
  indices_arr[ind++] = i+1;
  indices_arr[ind++] = _n_slices+1+i;
  indices_arr[ind++] = _n_slices+2+i;
}
indices_arr[ind-1] = _n_slices+1;

flat_face_arr.resize(_n_slices);
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
DLLSO void* NewCone(const void* glContext,
  unsigned int slices)  {
return new Cone_fig(*((const
  std::weak_ptr<const AppGLContext>*)glContext), slices);
}
}
#endif

