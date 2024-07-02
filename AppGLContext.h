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

//������ ������ � ��������� �������
enum F2Mode {
  F2Normal, //������� (����� ������ ������)
  F2CuttingFig //�. ������� ����� ��������� �������
};

class AppGLContext final  {
private:
  //��������� ���������
  static constexpr double yFov = 45.0,
    zNear = 0.25, zFar = 50.0;
  //���� �������� ������ ��� ����� ������ (����. �����.)
  double Cam_x_ang = 45.0, Cam_y_ang = 45.0;
  //������� ����� ������ ������ �� xOy
  double Cam_x_vpos = 0.0, Cam_y_vpos = 0.0;
  //������� ��������� ������ � ������������
  double Cam_x_cpos = 0.0, Cam_y_cpos = 0.0,
    Cam_z_cpos = 0.0;
  //���������� ������ �� ����� ������
  double Cam_distance = 8.0;
  //������� ���� ������
  glm::dvec4 frustum_planes[6];
  //������ ��. �� ������ � ������������ (�� ���������)
  std::vector<Figure3d_unit*> Figures;
  //������ �����. ����� (��� �������� ��� ��������)
  //(���������)
  std::vector<Figure3d_unit*> Figure_satellite;
  //��������� �� ��������� ������
  Figure3d_unit* fig_selection = nullptr;
  //�������� ��������� � ���� (OpenGL) (���������)
  HWND hwnd;
  HGLRC glRC;
  HDC glDC;
  //������� ����
  int wWidth = 0, wHeight = 0;
  //����� �������������� � �������
  F2Mode fig_mode = F2Normal;
  std::weak_ptr<const AppGLContext> self;

  //������� ���������� ARB_vertex_buffer_object (gl>=1.5)
  PFNGLBINDBUFFERPROC glBindBuffer;
  PFNGLDELETEQUERIESPROC glDeleteBuffers;
  PFNGLGENQUERIESPROC glGenBuffers;
  PFNGLBUFFERDATAPROC glBufferData;
  bool gl_arb_buffers = false; //���� ���������

  //������� ���������� GL_vertex/fragment_shader (gl>=2.0)
  PFNGLATTACHSHADERPROC glAttachShader;
  PFNGLCOMPILESHADERPROC glCompileShader;
  PFNGLCREATEPROGRAMPROC glCreateProgram;
  PFNGLCREATESHADERPROC glCreateShader;
  PFNGLDELETEPROGRAMPROC glDeleteProgram;
  PFNGLDELETESHADERPROC glDeleteShader;
  PFNGLLINKPROGRAMPROC glLinkProgram;
  PFNGLSHADERSOURCEPROC glShaderSource;
  PFNGLUSEPROGRAMPROC glUseProgram;
  bool gl_shaders = false; //���� ���������
  GLuint phong_shader;

  //�������� � ��������� ��������� ����� OpenGL
  void Setup_GL_Params();
  //��������� �������
  void Setup_Shaders();
  //����������� ��������� � ��������� ���������� OpenGL
  //(��������� ���������� ������� � ������ ���������)
  void Setup_GL_extensions();
  //������� ��������������� ������
  void ClearSatellite();

  AppGLContext(HWND hwnd, int width, int height);
public:
  //��������� ������������ ���� � ������� ��������
  void Setup_viewPort(int width, int height);
  //���������� ������� ������ � ������� ������ �����
  void Setup_camera();
  //��������� ������ �����
  void DrawFrame() const;
  //��������� �� ������ � ���� ������
  bool FrustumCull(Figure3d_unit* fig) const;
  //�������� ������� ����� ������ (3), ���� ������ (2)
  //� ������� ������ (3) � ������ ������� 8
  void GetCamPos(double*) const;
  //���������� ��������� ������ ������
  inline double GetCamDst() const   {
  return Cam_distance;
  }
  //���������� ������� ����� ������ ������
  inline void SetViewPos(double x, double y) {
  Cam_x_vpos = x, Cam_y_vpos = y;
  }
  //���������� ���� ������ ������
  inline void SetCamAng(double x, double y)   {
  Cam_x_ang = x, Cam_y_ang = y;
  }
  //���������� ��������� ������ ������
  inline void SetCamDst(double d) {
  Cam_distance = d;
  }
  //�������� ������ � ������������
  void AddFigure(Figure3d_unit*);
  //������� ������ �� ������������
  void RemoveFigure(const Figure3d_unit*);
  //�������� ��������� ������
  inline Figure3d_unit* GetSelectedFig() const {
  return fig_selection;
  }
  //���������� ��������� ������
  inline void SetSelectedFig(Figure3d_unit* fig)  {
  fig_selection = fig;
  }
  //���������� �������� �������
  void CtrlSimpleClick(int X, int Y);
  //���������� ����������������� �������� ������
  void CtrlCamRotate(int dX, int dY);
  //���������� ����������������� �������� ������
  void CtrlCamZoom(int dY);
  //���������� ����������������� ����������� ������
  void CtrlCamMoveXoY(int dX, int dY);
  //���������� ����� �������
  bool SetCuttingMode();
  //�������� ��������
  bool CancelOperationMode();
  //�������� ������� �����
  inline F2Mode GetFigMode() const {
  return fig_mode;
  }
  //������� � ������� ��������� ���� � ��������� OpenGL
  static std::shared_ptr<AppGLContext>
    Create(HWND hwnd, int width, int height) {
    auto ctx = std::shared_ptr<AppGLContext>(
      new AppGLContext(hwnd, width, height));
    ctx->self = ctx;
    return ctx;
  }
  ~AppGLContext();
  //���������� �������� ������
  AppGLContext(AppGLContext&) = delete;
  AppGLContext& operator=(AppGLContext&) = delete;
  AppGLContext(AppGLContext&&) = delete;
  AppGLContext& operator=(AppGLContext&&) = delete;

  //������� ���������� ARB_vertex_buffer_object (gl>=1.5)
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
