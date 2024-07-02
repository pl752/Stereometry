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

#include "Plane.h"
#include <Windows.h>
#include <GL/gl.h>
#include <cmath>
//----------------------------------------------------------
#pragma package(smart_init)

constexpr double RAD2DEG = 180.0/M_PI;

//Конструктор с коэф. уравнения плоскости
Plane::Plane(double a, double b, double c, double d) :
  Figure3d_unit(std::weak_ptr<const AppGLContext>())  {
this->a = c==0.0f ? 90.0 : RAD2DEG*std::atan2(
  std::sqrt(a*a+b*b), c);
this->b = 90.0+RAD2DEG*std::atan2(b, a);
this->d = d/std::sqrt(a*a+b*b+c*c);
MakePermanentlyFlat();
}

//Отрисовка полупрозрачной плоскости без отсеивания граней
void Plane::Draw() const {
GLboolean cull, blend;
glGetBooleanv(GL_CULL_FACE, &cull);
glGetBooleanv(GL_BLEND, &blend);
if(cull) glDisable(GL_CULL_FACE);
if(!blend) glEnable(GL_BLEND);
glPushMatrix();
glRotatef(b, 0, 0, 1);
glRotatef(a, 1, 0, 0);
glTranslatef(0, 0, -d);
//TODO: Привязка к коорд. камеры
glBegin(GL_QUADS);
  glColor4f(0, 1, 0, 0.7);
  glVertex2f(100, 100);
  glVertex2f(-100, 100);
  glVertex2f(-100, -100);
  glVertex2f(100, -100);
glEnd();
glPopMatrix();
if(!blend) glDisable(GL_BLEND);
if(cull) glEnable(GL_CULL_FACE);
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
DLLSO void* NewPlane(
  double a, double b, double c, double d)    {
return new Plane(a, b, c, d);
}
}
#endif

