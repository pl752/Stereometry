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

#include "figure3d_unit.h"
#include <Windows.h>
#include <GL/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <algorithm>
#include <map>
#include <deque>
#include <cmath>
#include <stdexcept>
#include "Raycast.h"
//----------------------------------------------------------
#pragma package(smart_init)

using namespace std;

//��������� ������� ������������ ������� ������
//dest_ind - ����� � ������ ��������
//n*(k-2) �������������
//src_ind - ������ k ��������� ������
//point_num - ���-�� ������ ��������� ������ (k)
//face_num - ���-�� ��������� ������ (n)
void _Simple_triang_fan(GLuint *dest_ind,
  const GLuint *src_ind, GLuint point_num,
  GLuint face_num)  {
size_t ind = 0;
for(GLuint i = 0; i<face_num; ++i) {
    //������� ������ �� ������������ � ����� ������ ��������
  for(GLuint j = 2; j<point_num; ++j) {
    dest_ind[ind++] = src_ind[point_num*i];
    dest_ind[ind++] = src_ind[point_num*i+j-1];
    dest_ind[ind++] = src_ind[point_num*i+j];
  }
}
}

//����������� � ��������� � ���������
Figure3d_unit::Figure3d_unit(const
  std::weak_ptr<const AppGLContext>& glContext) :
  glContext(glContext), Scale(1.0), Scale_old(1.0),
  Offset(0.0), Offset_old(0.0)  {
auto ctx = glContext.lock();
if(ctx && ctx->HasVertexBufferObject())  {
  draw_arr.clear();
  draw_arr.shrink_to_fit();
  ctx->GenBuffers(1, &_vbo);
}
}

//�������� �������� ���������
void Figure3d_unit::ReplaceContext(const
  std::weak_ptr<const AppGLContext>& other)    {
auto ctx = glContext.lock();
auto ctx_o = other.lock();
if(ctx == ctx_o) return;
if(ctx && ctx->HasVertexBufferObject())  {
  ctx->DeleteBuffers(1, &_vbo);
  _vbo = 0;
}
ctx.reset();
glContext = other;
if(ctx_o && ctx_o->HasVertexBufferObject())  {
  ctx_o->GenBuffers(1, &_vbo);
}
_draw_buf_upd_flag = true;
}

//�����������
Figure3d_unit::Figure3d_unit(const Figure3d_unit &other) :
  sphere_radius(other.sphere_radius),
  glContext(other.glContext),
  vertices_arr(other.vertices_arr),
  vert_normals_arr(other.vert_normals_arr),
  face_normals_arr(other.face_normals_arr),
  indices_arr(other.indices_arr),
  flat_face_arr(other.flat_face_arr),
  no_vec_normals(other.no_vec_normals),
  _n_d_face(other._n_d_face),
  _n_d_vertex(other._n_d_vertex),
  Scale(other.Scale), Scale_old(other.Scale_old),
  Offset(other.Offset), Offset_old(other.Offset_old)  {
auto ctx = glContext.lock();
if(ctx && ctx->HasVertexBufferObject())  {
  draw_arr.clear();
  draw_arr.shrink_to_fit();
  ctx->GenBuffers(1, &_vbo);
}
}

//�����������
Figure3d_unit& Figure3d_unit::operator=(
  const Figure3d_unit &other)   {
if(this == &other) return *this;
sphere_radius = other.sphere_radius;
Scale = other.Scale;
Scale_old = other.Scale_old;
Offset = other.Offset;
Offset_old = other.Offset_old;
vertices_arr = other.vertices_arr;
vert_normals_arr = other.vert_normals_arr;
face_normals_arr = other.face_normals_arr;
indices_arr = other.indices_arr;
flat_face_arr = other.flat_face_arr;
no_vec_normals = other.no_vec_normals;
_n_d_face = other._n_d_face;
_n_d_vertex = other._n_d_vertex;
ReplaceContext(other.glContext);
_draw_buf_upd_flag = true;
return *this;
}

Figure3d_unit::~Figure3d_unit() {
auto ctx = glContext.lock();
if(ctx && ctx->HasVertexBufferObject())  {
  ctx->DeleteBuffers(1, &_vbo);
}
}

