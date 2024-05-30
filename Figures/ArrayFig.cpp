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

#include "ArrayFig.h"
#include <stdexcept>
#include <cstring>
//----------------------------------------------------------
#pragma package(smart_init)

//Предварительно вычисленные исходные массивы и параметры
#define TETRAHEDRON_NUM_VERT            4
#define TETRAHEDRON_NUM_FACES           4
#define TETRAHEDRON_NUM_EDGE_PER_FACE   3
constexpr double tetrahedron_v[TETRAHEDRON_NUM_VERT*3] =
{
            1.0,             0.0,             0.0,
-0.333333333333,  0.942809041582,             0.0,
-0.333333333333, -0.471404520791,  0.816496580928,
-0.333333333333, -0.471404520791, -0.816496580928
};

constexpr GLuint tetrahedron_i[TETRAHEDRON_NUM_FACES*3] =
{
1, 3, 2,
0, 2, 3,
0, 3, 1,
0, 1, 2
};

#define CUBE_NUM_VERT           8
#define CUBE_NUM_FACES          6
#define CUBE_NUM_EDGE_PER_FACE  4
constexpr double cube_v[CUBE_NUM_VERT*3] =
{
 1.0, 1.0, 1.0,
-1.0, 1.0, 1.0,
-1.0,-1.0, 1.0,
 1.0,-1.0, 1.0,
 1.0,-1.0,-1.0,
 1.0, 1.0,-1.0,
-1.0, 1.0,-1.0,
-1.0,-1.0,-1.0
};

constexpr GLuint cube_i[
  CUBE_NUM_FACES*CUBE_NUM_EDGE_PER_FACE] =
{
0,1,2,3,
0,3,4,5,
0,5,6,1,
1,6,7,2,
7,4,3,2,
4,7,6,5
};

#define OCTAHEDRON_NUM_VERT           6
#define OCTAHEDRON_NUM_FACES          8
#define OCTAHEDRON_NUM_EDGE_PER_FACE  3
constexpr double octahedron_v[OCTAHEDRON_NUM_VERT*3] =
{
 1.0,  0.0,  0.0,
 0.0,  1.0,  0.0,
 0.0,  0.0,  1.0,
-1.0,  0.0,  0.0,
 0.0, -1.0,  0.0,
 0.0,  0.0, -1.0,
};

constexpr GLuint octahedron_i[OCTAHEDRON_NUM_FACES*3] =
{
0, 1, 2,
0, 5, 1,
0, 2, 4,
0, 4, 5,
3, 2, 1,
3, 1, 5,
3, 4, 2,
3, 5, 4
};

#define DODECAHEDRON_NUM_VERT           20
#define DODECAHEDRON_NUM_FACES          12
#define DODECAHEDRON_NUM_EDGE_PER_FACE  5
constexpr double dodecahedron_v[DODECAHEDRON_NUM_VERT*3] =
{
           0.0,  1.61803398875,  0.61803398875,
-          1.0,            1.0,            1.0,
-0.61803398875,            0.0,  1.61803398875,
 0.61803398875,            0.0,  1.61803398875,
           1.0,            1.0,            1.0,
           0.0,  1.61803398875, -0.61803398875,
           1.0,            1.0, -          1.0,
 0.61803398875,            0.0, -1.61803398875,
-0.61803398875,            0.0, -1.61803398875,
-          1.0,            1.0, -          1.0,
           0.0, -1.61803398875,  0.61803398875,
           1.0, -          1.0,            1.0,
-          1.0, -          1.0,            1.0,
           0.0, -1.61803398875, -0.61803398875,
-          1.0, -          1.0, -          1.0,
           1.0, -          1.0, -          1.0,
 1.61803398875, -0.61803398875,            0.0,
 1.61803398875,  0.61803398875,            0.0,
-1.61803398875,  0.61803398875,            0.0,
-1.61803398875, -0.61803398875,            0.0
};

