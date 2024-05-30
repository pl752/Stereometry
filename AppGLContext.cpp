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

#include "AppGLContext.h"
#include "Raycast.h"
#include "Figures/ArrayFig.h"
//----------------------------------------------------------
#include <GL/gl.h>
#include <GL/glu.h>
//----------------------------------------------------------
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cmath>
#include <stdexcept>
#include <algorithm>
//----------------------------------------------------------
#pragma package(smart_init)

constexpr double DEG2RAD = M_PI/180.0;

using namespace std;

//��������� ����������� ������ ��� Handle-� ��. ����
static void SetupPixelFormat(HDC hdc) {
PIXELFORMATDESCRIPTOR pfd;
int pixf;
pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
pfd.nVersion = 1;
pfd.dwFlags = PFD_DRAW_TO_WINDOW |
  PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
pfd.dwLayerMask = PFD_MAIN_PLANE;
pfd.iPixelType = PFD_TYPE_RGBA;
pfd.cColorBits = 24;
pfd.cDepthBits = 32;
pfd.cAccumBits = 0;
pfd.cStencilBits = 0;
if((pixf = ChoosePixelFormat(hdc, &pfd))==0)    {
  throw std::runtime_error("ChoosePixelFormat fail");
}
if(SetPixelFormat(hdc, pixf, &pfd) == FALSE)    {
  throw std::runtime_error("SetPixelFormat fail");
}
}

