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

//Настроить графический формат для Handle-а гр. окна
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

//Записать позицию точки обзора (3), угол обзора (2)
//и позицию камеры (3) в массив размера 8
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

//Удалить вспомогательные фигуры
void AppGLContext::ClearSatellite()   {
for(auto x: Figure_satellite)   {
  delete x;
}
Figure_satellite.clear();
}

//Добавить фигуру в пространство
void AppGLContext::AddFigure(Figure3d_unit *fig)    {
//Перемещение фигуры на точку обзора
fig->Move(Cam_x_vpos, Cam_y_vpos, 0.0);
//Регистрация фигуры в пространстве
Figures.push_back(fig);
}

//Удалить фигуру из пространства
void AppGLContext::RemoveFigure(const Figure3d_unit* fig) {
auto it = std::remove(Figures.begin(), Figures.end(), fig);
Figures.erase(it, Figures.end());
if(fig == fig_selection) fig_selection = nullptr;
}

//Создать и выбрать контексты окна и отрисовки OpenGL
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
//Настроить матрицы проекции и обзора
Setup_viewPort(width, height);
Setup_camera();
Setup_GL_Params();
if(gl_shaders) Setup_Shaders();
*ValidityFlag = true;
}

//Включить и настроить параметры опций OpenGL
void AppGLContext::Setup_GL_Params()   {
glEnable(GL_COLOR_MATERIAL); //Цвет
glEnable(GL_DEPTH_TEST); //Тест глубины точек изображения
glEnable(GL_LIGHTING); //Свет
glShadeModel(GL_SMOOTH);
glEnable(GL_LIGHT0); //Источник света для камеры
glCullFace(GL_BACK); //Все фигуры замкнуты и задние грани
  //отсеиваются
glEnable(GL_CULL_FACE);
glFrontFace(GL_CCW); //Против часовой от наблюдателя
  //Соглашение о порядке следования вершин для отсеивания
glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//Режим работы альфа канала для полупрозрачных объектов
glClearColor(0, 0, 0, 0); //Цвет пустоты
}

#include "phong.vert.h"
#include "phong.frag.h"

//Настроить шейдеры
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