//����������� ���������� draw_arr (gl<1.5) ��� vbo (gl>=1.5)
void Figure3d_unit::_PrepareBuffers() const   {
auto ctx = glContext.lock();
if(!ctx) throw std::runtime_error(
    "glContext is invalid/deleted/not ready");

//TODO: �������� ������� ���������� �� �����������
const int data_size = 18*_n_d_face;
const bool nfn_flag = no_vec_normals;
size_t ind = 0;
auto fface = flat_face_arr.cbegin(),
  fface_e = flat_face_arr.cend();

draw_arr.resize(data_size);
for(GLuint i = 0; i<_n_d_face; ++i)    {
//�� 3 �������� � ������� ����� �� 3 �����
//��� ������� ������������
  for(int j = 0; j<3; ++j)    {
    while(!nfn_flag && fface != fface_e && (*fface)<i) {
      ++fface;
    }
    if(nfn_flag || (fface != fface_e && (*fface)==i))  {
      for(int k = 0; k<3; ++k)    {
        draw_arr[ind++] = face_normals_arr[3*i+k];
      }
    }
    else {
      for(int k = 0; k<3; ++k)    {
        draw_arr[ind++] =
          vert_normals_arr[3*indices_arr[3*i+j]+k];
      }
    }
    for(int k = 0; k<3; ++k)    {
      draw_arr[ind++] =
        vertices_arr[3*indices_arr[3*i+j]+k];
    }
  }
}

if(ctx->HasVertexBufferObject())  {
  //����������� ������ � vbo
  ctx->BindBuffer(GL_ARRAY_BUFFER, _vbo);
  ctx->BufferData(GL_ARRAY_BUFFER,
    data_size*sizeof(GLfloat),
    draw_arr.data(), GL_STATIC_DRAW);

  //��������� ������ ���������� �����������
  bool out_of_memory = false;
  GLenum err = GL_NO_ERROR;
  while((err = glGetError()) != GL_NO_ERROR) {
    switch (err)    {
    case GL_OUT_OF_MEMORY:
      out_of_memory = true;
      break;
    default:
      //TODO: ������� ���������� ���������� ������
      break;
    }
  }

  ctx->BindBuffer(GL_ARRAY_BUFFER, 0);
  if(out_of_memory)   {
    ctx->DeleteBuffers(1, &_vbo);
    _vbo = 0;
  }
  else    {
    draw_arr.clear();
        draw_arr.shrink_to_fit();
  }
}
}

//��������� ��������� ������
void Figure3d_unit::Draw() const    {
auto ctx = glContext.lock();
if(!ctx) throw std::runtime_error(
    "glContext is invalid/deleted/not ready");

//�������� draw_array, ���� ���� �������������
if(_draw_buf_upd_flag)  {
  _PrepareBuffers();
  _draw_buf_upd_flag = false;
}

//��������� ������ � draw_arr ��� vbo
if(ctx->HasVertexBufferObject())  {
  ctx->BindBuffer(GL_ARRAY_BUFFER, _vbo);
}
glInterleavedArrays(GL_N3F_V3F, 0,
  _vbo ? 0 : draw_arr.data());

//���� ���������
glDrawArrays(GL_TRIANGLES, 0, _n_d_face*3);

//������� ��������� ��-���������
glDisableClientState(GL_VERTEX_ARRAY);
glDisableClientState(GL_NORMAL_ARRAY);
if(ctx->HasVertexBufferObject())  {
  ctx->BindBuffer(GL_ARRAY_BUFFER, 0);
}
}

//����������� ���������� ������ ����� ���������������
void Figure3d_unit::_Calc_Scale()   {
glm::dvec3 sc, p;
//���������� ���������� ��������
sc = Scale/Scale_old;
//��������������� ������� ������ ���. ������
size_t ind = 0;
for(GLuint i = 0; i<_n_d_vertex; ++i)    {
  for(GLuint j = 0; j<3; ++j)    {
    vertices_arr[ind] =
      (vertices_arr[ind]-Offset[j])*sc[j]+Offset[j];
        ind++;
  }
}
if(!no_vec_normals) {
  ind = 0;
  for(GLuint i = 0; i<_n_d_vertex; ++i)    {
    p = glm::make_vec3(vert_normals_arr.data()+ind)/sc;
    double norm = glm::length(p);
    if(norm > 1e-9)   {
      p /= norm;
    }
    else {
      p = {0,0,1};
    }
    memcpy(vert_normals_arr.data()+ind, &p,
      3*sizeof(double));
    ind += 3;
  }
}
ind = 0;
for(GLuint i = 0; i<_n_d_face; ++i)    {
  p = glm::make_vec3(face_normals_arr.data()+ind)/sc;
  double norm = glm::length(p);
  if(norm > 1e-9)   {
    p /= norm;
  }
  else {
    p = {0,0,1};
  }
  memcpy(face_normals_arr.data()+ind, &p,
    3*sizeof(double));
  ind += 3;
}
//��������������� ������������ ����������
sphere_radius *= max(sc.x, max(sc.y, sc.z));
}