constexpr GLuint dodecahedron_i[
  DODECAHEDRON_NUM_FACES*DODECAHEDRON_NUM_EDGE_PER_FACE] =
{
 0,  1,  2,  3,  4,
 5,  6,  7,  8,  9,
10, 11,  3,  2, 12,
13, 14,  8,  7, 15,

 3, 11, 16, 17,  4,
 2,  1, 18, 19, 12,
 7,  6, 17, 16, 15,
 8, 14, 19, 18,  9,

17,  6,  5,  0,  4,
16, 11, 10, 13, 15,
18,  1,  0,  5,  9,
19, 14, 13, 10, 12
};

#define ICOSAHEDRON_NUM_VERT           12
#define ICOSAHEDRON_NUM_FACES          20
#define ICOSAHEDRON_NUM_EDGE_PER_FACE  3
constexpr double icosahedron_v[ICOSAHEDRON_NUM_VERT*3] =
{
            1.0,             0.0,             0.0,
 0.447213595500,  0.894427191000,             0.0,
 0.447213595500,  0.276393202252,  0.850650808354,
 0.447213595500, -0.723606797748,  0.525731112119,
 0.447213595500, -0.723606797748, -0.525731112119,
 0.447213595500,  0.276393202252, -0.850650808354,
-0.447213595500, -0.894427191000,             0.0,
-0.447213595500, -0.276393202252,  0.850650808354,
-0.447213595500,  0.723606797748,  0.525731112119,
-0.447213595500,  0.723606797748, -0.525731112119,
-0.447213595500, -0.276393202252, -0.850650808354,
-           1.0,             0.0,             0.0
};

constexpr GLuint icosahedron_i[ICOSAHEDRON_NUM_FACES*3] =
{
0,   1,  2 ,
0,   2,  3 ,
0,   3,  4 ,
0,   4,  5 ,
0,   5,  1 ,
1,   8,  2 ,
2,   7,  3 ,
3,   6,  4 ,
4,  10,  5 ,
5,   9,  1 ,
1,   9,  8 ,
2,   8,  7 ,
3,   7,  6 ,
4,   6, 10 ,
5,  10,  9 ,
11,  9, 10 ,
11,  8,  9 ,
11,  7,  8 ,
11,  6,  7 ,
11, 10,  6
};

#define RHOMBICDODECAHEDRON_NUM_VERT            14
#define RHOMBICDODECAHEDRON_NUM_FACES           12
#define RHOMBICDODECAHEDRON_NUM_EDGE_PER_FACE   4
constexpr double rhombicdodecahedron_v[
  RHOMBICDODECAHEDRON_NUM_VERT*3] =
{
            0.0,             0.0,  1.0,
 0.707106781187,             0.0,  0.5,
            0.0,  0.707106781187,  0.5,
-0.707106781187,             0.0,  0.5,
            0.0, -0.707106781187,  0.5,
 0.707106781187,  0.707106781187,  0.0,
-0.707106781187,  0.707106781187,  0.0,
-0.707106781187, -0.707106781187,  0.0,
 0.707106781187, -0.707106781187,  0.0,
 0.707106781187,             0.0, -0.5,
            0.0,  0.707106781187, -0.5,
-0.707106781187,             0.0, -0.5,
            0.0, -0.707106781187, -0.5,
            0.0,             0.0, -1.0
};

constexpr GLuint rhombicdodecahedron_i[
  RHOMBICDODECAHEDRON_NUM_FACES*
  RHOMBICDODECAHEDRON_NUM_EDGE_PER_FACE] =
{
0,  1,  5,  2,
0,  2,  6,  3,
0,  3,  7,  4,
0,  4,  8,  1,
5, 10,  6,  2,
6, 11,  7,  3,
7, 12,  8,  4,
8,  9,  5,  1,
5,  9, 13, 10,
6, 10, 13, 11,
7, 11, 13, 12,
8, 12, 13,  9
};


