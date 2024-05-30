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

#include "STLLoadableFig.h"
#include <stl_reader.h>
//----------------------------------------------------------
#pragma package(smart_init)

STLLoadableFig::STLLoadableFig(const AppGLContext *glContext,
  const char* filename) : Figure3d_unit(glContext)    {
std::vector<unsigned int> solids_arr;
stl_reader::ReadStlFile<std::vector<double>,
  std::vector<double>, std::vector<GLuint>,
  std::vector<unsigned int>>(filename, vertices_arr,
  face_normals_arr, indices_arr, solids_arr);
_n_d_face = indices_arr.size()/3;
_n_d_vertex = vertices_arr.size()/3;
_Calc_Bound_Sphere();
MakePermanentlyFlat();
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
DLLSO void* NewSTLLoadableFig(const void* glContext,
  const char* filename)    {
return new STLLoadableFig(
  L(AppGLContext*)glContext, filename);
}
}
#endif