//����������� ���������� ������ ����� ��������
void Figure3d_unit::_Calc_Offset()  {
glm::dvec3 off;
//���������� �������������� ��������
off = Offset-Offset_old;
//�������� ������
size_t ind = 0;
for(GLuint i = 0; i<_n_d_vertex; ++i)    {
  for(GLuint j = 0; j<3; ++j)    {
    vertices_arr[ind++] += off[j];
  }
}
}

//���������� ������ �������� ������
void Figure3d_unit::_Calc_Tri_Normals() {
//��������� ������ ��� �������
face_normals_arr.resize(3*_n_d_face);
glm::dvec3 tri[3], norm;
for(GLuint k = 0; k<_n_d_face; ++k)    {
//����� ������ ��� ������ ������������
  for(int i = 0; i<3; ++i)    {
    tri[i] = glm::make_vec3(
      vertices_arr.data()+3*indices_arr[3*k+i]);
  }
  //���������� �������
  getNormal(tri, norm);
  memcpy(face_normals_arr.data()+3*k, &norm,
    3*sizeof(double));
}
}

//���������� ������ ������������ �����
void Figure3d_unit::_Calc_Bound_Sphere()    {
double dst = 0.0;
GLuint ind = 0;
for(size_t i = 0; i<_n_d_vertex; ++i)  {
  double d = 0.0;
  for(int j = 0; j<3; ++j)    {
    double v = vertices_arr[ind++];
    d += v*v;
  }
  if(d>dst) dst = d;
}
sphere_radius = std::sqrt(dst);
}

//��������� ������ (������ XYZ) �������
void Figure3d_unit::Rotate(double x, double y, double z)   {
glm::dvec3 p, sin_v, cos_v, Rotation(x,y,z);
glm::dmat3 m;
//���������� ������� � ��������� ����� ��������
sin_v = sin(Rotation);
cos_v = cos(Rotation);
//������� �������� ������ XYZ
m[0][0] = cos_v[2]*cos_v[1];
m[1][0] = cos_v[2]*sin_v[1]*sin_v[0]-sin_v[2]*cos_v[0];
m[2][0] = cos_v[2]*sin_v[1]*cos_v[0]+sin_v[2]*sin_v[0];
m[0][1] = sin_v[2]*cos_v[1];
m[1][1] = sin_v[2]*sin_v[1]*sin_v[0]+cos_v[2]*cos_v[0];
m[2][1] = sin_v[2]*sin_v[1]*cos_v[0]-cos_v[2]*sin_v[0];
m[0][2] = -sin_v[1];
m[1][2] = cos_v[1]*sin_v[0];
m[2][2] = cos_v[1]*cos_v[0];
//������� ������ ������ ������
for(GLuint k = 0; k<_n_d_vertex; ++k)    {
  p = m*(glm::make_vec3(vertices_arr.data()+3*k)-Offset)+
    Offset;
  memcpy(vertices_arr.data()+3*k, &p, 3*sizeof(double));
}
//������� �������� ������ ������
if(!no_vec_normals) {
  for(GLuint k = 0; k<_n_d_vertex; ++k)    {
    p = m*glm::make_vec3(vert_normals_arr.data()+3*k);
    memcpy(vert_normals_arr.data()+3*k, &p,
      3*sizeof(double));
  }
}
for(GLuint k = 0; k<_n_d_face; ++k)    {
  p = m*glm::make_vec3(face_normals_arr.data()+3*k);
  memcpy(face_normals_arr.data()+3*k, &p,
    3*sizeof(double));
}
Scale_old = Scale = abs(m*Scale);
//�������� draw_arr ��� ���������
_draw_buf_upd_flag = true;
}

