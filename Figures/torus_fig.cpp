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

#include "torus_fig.h"
#include "generators.h"
#include <memory>
//----------------------------------------------------------
#pragma package(smart_init)

//Конструктор с кол-вом сечений вращения и радиусами
//(гор и верт)
Torus_fig::Torus_fig(const AppGLContext *glContext,
  GLuint u_slices, GLuint v_slices,
  double u_rad, double v_rad) : Figure3d_unit(glContext) {
_n_d_face = u_slices*v_slices*2;
_n_d_vertex = u_slices*v_slices;
sphere_radius = u_rad + v_rad;
_Calculate_verts(u_slices, v_slices, u_rad, v_rad);
_Calculate_inds(u_slices, v_slices);
_Calc_Tri_Normals();
}

//Функция для расчёта начального массива вершин
//Поверхность бублика
void Torus_fig::_Calculate_verts(
  GLuint _nu_slices, GLuint _nv_slices,
  double _u_radius, double _v_radius)   {
vertices_arr.resize(_n_d_vertex*3);
vert_normals_arr.resize(_n_d_vertex*3);
auto u_disk = std::make_unique<double[]>(_nu_slices<<1);
auto v_disk = std::make_unique<double[]>(_nv_slices<<1);
gen_disk(u_disk.get(), _nu_slices);
gen_disk(v_disk.get(), _nv_slices);
size_t ind = 0;
for(GLuint i=0; i<_nu_slices; ++i) {
  for(GLuint j=0; j<_nv_slices; ++j) {
    vertices_arr[ind++] = u_disk[2*i]*
      (_u_radius+_v_radius*v_disk[2*j]);
    vertices_arr[ind++] = u_disk[2*i+1]*
      (_u_radius+_v_radius*v_disk[2*j]);
    vertices_arr[ind++] = v_disk[2*j+1]*_v_radius;
  }
}
ind = 0;
for(GLuint i=0; i<_nu_slices; ++i) {
  for(GLuint j=0; j<_nv_slices; ++j) {
    vert_normals_arr[ind++] = u_disk[2*i]*v_disk[2*j];
    vert_normals_arr[ind++] = u_disk[2*i+1]*v_disk[2*j];
    vert_normals_arr[ind++] = v_disk[2*j+1];
  }
}
}

//Функция для расчёта начального массива индексов
//вершин граней
void Torus_fig::_Calculate_inds(
  GLuint _nu_slices, GLuint _nv_slices)    {
indices_arr.resize(_n_d_face*3);
size_t ind = 0, vert = 0;
//Поверхность бублика
for(GLuint i=0; i<_nu_slices-1; ++i) {
  for(GLuint j=0; j<_nv_slices; ++j) {
    indices_arr[ind++] = vert+j+1;
    indices_arr[ind++] = vert+j;
    indices_arr[ind++] = _nv_slices+vert+j;
  }
  indices_arr[ind-3] = _nv_slices*i;
  for(GLuint j=0; j<_nv_slices; ++j) {
    indices_arr[ind++] = _nv_slices+vert+j;
    indices_arr[ind++] = _nv_slices+vert+j+1;
    indices_arr[ind++] = vert+j+1;
  }
  indices_arr[ind-1] = vert;
  vert += _nv_slices;
  indices_arr[ind-2] = vert;
}
//Перввое звено поверхности бублика
for(GLuint j=0; j<_nv_slices; ++j) {
  indices_arr[ind++] = vert+j+1;
  indices_arr[ind++] = vert+j;
  indices_arr[ind++] = j;
}
indices_arr[ind-3] = vert;
for(GLuint j=0; j<_nv_slices; ++j) {
  indices_arr[ind++] = j;
  indices_arr[ind++] = j+1;
  indices_arr[ind++] = vert+j+1;
}
indices_arr[ind-1] = vert;
indices_arr[ind-2] = 0;
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
DLLSO void* NewTorus(const void* glContext,
  unsigned int u_slices, unsigned int v_slices,
  double u_rad, double v_rad)    {
return new Torus_fig((AppGLContext*)glContext,
  u_slices, v_slices, u_rad, v_rad);
}
}
#endif