#ifdef _DEBUG
void __stdcall GLMessageCallback(GLenum source, GLenum type,
  GLuint id, GLenum severity, GLsizei length,
  const GLchar* message, const void* userParam)  {
char err[1024];
sprintf_s(err, 1024,
  "GL CALLBACK: %s\ntype = 0x%x,\nseverity = 0x%x,\n\
  message = %s\n",
  (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
  type, severity, message );
MessageBoxA(NULL, err, "GLError", MB_OK);
}
#endif

//�������� ������� ����� ������ (3), ���� ������ (2)
//� ������� ������ (3) � ������ ������� 8
void AppGLContext::GetCamPos(double* pos) const {
pos[0] = Cam_x_vpos;
pos[1] = Cam_y_vpos;
pos[2] = 0.0;
pos[3] = Cam_x_cpos;
pos[4] = Cam_y_cpos;
pos[5] = Cam_z_cpos;
pos[6] = Cam_x_ang;
pos[7] = Cam_y_ang;
}

//������� ��������������� ������
void AppGLContext::ClearSatellite()   {
for(auto x: Figure_satellite)   {
  delete x;
}
Figure_satellite.clear();
}

//�������� ������ � ������������
void AppGLContext::AddFigure(Figure3d_unit *fig)    {
//����������� ������ �� ����� ������
fig->Move(Cam_x_vpos, Cam_y_vpos, 0.0);
//����������� ������ � ������������
Figures.push_back(fig);
}

//������� ������ �� ������������
void AppGLContext::RemoveFigure(const Figure3d_unit* fig) {
auto it = std::remove(Figures.begin(), Figures.end(), fig);
Figures.erase(it, Figures.end());
if(fig == fig_selection) fig_selection = nullptr;
}

//������� � ������� ��������� ���� � ��������� OpenGL
AppGLContext::AppGLContext(HWND hwnd,
  int width, int height) : hwnd(hwnd)  {
glDC = GetDC(hwnd);
SetupPixelFormat(glDC);
glRC = wglCreateContext(glDC);
wglMakeCurrent(glDC, glRC);
Setup_GL_extensions();
#ifdef _DEBUG
auto proc = wglGetProcAddress("glDebugMessageCallback");
if(proc) {
  glEnable( GL_DEBUG_OUTPUT );
  ((PFNGLDEBUGMESSAGECALLBACKPROC)proc)(
    GLMessageCallback, 0);
}
#endif
//��������� ������� �������� � ������
Setup_viewPort(width, height);
Setup_camera();
Setup_GL_Params();
if(gl_shaders) Setup_Shaders();
*ValidityFlag = true;
}

//�������� � ��������� ��������� ����� OpenGL
void AppGLContext::Setup_GL_Params()   {
glEnable(GL_COLOR_MATERIAL); //����
glEnable(GL_DEPTH_TEST); //���� ������� ����� �����������
glEnable(GL_LIGHTING); //����
glShadeModel(GL_SMOOTH);
glEnable(GL_LIGHT0); //�������� ����� ��� ������
glCullFace(GL_BACK); //��� ������ �������� � ������ �����
  //�����������
glEnable(GL_CULL_FACE);
glFrontFace(GL_CCW); //������ ������� �� �����������
  //���������� � ������� ���������� ������ ��� ����������
glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//����� ������ ����� ������ ��� �������������� ��������
glClearColor(0, 0, 0, 0); //���� �������
}

#include "phong.vert.h"
#include "phong.frag.h"

//��������� �������
void AppGLContext::Setup_Shaders()  {
const char * const phong_vv[] = {phong_vert};
const char * const phong_fv[] = {phong_frag};
GLuint phong_vert_shader, phong_frag_shader;
phong_vert_shader = glCreateShader(GL_VERTEX_SHADER);
phong_frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
glShaderSource(phong_vert_shader, 1,
  phong_vv, &phong_vert_len);
glShaderSource(phong_frag_shader, 1,
  phong_fv, &phong_frag_len);
glCompileShader(phong_vert_shader);
glCompileShader(phong_frag_shader);
phong_shader = glCreateProgram();
glAttachShader(phong_shader, phong_vert_shader);
glAttachShader(phong_shader, phong_frag_shader);
glLinkProgram(phong_shader);
glDeleteShader(phong_vert_shader);
glDeleteShader(phong_frag_shader);
glUseProgram(phong_shader);
}

//����������� ��������� � ��������� ���������� OpenGL
//(��������� ���������� ������� � ������ ���������)
void AppGLContext::Setup_GL_extensions()  {
//������� ���������� ARB_vertex_buffer_object (gl>=1.5)
gl_arb_buffers =
  ((glBindBuffer=(PFNGLBINDBUFFERPROC)
    wglGetProcAddress("glBindBuffer")) &&
  (glDeleteBuffers=(PFNGLDELETEQUERIESPROC)
    wglGetProcAddress("glDeleteBuffers")) &&
   (glGenBuffers=(PFNGLGENQUERIESPROC)
    wglGetProcAddress("glGenBuffers")) &&
   (glBufferData=(PFNGLBUFFERDATAPROC)
    wglGetProcAddress("glBufferData"))) ||
  ((glBindBuffer=(PFNGLBINDBUFFERPROC)
    wglGetProcAddress("glBindBufferARB")) &&
  (glDeleteBuffers=(PFNGLDELETEQUERIESPROC)
    wglGetProcAddress("glDeleteBuffersARB")) &&
   (glGenBuffers=(PFNGLGENQUERIESPROC)
    wglGetProcAddress("glGenBuffersARB")) &&
   (glBufferData=(PFNGLBUFFERDATAPROC)
    wglGetProcAddress("glBufferDataARB")));
//������� ���������� GL_vertex/fragment_shader (gl>=2.0)
gl_shaders =
  (glAttachShader = (PFNGLATTACHSHADERPROC)
    wglGetProcAddress("glAttachShader")) &&
  (glCompileShader = (PFNGLCOMPILESHADERPROC)
    wglGetProcAddress("glCompileShader")) &&
  (glCreateProgram = (PFNGLCREATEPROGRAMPROC)
    wglGetProcAddress("glCreateProgram")) &&
  (glCreateShader = (PFNGLCREATESHADERPROC)
    wglGetProcAddress("glCreateShader")) &&
  (glDeleteProgram = (PFNGLDELETEPROGRAMPROC)
    wglGetProcAddress("glDeleteProgram")) &&
  (glDeleteShader = (PFNGLDELETESHADERPROC)
    wglGetProcAddress("glDeleteShader")) &&
  (glLinkProgram = (PFNGLLINKPROGRAMPROC)
    wglGetProcAddress("glLinkProgram")) &&
  (glShaderSource = (PFNGLSHADERSOURCEPROC)
    wglGetProcAddress("glShaderSource")) &&
  (glUseProgram = (PFNGLUSEPROGRAMPROC)
    wglGetProcAddress("glUseProgram"));
}

//��������� ������������ ���� � ������� ��������
void AppGLContext::Setup_viewPort(int width, int height) {
wWidth = width;
wHeight = height;
//���� ���������
glViewport(0, 0, width, height);
glMatrixMode(GL_PROJECTION);
//������������� ��������, ����. ������ 50,
//������ ���������, ���� ������ 45 ��������
glLoadIdentity();
gluPerspective(yFov,
  double(width)/double(height), zNear, zFar);
//��-��������� �� ������ � ������ ������� ������
glMatrixMode(GL_MODELVIEW);
}

//���������� ������� ������ � ������� ������ �����
void AppGLContext::Setup_camera() {
double vsinx = sin(DEG2RAD*Cam_x_ang),
  vcosx = cos(DEG2RAD*Cam_x_ang),
  vsiny = sin(DEG2RAD*Cam_y_ang),
  vcosy = cos(DEG2RAD*Cam_y_ang);

//������ ������� ������ � ����������� �����������
Cam_x_cpos = Cam_distance*vcosx*vsiny+Cam_x_vpos;
Cam_y_cpos = Cam_distance*vsinx*vsiny+Cam_y_vpos;
Cam_z_cpos = Cam_distance*vcosy;

//��������� ������� ������, Z - ���������
//(� OpenGL - ������ ������ ������ ����)
glLoadIdentity();
gluLookAt(Cam_x_cpos, Cam_y_cpos, Cam_z_cpos,
  Cam_x_vpos, Cam_y_vpos, 0.0, 0.0, 0.0, 1.0);

//������ ������� ����� ��� 45 �������� �� 2 �����.
//�������� �������� ����� ��-���������
const float p[4]={
  (float)((-vcosx*vcosy-vsinx+vcosx*vsiny)*
    Cam_distance+Cam_x_vpos),
  (float)((-vsinx*vcosy+vcosx+vsinx*vsiny)*
    Cam_distance+Cam_y_vpos),
  (float)((vcosy+vsiny)*Cam_distance), 1.0f};
//����� ���������� �� ������� � �������� �������� �
//�������
if(gl_shaders)  {
  glPushMatrix();
  glLoadIdentity();
}
glLightfv(GL_LIGHT0,GL_POSITION,p);
if(gl_shaders)  {
  glPopMatrix();
}
//��������� ������� ������ ����� ������� �������� � ������
GLfloat mat[16];
glGetFloatv(GL_MODELVIEW_MATRIX, mat);
glm::dmat4x4 view_mat = glm::make_mat4(mat);
glGetFloatv(GL_PROJECTION_MATRIX, mat);
glm::dmat4x4 cmat = glm::make_mat4(mat);
cmat *= view_mat;
cmat = transpose(cmat);
frustum_planes[0] = (cmat[3] + cmat[0]);
frustum_planes[1] = (cmat[3] - cmat[0]);
frustum_planes[2] = (cmat[3] + cmat[1]);
frustum_planes[3] = (cmat[3] - cmat[1]);
frustum_planes[4] = (cmat[3] + cmat[2]);
frustum_planes[5] = (cmat[3] - cmat[2]);
for(int i = 0; i<6; ++i)    {
  frustum_planes[i] /=
    glm::length(glm::vec3(frustum_planes[i]));
}
}

AppGLContext::~AppGLContext()   {
*ValidityFlag = false;
//������ ����������� ������
ClearSatellite();
//������ �������
if(gl_shaders)  {
  glUseProgram(0);
  glDeleteProgram(phong_shader);
}
//����������� ��������� ���������
if(glRC)  {
  wglMakeCurrent(glDC,0);
  wglDeleteContext(glRC);
}
if(glDC)  {
  ReleaseDC(hwnd, glDC);
}
}

//��������� �� ������ � ���� ������
bool AppGLContext::FrustumCull(Figure3d_unit* fig) const {
glm::dvec4 pos(1.0);
double radius = fig->GetSphRadius();
fig->GetCenterPos((double*)&pos);
for(int i = 0; i<6; ++i)    {
  if(glm::dot(frustum_planes[i], pos)<-radius)
    return false;
}
return true;
}

//��������� ������ �����
void AppGLContext::DrawFrame() const {
//������� ��������� ���� � ��������� OpenGL
wglMakeCurrent(glDC, glRC);
//�������� ������� ����
glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
//�������� ��� ������, �� ��������� - ������ �����
//��������� - ������� �� ������
for(auto x: Figures) {
  if(!FrustumCull(x)) continue;
  if(x == fig_selection) switch(fig_mode)  {
  case F2CuttingFig:
    glColor3f(0, 1, 1); //������� - �������
    break;
  default:
    glColor3f(1, 1, 0); //���������� - �����
    break;
  }
  else glColor3f(1, 1, 1);
  x->Draw();
}
//�������� ������� ��������������� ������
glColor3f(1, 0, 0);
for(auto x: Figure_satellite) {
  if(!FrustumCull(x)) continue;
  x->Draw();
}
//������� ���� �� �����
SwapBuffers(glDC);
}

//���������� �������� �������
void AppGLContext::CtrlSimpleClick(int X, int Y)  {
Figure3d_unit *fig = nullptr;
double cam_arr[8], dst = 2.0*zFar, d;
glm::dvec3 cam, ray, pt(0), p;
//�������� ��� ������� ���� � ������������
getRaycast(
  2.0*((double)X/wWidth)-1.0,
  1.0-2.0*((double)Y/wHeight), ray);

//��� ������ ������ ��������� ������� ����������� ���
//������� ����� ������������ �����, ��� ������� - ����
//����� ������� ����������� � �������, ����������
//��������� ����� ����������� � � ������
GetCamPos(cam_arr);
cam = glm::make_vec3(cam_arr+3);
for(auto x: Figures)    {
  if(x->SphereIntersect(cam, ray) &&
    x->IntersectPoint(cam, ray, p, d) && d<dst)  {
    fig = x;
    dst = d;
    pt = p;
  }
}

switch(fig_mode)    {
case F2CuttingFig:
  //� ������ ������� ������ ������� ����� �������
  //���������
  if(fig == fig_selection)  {
    if(Figure_satellite.size()<2)   {
    //������ ��� ������� �������� ���������������
    //���������� � ������ �������
      auto ico = ArrayFig::NewIcosahedron(this);
      ico->Resize(0.1, 0.1, 0.1);
      ico->Move(pt[0], pt[1], pt[2]);
      Figure_satellite.push_back(ico);
    }
    else {
    //��� ����� ����������, ��������� �������
      glm::dvec3 norm, tri[3];
      glm::dvec4 plane;
      Figure_satellite[0]->GetCenterPos((double*)(tri));
      Figure_satellite[1]->GetCenterPos((double*)(tri+1));
      tri[2] = pt;
      //���������� ������� ���������
      getNormal(tri, norm);
      plane = {norm.x, norm.y, norm.z, -glm::dot(norm, pt)};
      //�������
      fig_selection->PerformCut(plane);
      //������ �����. ������
      ClearSatellite();
      fig_mode = F2Normal;
    }
  }
  break;
default:
  //� ������� ������ �������� ������ ������,
  //���� ������� ������ �� ��������, ������� �����
  fig_selection = fig;
  break;
}
}

//���������� ����������������� �������� ������
void AppGLContext::CtrlCamRotate(int dX, int dY)    {
Cam_x_ang += -1.0*dX;
while(Cam_x_ang<0.0)  {
  Cam_x_ang += 360.0;
}
while(Cam_x_ang>360.0) {
  Cam_x_ang -= 360.0;
}
Cam_y_ang += -1.0*dY;
if(Cam_y_ang<0.1)  {
  Cam_y_ang = 0.1;
}
else if(Cam_y_ang>179.9) {
  Cam_y_ang = 179.9;
}
}

//���������� ����������������� �������� ������
void AppGLContext::CtrlCamZoom(int dY)  {
Cam_distance += 0.01 * Cam_distance * dY;
}

//���������� ����������������� ����������� ������
void AppGLContext::CtrlCamMoveXoY(int dX, int dY)   {
double vsin = sin(DEG2RAD*Cam_x_ang),
  vcos = cos(DEG2RAD*Cam_x_ang);
Cam_x_vpos += Cam_distance*(-vcos*dY+vsin*dX)*0.75/wHeight;
Cam_y_vpos -= Cam_distance*(vsin*dY+vcos*dX)*0.75/wHeight;
}

//���������� ����� �������
bool AppGLContext::SetCuttingMode() {
if(GetSelectedFig() != nullptr) {
  //������ �����. ������ � ��������� � ����� �������
  ClearSatellite();
  fig_mode = F2CuttingFig;
  return true;
}
return false;
}

//�������� ��������
bool AppGLContext::CancelOperationMode()    {
if(fig_mode != F2Normal)    {
  ClearSatellite();
  fig_mode = F2Normal;
  return true;
}
return false;
}

//Extern ������ ��� ������������ ����������
#ifdef DLL
//��������� �������� ������� � ����������� �� ���������
#ifdef _WIN32
#define DLLSO __declspec(dllexport)
#else
#define DLLSO __attribute__((visibility("default")))
#endif

extern "C" {
DLLSO void* CreateAppGLContext(void *hwnd,
  int width, int height)    {
return new AppGLContext((HWND)hwnd, width, height);
}

DLLSO void DestroyAppGLContext(void *glContext)    {
delete ((AppGLContext*)glContext);
}

DLLSO void AppGLSetupViewPort(void *glContext,
  int width, int height)    {
((AppGLContext*)glContext)->Setup_viewPort(width, height);
}

DLLSO void AppGLSetupCamera(void *glContext)    {
((AppGLContext*)glContext)->Setup_camera();
}

DLLSO void AppGLDrawFrame(const void *glContext)    {
((AppGLContext*)glContext)->DrawFrame();
}

DLLSO void GetAppGLCamPos_8dv(const void *glContext,
  double *v)  {
((AppGLContext*)glContext)->GetCamPos(v);
}

DLLSO double GetAppGLCamDst(const void *glContext)    {
return ((AppGLContext*)glContext)->GetCamDst();
}

DLLSO void SetAppGLViewPos_dxy(void *glContext,
  double x, double y)    {
((AppGLContext*)glContext)->SetViewPos(x, y);
}

DLLSO void SetAppGLCamAng_dxy(
  void *glContext, double x, double y)    {
((AppGLContext*)glContext)->SetCamAng(x, y);
}

DLLSO void SetAppGLCamDst_d(void *glContext, double d)    {
((AppGLContext*)glContext)->SetCamDst(d);
}

DLLSO void AppGLAddFigure(void *glContext, void *fig)    {
((AppGLContext*)glContext)->AddFigure((Figure3d_unit*)fig);
}

DLLSO void AppGLRemoveFigure(void *glContext,
  const void *fig)   {
((AppGLContext*)glContext)->RemoveFigure(
  (Figure3d_unit*)fig);
}

DLLSO void* GetAppGLSelectedFigure(const void *glContext) {
return ((AppGLContext*)glContext)->GetSelectedFig();
}

DLLSO void SetAppGLSelectedFigure(void *glContext,
  void *fig)  {
((AppGLContext*)glContext)->SetSelectedFig(
  (Figure3d_unit*)fig);
}

DLLSO void AppGLCtrlSimpleClick(void *glContext,
  int X, int Y) {
((AppGLContext*)glContext)->CtrlSimpleClick(X, Y);
}

DLLSO void AppGLCtrlCamRotate(
  void *glContext, int dX, int dY)  {
((AppGLContext*)glContext)->CtrlCamRotate(dX, dY);
}

DLLSO void AppGLCtrlCamZoom(void *glContext, int dY)  {
((AppGLContext*)glContext)->CtrlCamZoom(dY);
}

DLLSO void AppGLCtrlCamMoveXoY(void *glContext,
  int dX, int dY) {
((AppGLContext*)glContext)->CtrlCamMoveXoY(dX, dY);
}

DLLSO bool SetAppGLCuttingMode(void *glContext) {
return ((AppGLContext*)glContext)->SetCuttingMode();
}

DLLSO bool CancelAppGLOperationMode(void *glContext)  {
return ((AppGLContext*)glContext)->CancelOperationMode();
}

DLLSO int GetAppGLFigMode(const void *glContext)  {
return (int)(((AppGLContext*)glContext)->GetFigMode());
}
}
#endif

