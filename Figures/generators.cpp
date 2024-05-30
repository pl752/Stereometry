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

#include "generators.h"
#include <cmath>
//----------------------------------------------------------
#pragma package(smart_init)

//—генерировать точки аппр. окружности
//dest - вывод XY пар координат точек
//slices - количество точек
void gen_disk(double *dest, size_t slices)   {
double ang = 0.0;
for(size_t i = 0; i<slices; ++i)   {
  size_t k = i<<1;
  dest[k] = std::cos(ang);
  dest[k+1] = std::sin(ang);
  ang += 2.0*M_PI/slices;
}
}

