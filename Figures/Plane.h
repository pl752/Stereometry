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

#ifndef PlaneH
#define PlaneH

#include <Figures/figure3d_unit.h>
//Класс полупрозрачной плоскости (пока не используется)
class Plane : public Figure3d_unit {
//Углы поворота и четвёртый коэф
double a, b, d;

public:
  //Конструкторы с коэф. уравнения плоскости
  Plane(double a, double b, double c, double d);
  Plane(const double *p) : Plane(p[0], p[1], p[2], p[3]) {}
  Plane(const Plane &other) : Figure3d_unit(nullptr),
    a(other.a), b(other.b), d(other.d) {}
  Plane& operator=(const Plane& other)  {
    if(this == &other) return *this;
    a = other.a;
    b = other.b;
    d = other.d;
    return *this;
  }
  Figure3d_unit* VirtualCopy() const override  {
    return new Plane(*this);
  }
  void Draw() const override;
};

#endif