//Определение поддержки и настройка расширений OpenGL
//(Настройка указателей функций и флагов поддержки)
void AppGLContext::Setup_GL_extensions()  {
//Функции расширения ARB_vertex_buffer_object (gl>=1.5)
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
//Функции расширения GL_vertex/fragment_shader (gl>=2.0)
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

//Настройка графического окна и матрицы проекции
void AppGLContext::Setup_viewPort(int width, int height) {
wWidth = width;
wHeight = height;
//Окно отрисовки
glViewport(0, 0, width, height);
glMatrixMode(GL_PROJECTION);
//Перспективная проекция, дист. обзора 50,
//камера маленькая, поле зрения 45 градусов
glLoadIdentity();
gluPerspective(yFov,
  double(width)/double(height), zNear, zFar);
//По-умолчанию мы всегда в режиме матрицы обзора
glMatrixMode(GL_MODELVIEW);
}

//Обновление позиции камеры и матрицы обзора фигур
void AppGLContext::Setup_camera() {
double vsinx = sin(DEG2RAD*Cam_x_ang),
  vcosx = cos(DEG2RAD*Cam_x_ang),
  vsiny = sin(DEG2RAD*Cam_y_ang),
  vcosy = cos(DEG2RAD*Cam_y_ang);

//Расчёт позиции камеры в сферических координатах
Cam_x_cpos = Cam_distance*vcosx*vsiny+Cam_x_vpos;
Cam_y_cpos = Cam_distance*vsinx*vsiny+Cam_y_vpos;
Cam_z_cpos = Cam_distance*vcosy;

//Настройка матрицы обзора, Z - вертикаль
//(В OpenGL - всегда правая тройка осей)
glLoadIdentity();
gluLookAt(Cam_x_cpos, Cam_y_cpos, Cam_z_cpos,
  Cam_x_vpos, Cam_y_vpos, 0.0, 0.0, 0.0, 1.0);

//Расчёт позиции света под 45 градусов по 2 углам.
//Точечный источник света по-умолчанию
const float p[4]={
  (float)((-vcosx*vcosy-vsinx+vcosx*vsiny)*
    Cam_distance+Cam_x_vpos),
  (float)((-vsinx*vcosy+vcosx+vsinx*vsiny)*
    Cam_distance+Cam_y_vpos),
  (float)((vcosy+vsiny)*Cam_distance), 1.0f};
//Чтобы избавиться от расчёта с обратной матрицей в
//шейдере
if(gl_shaders)  {
  glPushMatrix();
  glLoadIdentity();
}
glLightfv(GL_LIGHT0,GL_POSITION,p);
if(gl_shaders)  {
  glPopMatrix();
}
//Плоскости границы обзора через матрицы проекции и обзора
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
//Чистим собственные фигуры
ClearSatellite();
//Чистим шейдеры
if(gl_shaders)  {
  glUseProgram(0);
  glDeleteProgram(phong_shader);
}
//Освобождаем контексты отрисовки
if(glRC)  {
  wglMakeCurrent(glDC,0);
  wglDeleteContext(glRC);
}
if(glDC)  {
  ReleaseDC(hwnd, glDC);
}
}

//Находится ли фигура в поле зрения
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

//Отрисовка нового кадра
void AppGLContext::DrawFrame() const {
//Выбрать контексты окна и отрисовки OpenGL
wglMakeCurrent(glDC, glRC);
//Очистить будущий кадр
glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
//Рисовать все фигуры, не выбранные - белого цвета
//Остальные - зависит от режима
for(auto x: Figures) {
  if(!FrustumCull(x)) continue;
  if(x == fig_selection) switch(fig_mode)  {
  case F2CuttingFig:
    glColor3f(0, 1, 1); //Сечение - голубой
    break;
  default:
    glColor3f(1, 1, 0); //Нормальный - жёлтый
    break;
  }
  else glColor3f(1, 1, 1);
  x->Draw();
}
//Рисовать красные вспомогательные фигуры
glColor3f(1, 0, 0);
for(auto x: Figure_satellite) {
  if(!FrustumCull(x)) continue;
  x->Draw();
}
//Выводим кадр на экран
SwapBuffers(glDC);
}

//Обработчик простого нажатия
void AppGLContext::CtrlSimpleClick(int X, int Y)  {
Figure3d_unit *fig = nullptr;
double cam_arr[8], dst = 2.0*zFar, d;
glm::dvec3 cam, ray, pt(0), p;
//Посылаем луч нажатия мыши в пространство
getRaycast(
  2.0*((double)X/wWidth)-1.0,
  1.0-2.0*((double)Y/wHeight), ray);

//Для каждой фигуры проверяем наличие пересечений или
//касаний лучом охватывающей сферы, при наличии - ищем
//точку точного пересечения с гранями, запоминаем
//ближайшую точку пересечения и её фигуру
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
  //В режиме сечения ставим опорные точки секущей
  //плоскости
  if(fig == fig_selection)  {
    if(Figure_satellite.size()<2)   {
    //Первые две вершины отмечаем вспомогательным
    //икосаэдром в точках нажатий
      auto ico = ArrayFig::NewIcosahedron(this);
      ico->Resize(0.1, 0.1, 0.1);
      ico->Move(pt[0], pt[1], pt[2]);
      Figure_satellite.push_back(ico);
    }
    else {
    //Три точки достаточно, выполняем сечение
      glm::dvec3 norm, tri[3];
      glm::dvec4 plane;
      Figure_satellite[0]->GetCenterPos((double*)(tri));
      Figure_satellite[1]->GetCenterPos((double*)(tri+1));
      tri[2] = pt;
      //Вычисление секущей плоскости
      getNormal(tri, norm);
      plane = {norm.x, norm.y, norm.z, -glm::dot(norm, pt)};
      //Сечение
      fig_selection->PerformCut(plane);
      //Чистим вспом. фигуры
      ClearSatellite();
      fig_mode = F2Normal;
    }
  }
  break;
default:
  //В обычном режиме выбираем фигуру кликом,
  //Если никакая фигура не кликнута, снимаем выбор
  fig_selection = fig;
  break;
}
}

//Обработчик пользовательского поворота камеры
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

//Обработчик пользовательского масштаба камеры
void AppGLContext::CtrlCamZoom(int dY)  {
Cam_distance += 0.01 * Cam_distance * dY;
}

//Обработчик пользовательского перемещения камеры
void AppGLContext::CtrlCamMoveXoY(int dX, int dY)   {
double vsin = sin(DEG2RAD*Cam_x_ang),
  vcos = cos(DEG2RAD*Cam_x_ang);
Cam_x_vpos += Cam_distance*(-vcos*dY+vsin*dX)*0.75/wHeight;
Cam_y_vpos -= Cam_distance*(vsin*dY+vcos*dX)*0.75/wHeight;
}

//Установить режим сечения
bool AppGLContext::SetCuttingMode() {
if(GetSelectedFig() != nullptr) {
  //Чистим вспом. фигуры и переходим в режим сечения
  ClearSatellite();
  fig_mode = F2CuttingFig;
  return true;
}
return false;
}

//Отменить операцию
bool AppGLContext::CancelOperationMode()    {
if(fig_mode != F2Normal)    {
  ClearSatellite();
  fig_mode = F2Normal;
  return true;
}
return false;
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