ArrayFig::ArrayFig(const AppGLContext *glContext,
  const double *verts, const GLuint *inds,
  GLuint num_verts, GLuint num_faces,
  GLuint vert_per_face) : Figure3d_unit(glContext)  {
if(vert_per_face < 3)
  throw std::runtime_error("Min face is triangle");
_n_d_vertex = num_verts;
_n_d_face = num_faces*(vert_per_face-2);
vertices_arr.resize(num_verts*3);
indices_arr.resize(_n_d_face*3);
std::memcpy(vertices_arr.data(), verts,
  num_verts*3*sizeof(double));
if(vert_per_face == 3)  {
  std::memcpy(indices_arr.data(), inds,
    num_faces*3*sizeof(GLuint));
}
else    {
  _Simple_triang_fan(indices_arr.data(), inds,
    vert_per_face, num_faces);
}
_Calc_Bound_Sphere();
_Calc_Tri_Normals();
MakePermanentlyFlat();
}

ArrayFig* ArrayFig::NewTetrahedron(
  const AppGLContext *glContext)    {
return new ArrayFig(glContext, tetrahedron_v, tetrahedron_i,
  TETRAHEDRON_NUM_VERT, TETRAHEDRON_NUM_FACES,
    TETRAHEDRON_NUM_EDGE_PER_FACE);
}

ArrayFig* ArrayFig::NewCube(
  const AppGLContext *glContext)   {
return new ArrayFig(glContext, cube_v, cube_i,
  CUBE_NUM_VERT, CUBE_NUM_FACES, CUBE_NUM_EDGE_PER_FACE);
}

ArrayFig* ArrayFig::NewOctahedron(
  const AppGLContext *glContext) {
return new ArrayFig(glContext, octahedron_v, octahedron_i,
  OCTAHEDRON_NUM_VERT, OCTAHEDRON_NUM_FACES,
  OCTAHEDRON_NUM_EDGE_PER_FACE);
}

ArrayFig* ArrayFig::NewDodecahedron(
  const AppGLContext *glContext)   {
return new ArrayFig(glContext,
  dodecahedron_v, dodecahedron_i,
  DODECAHEDRON_NUM_VERT, DODECAHEDRON_NUM_FACES,
  DODECAHEDRON_NUM_EDGE_PER_FACE);
}

ArrayFig* ArrayFig::NewIcosahedron(
  const AppGLContext *glContext)    {
return new ArrayFig(glContext, icosahedron_v, icosahedron_i,
  ICOSAHEDRON_NUM_VERT, ICOSAHEDRON_NUM_FACES,
  ICOSAHEDRON_NUM_EDGE_PER_FACE);
}

ArrayFig* ArrayFig::NewRhombicdodecahedron(
  const AppGLContext *glContext)    {
return new ArrayFig(glContext,
  rhombicdodecahedron_v, rhombicdodecahedron_i,
  RHOMBICDODECAHEDRON_NUM_VERT,
  RHOMBICDODECAHEDRON_NUM_FACES,
  RHOMBICDODECAHEDRON_NUM_EDGE_PER_FACE);
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
DLLSO void* NewArrayFig(const void* glContext,
  const double *verts, const unsigned int *inds,
  unsigned int num_verts, unsigned int num_faces,
  unsigned int vert_per_face)    {
return new ArrayFig((AppGLContext*)glContext, verts, inds,
  num_verts, num_faces, vert_per_face);
}

DLLSO void* NewTetrahedron(const void* glContext)    {
return ArrayFig::NewTetrahedron((AppGLContext*)glContext);
}

DLLSO void* NewCube(const void* glContext)    {
return ArrayFig::NewCube((AppGLContext*)glContext);
}

DLLSO void* NewOctahedron(const void* glContext)    {
return ArrayFig::NewOctahedron((AppGLContext*)glContext);
}

DLLSO void* NewDodecahedron(const void* glContext)    {
return ArrayFig::NewDodecahedron((AppGLContext*)glContext);
}

DLLSO void* NewIcosahedron(const void* glContext)    {
return ArrayFig::NewIcosahedron((AppGLContext*)glContext);
}

DLLSO void* NewRhombicdodecahedron(const void* glContext) {
return ArrayFig::NewRhombicdodecahedron(
  (AppGLContext*)glContext);
}
}
#endif

