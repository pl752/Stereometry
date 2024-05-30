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

#include "uv_sphere_fig.h"
#include "generators.h"
#include <cstring>
#include <memory>
//----------------------------------------------------------
#pragma package(smart_init)

//Конструктор с кол-вом сечений вращения (гор. и верт.)
UV_Sphere_fig::UV_Sphere_fig(const AppGLContext *glContext,
  GLuint u_slices, GLuint v_slices)
  : Figure3d_unit(glContext)  {
_n_d_face = u_slices*(v_slices-1)*2;
_n_d_vertex = u_slices*(v_slices-1)+2;
_Calculate_verts(u_slices, v_slices);
_Calculate_inds(u_slices, v_slices);
_Calc_Tri_Normals();
}

//Функция для расчёта начального массива вершин
void UV_Sphere_fig::_Calculate_verts(GLuint _nu_slices,
  GLuint _nv_slices)   {
vertices_arr.resize(_n_d_vertex*3);
auto u_disk = std::make_unique<double[]>(_nu_slices<<1);
auto v_disk = std::make_unique<double[]>(_nv_slices<<2);
gen_disk(u_disk.get(), _nu_slices);
gen_disk(v_disk.get(), _nv_slices<<1);
std::memset(vertices_arr.data(), 0, sizeof(double)*5);
//Осевые точки глобуса
vertices_arr[2] = 1.0;
vertices_arr[5] = -1.0;
size_t ind = 6;
//Боковые точки глобуса
for(GLuint i=1; i<_nv_slices; ++i) {
  for(GLuint j=0; j<_nu_slices; ++j) {
    vertices_arr[ind++] = u_disk[2*j]*v_disk[2*i+1];
    vertices_arr[ind++] = u_disk[2*j+1]*v_disk[2*i+1];
    vertices_arr[ind++] = v_disk[2*i];
  }
}
u_disk.reset();
v_disk.reset();
vert_normals_arr = vertices_arr;
}

//Функция для расчёта начального массива индексов
//вершин граней
void UV_Sphere_fig::_Calculate_inds(GLuint _nu_slices,
  GLuint _nv_slices)    {
indices_arr.resize(_n_d_face*3);
size_t ind = 0, vert = 2;
//Верхнее основание глобуса
for(GLuint i = 0; i<_nu_slices; ++i)    {
  indices_arr[ind++] = 0;
  indices_arr[ind++] = vert++;
  indices_arr[ind++] = vert;
}
indices_arr[ind-1] = 2;
vert = 2;
GLuint j;
//Боковые диски глобуса
for(GLuint i=1; i<_nv_slices-1; ++i)   {
  for(j=0; j<_nu_slices; ++j) {
    indices_arr[ind++] = vert+j+1;
    indices_arr[ind++] = vert+j;
    indices_arr[ind++] = vert+_nu_slices+j;
  }
  indices_arr[ind-3] = vert;
  for(j=0; j<_nu_slices; ++j) {
    indices_arr[ind++] = vert+_nu_slices+j;
    indices_arr[ind++] = vert+_nu_slices+j+1;
    indices_arr[ind++] = vert+j+1;
  }
  indices_arr[ind-1] = vert;
  vert += _nu_slices;
  indices_arr[ind-2] = vert;
}
//Нижнее основание глобуса
for(GLuint i = 0; i<_nu_slices; ++i)    {
  indices_arr[ind++] = 1;
  indices_arr[ind++] = vert+1;
  indices_arr[ind++] = vert++;
}
indices_arr[ind-2] = vert-_nu_slices;
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
DLLSO void* NewUVSphere(const void* glContext,
  unsigned int u_slices, unsigned int v_slices)    {
return new UV_Sphere_fig((AppGLContext*)glContext,
  u_slices, v_slices);
}
}
#endif

