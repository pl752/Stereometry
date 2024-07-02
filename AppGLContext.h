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

#ifndef AppGLContextH
#define AppGLContextH
//----------------------------------------------------------
#include <Windows.h>
#include "GLtypes.h"
#include <GL/glext.h>
#include <glm/vec4.hpp>
//----------------------------------------------------------
#include <vector>
#include <map>
#include <memory>
//----------------------------------------------------------

class Figure3d_unit;

//Режимы работы с выбранной фигурой
enum F2Mode {
  F2Normal, //Обычный (режим выбора фигуры)
  F2CuttingFig //Р. задания точек плоскости сечения
};

class AppGLContext final  {
private:
  //Временные параметры
  static constexpr double yFov = 45.0,
    zNear = 0.25, zFar = 50.0;
  //Угол поворота камеры отн точки обзора (сфер. коорд.)
  double Cam_x_ang = 45.0, Cam_y_ang = 45.0;
  //Позиция точки обзора камеры на xOy
  double Cam_x_vpos = 0.0, Cam_y_vpos = 0.0;
  //Позиция основания камеры в пространстве
  double Cam_x_cpos = 0.0, Cam_y_cpos = 0.0,
    Cam_z_cpos = 0.0;
  //Расстояние камеры от точки обзора
  double Cam_distance = 8.0;
  //Границы поля зрения
  glm::dvec4 frustum_planes[6];
  //Список ук. на фигуры в пространстве (не владеющий)
  std::vector<Figure3d_unit*> Figures;
  //Список вспом. фигур (для операций над фигурами)
  //(владеющий)
  std::vector<Figure3d_unit*> Figure_satellite;
  //Указатель на выбранную фигуру
  Figure3d_unit* fig_selection = nullptr;
  //Контекст отрисовки и окна (OpenGL) (владеющие)
  HWND hwnd;
  HGLRC glRC;
  HDC glDC;
  //Размеры окна
  int wWidth = 0, wHeight = 0;
  //Режим взаимодействия с фигурой
  F2Mode fig_mode = F2Normal;
  std::weak_ptr<const AppGLContext> self;

  //Функции расширения ARB_vertex_buffer_object (gl>=1.5)
  PFNGLBINDBUFFERPROC glBindBuffer;
  PFNGLDELETEQUERIESPROC glDeleteBuffers;
  PFNGLGENQUERIESPROC glGenBuffers;
  PFNGLBUFFERDATAPROC glBufferData;
  bool gl_arb_buffers = false; //Флаг поддержки

  //Функции расширения GL_vertex/fragment_shader (gl>=2.0)
  PFNGLATTACHSHADERPROC glAttachShader;
  PFNGLCOMPILESHADERPROC glCompileShader;
  PFNGLCREATEPROGRAMPROC glCreateProgram;
  PFNGLCREATESHADERPROC glCreateShader;
  PFNGLDELETEPROGRAMPROC glDeleteProgram;
  PFNGLDELETESHADERPROC glDeleteShader;
  PFNGLLINKPROGRAMPROC glLinkProgram;
  PFNGLSHADERSOURCEPROC glShaderSource;
  PFNGLUSEPROGRAMPROC glUseProgram;
  bool gl_shaders = false; //Флаг поддержки
  GLuint phong_shader;

  //Включить и настроить параметры опций OpenGL
  void Setup_GL_Params();
  //Настроить шейдеры
  void Setup_Shaders();
  //Определение поддержки и настройка расширений OpenGL
  //(Настройка указателей функций и флагов поддержки)
  void Setup_GL_extensions();
  //Удалить вспомогательные фигуры
  void ClearSatellite();

  AppGLContext(HWND hwnd, int width, int height);
public:
  //Настройка графического окна и матрицы проекции
  void Setup_viewPort(int width, int height);
  //Обновление позиции камеры и матрицы обзора фигур
  void Setup_camera();
  //Отрисовка нового кадра
  void DrawFrame() const;
  //Находится ли фигура в поле зрения
  bool FrustumCull(Figure3d_unit* fig) const;
  //Записать позицию точки обзора (3), угол обзора (2)
  //и позицию камеры (3) в массив размера 8
  void GetCamPos(double*) const;
  //Установить дистанцию обзора камеры
  inline double GetCamDst() const   {
  return Cam_distance;
  }
  //Установить позицию точки обзора камеры
  inline void SetViewPos(double x, double y) {
  Cam_x_vpos = x, Cam_y_vpos = y;
  }
  //Установить угол обзора камеры
  inline void SetCamAng(double x, double y)   {
  Cam_x_ang = x, Cam_y_ang = y;
  }
  //Установить дистанцию обзора камеры
  inline void SetCamDst(double d) {
  Cam_distance = d;
  }
  //Добавить фигуру в пространство
  void AddFigure(Figure3d_unit*);
  //Удалить фигуру из пространства
  void RemoveFigure(const Figure3d_unit*);
  //Получить выбранную фигуру
  inline Figure3d_unit* GetSelectedFig() const {
  return fig_selection;
  }
  //Установить выбранную фигуру
  inline void SetSelectedFig(Figure3d_unit* fig)  {
  fig_selection = fig;
  }
  //Обработчик простого нажатия
  void CtrlSimpleClick(int X, int Y);
  //Обработчик пользовательского поворота камеры
  void CtrlCamRotate(int dX, int dY);
  //Обработчик пользовательского масштаба камеры
  void CtrlCamZoom(int dY);
  //Обработчик пользовательского перемещения камеры
  void CtrlCamMoveXoY(int dX, int dY);
  //Установить режим сечения
  bool SetCuttingMode();
  //Отменить операцию
  bool CancelOperationMode();
  //Получить текущий режим
  inline F2Mode GetFigMode() const {
  return fig_mode;
  }
  //Создать и выбрать контексты окна и отрисовки OpenGL
  static std::shared_ptr<AppGLContext>
    Create(HWND hwnd, int width, int height) {
    auto ctx = std::shared_ptr<AppGLContext>(
      new AppGLContext(hwnd, width, height));
    ctx->self = ctx;
    return ctx;
  }
  ~AppGLContext();
  //Копировать контекст нельзя
  AppGLContext(AppGLContext&) = delete;
  AppGLContext& operator=(AppGLContext&) = delete;
  AppGLContext(AppGLContext&&) = delete;
  AppGLContext& operator=(AppGLContext&&) = delete;

  //Функции расширения ARB_vertex_buffer_object (gl>=1.5)
  inline bool HasVertexBufferObject() const {
  return gl_arb_buffers;
  }
  inline void BindBuffer(GLenum target,
    GLuint buffer) const   {
  glBindBuffer(target, buffer);
  }
  inline void DeleteBuffers(GLsizei n,
    const GLuint *buffers) const  {
  glDeleteBuffers(n, buffers);
  }
  inline void GenBuffers(GLsizei n,
    GLuint *buffers) const  {
  glGenBuffers(n, buffers);
  }
  inline void BufferData(GLenum target, GLsizeiptr size,
    const void *data, GLenum usage) const {
  glBufferData(target, size, data, usage);
  }
};

#endif
