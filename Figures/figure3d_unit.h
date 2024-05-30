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

#ifndef figure3d_unitH
#define figure3d_unitH

#include "AppGLContext.h"
#include "GLtypes.h"
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <vector>

#define M_SQRT3 1.7320508075688772935274463415059

//Выполнить веерную триангуляцию массива граней
//dest_ind - вывод в массив индексов
//n*(k-2) треугольников
//src_ind - массив k начальных граней
//point_num - кол-во вершин начальных граней (k)
//face_num - кол-во начальных граней (n)
void _Simple_triang_fan(GLuint *dest_ind,
  const GLuint *src_ind, GLuint point_num,
  GLuint face_num);

//Базовый абстрактный класс трёхмерной фигуры
class Figure3d_unit {
private:
protected:
  //Контекст отрисовки (OpenGL) (не владеющий указатель)
  const AppGLContext *glContext;
  double sphere_radius = 1.0; //Радиус охватывающей сферы
  glm::dvec3 Offset; //Текущая позиция центра фигуры
  glm::dvec3 Offset_old; //Прошлая позиция центра фигуры
  glm::dvec3 Scale; //Текущий масштаб фигуры
  glm::dvec3 Scale_old; //Прошлый масштаб фигуры
  //Массив координат вершин
  std::vector<double> vertices_arr;
  //Массив нормалей плоскостей граней
  std::vector<double> face_normals_arr;
  //Массив нормалей вершин (гладкий шейдинг)
  std::vector<double> vert_normals_arr;
  //Массив индексов вершин треугольных граней
  std::vector<GLuint> indices_arr;
  //Массив индексов граней, использующих вершинные нормали
  std::vector<GLuint> flat_face_arr;
  //Массив для glDrawArrays (gl<1.5)
  mutable std::vector<GLfloat> draw_arr;
  GLuint _n_d_face = 0; //Кол-во треугольников граней
  GLuint _n_d_vertex = 0; //Кол-во вершин
  //Буффер для glDrawArrays (gl>=1.5)
  mutable GLuint _vbo = 0;
  //Флаг необходимости обновить draw_arr
  mutable bool _draw_buf_upd_flag = true;
  //Флаг для отслеживания готовности контекста
  std::shared_ptr<const bool> ContextValidityFlag;
  //Флаг отсутствия повершинных нормалей
  bool no_vec_normals = false;
  //Пересчитать координаты вершин после масштабирования
  void _Calc_Scale();
  //Пересчитать координаты вершин после смещения
  void _Calc_Offset();
  //Подготовить содержимое draw_arr или vbo
  void _PrepareBuffers() const;
  //Рассчитать массив нормалей граней
  void _Calc_Tri_Normals();
  //Рассчитать радиус охватывающей сферы
  void _Calc_Bound_Sphere();
public:
  //Конструктор с привязкой к контексту
  Figure3d_unit(const AppGLContext *glContext);
  virtual ~Figure3d_unit();
  //Копирование
  Figure3d_unit(const Figure3d_unit &other);
  Figure3d_unit& operator=(const Figure3d_unit &other);
  virtual Figure3d_unit* VirtualCopy() const   {
  return new Figure3d_unit(*this);
  }
  //Перемещение
  Figure3d_unit(Figure3d_unit&&) = default;
  Figure3d_unit& operator=(Figure3d_unit&&) = default;
  //Заменить контекст отрисовки
  void ReplaceContext(const AppGLContext *other);
  //Задать масштаб фигуры
  void Resize(double x, double y, double z);
  //Повернуть фигуру (Эйлера XYZ) радианы
  void Rotate(double x, double y, double z);
  //Задать позицию центра фигуры
  void Move(double x, double y, double z);
  inline void Resize(const double *v)  {
  Resize(v[0],v[1],v[2]);
  }
  inline void Rotate(const double *v)  {
  Rotate(v[0],v[1],v[2]);
  }
  inline void Move(const double *v)    {
  Move(v[0],v[1],v[2]);
  }
  //Получить позицию центра фигуры в массив
  void GetCenterPos(double *v) const;
  //Получить масштаб фигуры в массив
  void GetScale(double *v) const;
  //Получить радиус охватывающей окружности
  double GetSphRadius() const {return sphere_radius;};
  //Удалить вершинные нормали и использовать
  //исключительно лицевые нормали
  void MakePermanentlyFlat();
  //Проверить пересечение с охватывающей сферой
  //cam - Позиция камеры (точка отсчёта)
  //ray - Направление луча
  //Возвращает true при наличии пересечения
  bool SphereIntersect(const glm::dvec3 &cam,
    const glm::dvec3 &ray) const;
  //Получить точное пересечение с фигурой
  //cam - Позиция камеры (точка отсчёта)
  //ray - Направление луча
  //norm - Нормаль плоскости
  //plane_pt - Точка на плоскости грани (любая)
  //out - Вывод координат точки точного
  //пересечения при успехе
  //(Массивы размера 3)
  //dst - Вывод расстояния от камеры до точки пересечения
  //Возвращает true при успехе
  bool IntersectPoint(const glm::dvec3 &cam,
    const glm::dvec3 &ray,
    glm::dvec3 &out, double &dst) const;
  //Выполнить отрисовку фигуры
  virtual void Draw() const;
  //Выполнить сечение фигуры плоскостью
  //plane - коэф. уравнения секущей плоскости (размер 4)
  void PerformCut(const glm::dvec4 &plane);
};

#endif