//������ ������� ������ ������
void Figure3d_unit::Move(double x, double y, double z) {
Offset = {x, y, z};
_Calc_Offset();
//������� ������ �������
Offset_old = Offset;
//�������� draw_arr ��� ���������
_draw_buf_upd_flag = true;
}

//������ ������� ������
void Figure3d_unit::Resize(double x, double y, double z)  {
Scale = {x, y, z};
_Calc_Scale();
//������� ������ �������
Scale_old = Scale;
//�������� draw_arr ��� ���������
_draw_buf_upd_flag = true;
}

//�������� ������� ������ ������ � ������
void Figure3d_unit::GetCenterPos(double *v) const {
memcpy(v, &Offset, 3*sizeof(double));
}

//�������� ������� ������ � ������
void Figure3d_unit::GetScale(double *v) const {
memcpy(v, &Scale, 3*sizeof(double));
}

//������� ��������� ������� � ������������
//������������� ������� �������
void Figure3d_unit::MakePermanentlyFlat()   {
no_vec_normals = true;
vert_normals_arr.clear();
vert_normals_arr.shrink_to_fit();
flat_face_arr.clear();
flat_face_arr.shrink_to_fit();
}

//��������� ����������� � ������������ ������
//cam - ������� ������ (����� �������)
//ray - ����������� ����
//(������� ������� 3)
//���������� true ��� ������� �����������
bool Figure3d_unit::SphereIntersect(const glm::dvec3 &cam,
  const glm::dvec3 &ray) const  {
//b^2-c ?= 0 ��� ������� ����������� �� ������
return (intersectSphere(cam, ray, Offset) +
  sphere_radius*sphere_radius) >= 0.0;
}

//�������� ������ ����������� � �������
//cam - ������� ������ (����� �������)
//ray - ����������� ����
//norm - ������� ���������
//plane_pt - ����� �� ��������� ����� (�����)
//out - ����� ��������� ����� �������
//����������� ��� ������
//(������� ������� 3)
//dst - ����� ���������� �� ������ �� ����� �����������
//���������� true ��� ������
bool Figure3d_unit::IntersectPoint(const glm::dvec3 &cam,
  const glm::dvec3 &ray, glm::dvec3 &out, double &dst) const
  {
double ds = INFINITY, d;
glm::dvec3 pt, ipoint;
bool flag = false;
for(GLuint i = 0; i<_n_d_face; ++i)    {
//���� � �������� ��������� ����������� � �������������
  if(planeIntersectPt(cam, ray,
      glm::make_vec3(face_normals_arr.data()+3*i),
      glm::make_vec3(
        vertices_arr.data()+3*indices_arr[3*i]),
      ipoint, d)
    && d<ds)   {
    //���� ����� ������� ����������� � ����������
    glm::dvec3 tri[3];
    for(int j = 0; j<3; ++j)    {
      tri[j] = glm::make_vec3(
        vertices_arr.data()+3*indices_arr[3*i+j]);
    }
    //����� ����� � ������������ �����
    if(pointInTriangle(ipoint, tri))  {
      pt = ipoint;
      ds = d;
      flag = true;
    }
  }
}
if(flag)  {
//��������� ����� ���� ������� �������
  out = pt;
    dst = ds;
}
return flag;
}

//��������� ������� ������ ����������
//plane - ����. ��������� ������� ��������� (������ 4)
void Figure3d_unit::PerformCut(const glm::dvec4 &plane) {
map<GLuint, GLuint> front_verts; //����� ����� ����������
//��� ��� ��������� ����� ����������� ���� � ����������
map<pair<GLuint, GLuint>, GLuint> created_verts;
map<GLuint, GLuint> verts_new_pairs; //��� �� ��� �������
vector<double> verts_new; //���������� ������ ������
vector<GLuint> inds_new; //���������� ������ ������
vector<double> v_norms_new; //���������� ������ ��������
vector<double> f_norms_new; //���������� ������ ��������
vector<GLuint> flats_new; //���������� ������ �������
deque<GLuint> chain; //������� ����� �����
GLuint v_cnt = 0, f_cnt = 0; //����� ���-�� ������ � ������
GLuint k;
const glm::dvec3 norm = plane;
auto fface = flat_face_arr.cbegin(),
  fface_e = flat_face_arr.cend();
for(k = 0; k<_n_d_vertex; ++k)    {
    //���������� �� ������ ������ ����������
  if(-glm::dot(norm,
    glm::make_vec3(vertices_arr.data()+3*k))-plane.w
    >= 0.0) {
    for(int i = 0; i<3; ++i)    {
      verts_new.push_back(vertices_arr[3*k+i]);
    }
    if(!no_vec_normals) {
      for(int i = 0; i<3; ++i)    {
        v_norms_new.push_back(
          vert_normals_arr[3*k+i]);
      }
    }
    front_verts[k] = v_cnt++;
  }
}

//�������� ��� ��������� ������������ ����� ����������
auto _CutInner12 = [&](GLuint a, GLuint b, GLuint c) {
  GLuint ind0, ind1, ind2;
  glm::dvec3 point;
  ind0 = front_verts.find(a)->second;
  //���� ��� ��������� �������
  auto it = created_verts.find(make_pair(a, b)),
    end = created_verts.end();
  if(it != end ||
    (it = created_verts.find(make_pair(b, a))) != end) {
    ind1 = it->second;
  }
  else {
  //���� ��� - ��������� ����� �������
    glm::dvec3 va, vb, out, point, na, nb;
    va = glm::make_vec3(vertices_arr.data()+3*a);
    vb = glm::make_vec3(vertices_arr.data()+3*b);

    planeIntersectPtAlt(va, vb, plane, point);
    for(int i = 0; i<3; ++i)    {
      verts_new.push_back(point[i]);
    }
    if(!no_vec_normals) {
      na = glm::make_vec3(vert_normals_arr.data()+3*a);
      nb = glm::make_vec3(vert_normals_arr.data()+3*b);
      normalLerp(va, point, vb, na, nb, out);
      for(int i = 0; i<3; ++i)    {
        v_norms_new.push_back(out[i]);
      }
    }
    ind1 = v_cnt++;
    created_verts[make_pair(a, b)] = ind1;
  }
  //���������� ��� ������
  it = created_verts.find(make_pair(a, c));
  end = created_verts.end();
  if(it != end ||
    (it = created_verts.find(make_pair(c, a))) != end) {
    ind2 = it->second;
  }
  else {
    glm::dvec3 va, vb, out, point, na, nb;
    va = glm::make_vec3(vertices_arr.data()+3*a);
    vb = glm::make_vec3(vertices_arr.data()+3*c);
    planeIntersectPtAlt(va, vb, plane, point);
    for(int i = 0; i<3; ++i)    {
      verts_new.push_back(point[i]);
    }
    if(!no_vec_normals) {
      na = glm::make_vec3(vert_normals_arr.data()+3*a);
      nb = glm::make_vec3(vert_normals_arr.data()+3*c);
      normalLerp(va, point, vb, na, nb, out);
      for(int i = 0; i<3; ++i)    {
        v_norms_new.push_back(out[i]);
      }
    }
    ind2 = v_cnt++;
    created_verts[make_pair(a, c)] = ind2;
  }
  //������ ����� �����������
  inds_new.push_back(ind0);
  inds_new.push_back(ind1);
  inds_new.push_back(ind2);
    if(!no_vec_normals && fface != fface_e &&
    (*fface)==k)  {
    flats_new.push_back(f_cnt);
  }
  f_cnt++;
    //���������� ����� ����� ��� ���������� ����� �����
  verts_new_pairs[ind2] = ind1;
};

//�������� ��� ��������� ��������������� ����� ����������
auto _CutInner21 = [&](GLuint a, GLuint b, GLuint c) {
  GLuint ind0, ind1, ind2, ind3;
  glm::dvec3 point;
  ind0 = front_verts.find(a)->second;
  ind1 = front_verts.find(b)->second;
  //���� ��� ��������� �������
  auto it = created_verts.find(make_pair(a, c)),
    end = created_verts.end();
  if(it != end ||
    (it = created_verts.find(make_pair(c, a))) != end) {
    ind2 = it->second;
  }
  else {
  //���� ��� - ��������� ����� �������
    glm::dvec3 va, vb, out, point, na, nb;
    va = glm::make_vec3(vertices_arr.data()+3*a);
    vb = glm::make_vec3(vertices_arr.data()+3*c);
    planeIntersectPtAlt(va, vb, plane, point);
    for(int i = 0; i<3; ++i)    {
      verts_new.push_back(point[i]);
    }
    if(!no_vec_normals) {
      na = glm::make_vec3(vert_normals_arr.data()+3*a);
      nb = glm::make_vec3(vert_normals_arr.data()+3*c);
      normalLerp(va, point, vb, na, nb, out);
      for(int i = 0; i<3; ++i)    {
        v_norms_new.push_back(out[i]);
      }
    }
    ind2 = v_cnt++;
    created_verts[make_pair(a, c)] = ind2;
  }
  //���������� ��� ������
  it = created_verts.find(make_pair(b, c));
  end = created_verts.end();
  if(it != end ||
    (it = created_verts.find(make_pair(c, b))) != end) {
    ind3 = it->second;
  }
  else {
    glm::dvec3 va, vb, out, point, na, nb;
    va = glm::make_vec3(vertices_arr.data()+3*b);
    vb = glm::make_vec3(vertices_arr.data()+3*c);
    planeIntersectPtAlt(va, vb, plane, point);;
    for(int i = 0; i<3; ++i)    {
      verts_new.push_back(point[i]);
    }
    if(!no_vec_normals) {
      na = glm::make_vec3(vert_normals_arr.data()+3*b);
      nb = glm::make_vec3(vert_normals_arr.data()+3*c);
      normalLerp(va, point, vb, na, nb, out);
      for(int i = 0; i<3; ++i)    {
        v_norms_new.push_back(out[i]);
      }
    }
    ind3 = v_cnt++;
    created_verts[make_pair(b, c)] = ind3;
  }
  //������ ��� ����� ������������
  inds_new.push_back(ind0);
  inds_new.push_back(ind1);
  inds_new.push_back(ind3);
  if(!no_vec_normals && fface != fface_e &&
    (*fface)==k)  {
    flats_new.push_back(f_cnt);
  }
  f_cnt++;
  inds_new.push_back(ind3);
  inds_new.push_back(ind2);
  inds_new.push_back(ind0);
  if(!no_vec_normals && fface != fface_e &&
    (*fface)==k)  {
    flats_new.push_back(f_cnt);
  }
  f_cnt++;
    //���������� ����� ����� ��� ���������� ����� �����
  verts_new_pairs[ind2] = ind3;
};

//������ ������� ��� �������� ������� ������
//��� ��� ����� �����. - �������� ����� ������� ������
//��� ��� �� �����. - ������ �� �������� (����������� �����)
//����� �������� ����� �����
auto end = front_verts.end();
GLuint sa, sb, sc, fn_adds = 0;
for(k = 0; k<_n_d_face; ++k)    {
  while(!no_vec_normals && fface != fface_e && (*fface)<k) {
    ++fface;
  }
  auto it = front_verts.find(indices_arr[3*k]);
  if(it != end)   {
    sa = it->first;
    it = front_verts.find(indices_arr[3*k+1]);
    if(it != end)   {
      sb = it->first;
      it = front_verts.find(indices_arr[3*k+2]);
      if(it != end)   {
        sa = front_verts.find(indices_arr[3*k])
          ->second;
        sb = front_verts.find(indices_arr[3*k+1])
          ->second;
        sc = it->second;
        if(!no_vec_normals && fface != fface_e &&
          (*fface)==k)  {
          flats_new.push_back(f_cnt);
        }
        inds_new.push_back(sa);
        inds_new.push_back(sb);
        inds_new.push_back(sc);
        fn_adds = 1;
        f_cnt++;
      }
      else    {
        _CutInner21(sa,sb,indices_arr[3*k+2]);
        fn_adds = 2;
      }
    }
    else    {
      sb = indices_arr[3*k+1];
      it = front_verts.find(indices_arr[3*k+2]);
      if(it != end)   {
        _CutInner21(it->first,sa,sb);
        fn_adds = 2;
      }
      else    {
        _CutInner12(sa,sb,indices_arr[3*k+2]);
        fn_adds = 1;
      }
    }
  }
  else {
    sa = indices_arr[3*k];
    it = front_verts.find(indices_arr[3*k+1]);
    if(it != end)   {
      sb = it->first;
      it = front_verts.find(indices_arr[3*k+2]);
      if(it != end)   {
        _CutInner21(sb,it->first,sa);
        fn_adds = 2;
      }
      else    {
        _CutInner12(sb,indices_arr[3*k+2],sa);
        fn_adds = 1;
      }
    }
    else    {
      sb = indices_arr[3*k+1];
      it = front_verts.find(indices_arr[3*k+2]);
      if(it != end)   {
        _CutInner12(it->first,sa,sb);
        fn_adds = 1;
      }
    }
  }
  for(;fn_adds>0;--fn_adds)   {
        for(int i=0; i<3; ++i)  {
      f_norms_new.push_back(
        face_normals_arr[3*k+i]);
    }
    }
}

//�������� �������� ������� ����� ���� ��� ����� ������
while(!verts_new_pairs.empty()) {
  auto it = verts_new_pairs.begin();
    chain.clear();
  chain.push_back(it->first);
  chain.push_back(it->second);
  verts_new_pairs.erase(it);
  while(chain.front() != chain.back())    {
    //�������� �� ��������� �������
    it = verts_new_pairs.find(chain.back());
    if(it != verts_new_pairs.end())   {
      chain.push_back(it->second);
            verts_new_pairs.erase(it);
    }
    else    {
      //��� ����� ����������� �� ������
            //������� ������ ���������� ����
      chain.push_back(chain.front());
            break;
    }
  }
  size_t sz = chain.size()-1,
    ind_sz = inds_new.size();
  GLuint *polygon = new GLuint[sz];
  //������� �������� ������� �������� �����
  auto it0 = chain.begin();
  for(int i = 0; i<sz; ++i)   {
        polygon[i] = *(it0++);
  }
  inds_new.resize(ind_sz+3*(sz-2));
  //������������� � ���������
    //TODO: ��������� ���������� � "�������" ��������
  _Simple_triang_fan(inds_new.data()+ind_sz,
    polygon, sz, 1);
  delete[] polygon;
  for(int k = 0; k<sz-2; ++k) {
    for(int i=0; i<3; ++i)  {
      f_norms_new.push_back(plane[i]);
    }
  }
  if(!no_vec_normals) {
    for(int k = 0; k<sz-2; ++k) {
      flats_new.push_back(f_cnt+k);
    }
  }
    f_cnt += sz-2;
}

//�������� ���������� ������
vertices_arr.swap(verts_new);
indices_arr.swap(inds_new);
face_normals_arr.swap(f_norms_new);
if(!no_vec_normals) {
  vert_normals_arr.swap(v_norms_new);
  flat_face_arr.swap(flats_new);
}
_n_d_vertex = v_cnt;
_n_d_face = f_cnt;
//�������� draw_arr ��� ���������
_draw_buf_upd_flag = true;
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
DLLSO void FigDestroy(void* fig)    {
delete ((Figure3d_unit*)fig);
}

DLLSO void* GetFigVirtualCopy(const void* fig)    {
return ((Figure3d_unit*)fig)->VirtualCopy();
}

DLLSO void FigReplaceContext(void* fig,
  const void* glContext)   {
((Figure3d_unit*)fig)->ReplaceContext(
  (AppGLContext*)glContext);
}

DLLSO void FigResize_d(void* fig,
  double x, double y, double z)    {
((Figure3d_unit*)fig)->Resize(x, y, z);
}

DLLSO void FigResize_dv(void* fig, const double *v)    {
((Figure3d_unit*)fig)->Resize(v);
}

DLLSO void FigMove_d(void* fig,
  double x, double y, double z)    {
((Figure3d_unit*)fig)->Move(x, y, z);
}

DLLSO void FigMove_dv(void* fig, const double *v)    {
((Figure3d_unit*)fig)->Move(v);
}

DLLSO void FigRotate_d(void* fig,
  double x, double y, double z)    {
((Figure3d_unit*)fig)->Rotate(x, y, z);
}

DLLSO void FigRotate_dv(void* fig, const double *v)    {
((Figure3d_unit*)fig)->Rotate(v);
}

DLLSO void GetFigScale_dv(void* fig, double *v)    {
((Figure3d_unit*)fig)->GetScale(v);
}

DLLSO void GetFigCenterPos_dv(void* fig, double *v)    {
((Figure3d_unit*)fig)->GetCenterPos(v);
}
}
#endif

