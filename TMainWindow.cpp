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
 
#include <vcl.h>
#pragma hdrstop

#include "TMainWindow.h"
#include "TFigEditDialog.h"
#include "Figures/figures3d.h"
//----------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

//���������� ������ �������� �����
TMainWindow *MainWindow;
//----------------------------------------------------------


//����������� � ��������� � ���������� �������
__fastcall TMainWindow::TMainWindow(TComponent* Owner)
  : TForm(Owner) {}

//��������� ������ ����� � �������� ��������
void TMainWindow::FillFigureList() {
TTreeNode *Parent, *Node;
Parent = TreeView1->Items->Add(0, "��������� ����");
Node = TreeView1->Items->AddChild(Parent, "��������");
Node->Data = (void*)[](){
  MainWindow->AddFigure(
    ArrayFig::NewTetrahedron(MainWindow->glContextWeak),
    "��������");
};
Node = TreeView1->Items->AddChild(Parent, "���");
Node->Data = (void*)[](){
  MainWindow->AddFigure(
    ArrayFig::NewCube(MainWindow->glContextWeak), "���");
};
Node = TreeView1->Items->AddChild(Parent, "�������");
Node->Data = (void*)[](){
  MainWindow->AddFigure(
    ArrayFig::NewOctahedron(MainWindow->glContextWeak),
    "�������");
};
Node = TreeView1->Items->AddChild(Parent, "��������");
Node->Data = (void*)[](){
  MainWindow->AddFigure(
    ArrayFig::NewIcosahedron(MainWindow->glContextWeak),
    "��������");
};
Node = TreeView1->Items->AddChild(Parent, "���������");
Node->Data = (void*)[](){
  MainWindow->AddFigure(
    ArrayFig::NewDodecahedron(MainWindow->glContextWeak),
    "���������");
};
Node = TreeView1->Items->
  AddChild(Parent, "��������������");
Node->Data = (void*)[](){
  MainWindow->AddFigure(
    ArrayFig::NewRhombicdodecahedron(
    MainWindow->glContextWeak), "��������������");
};

Parent = TreeView1->Items->Add(0, "���� ��������");
Node = TreeView1->Items->AddChild(Parent, "�����");
Node->Data = (void*)[](){
  MainWindow->AddFigure(new Cone_fig(
      MainWindow->glContextWeak, 100),
    "�����");
};
Node = TreeView1->Items->AddChild(Parent, "�������");
Node->Data = (void*)[](){
  MainWindow->AddFigure(
    new Cylinder_fig(MainWindow->glContextWeak, 100),
    "�������");
};
Node = TreeView1->Items->AddChild(Parent, "����� (������)");
Node->Data = (void*)[](){
  MainWindow->AddFigure(
    new UV_Sphere_fig(MainWindow->glContextWeak, 100, 50),
    "����� (������)");
};
Node = TreeView1->Items->AddChild(Parent, "������");
Node->Data = (void*)[](){
  MainWindow->AddFigure(
    new Torus_fig(
      MainWindow->glContextWeak, 100, 100, 1.0, 0.5),
    "������");
};

Parent = TreeView1->Items->Add(0, "��������");
Node = TreeView1->Items->AddChild(Parent, "�������");
Node->Data = (void*)[](){
  if(MainWindow->glContext->SetCuttingMode()) {
    MainWindow->UpdateButtons();
    MainWindow->glContext->DrawFrame();
  }
};

Node = TreeView1->Items->AddChild(Parent, "��������� STL");
Node->Data = (void*)[](){
  if(MainWindow->STLOpenDialog->Execute(
      MainWindow->Handle))  {
    AnsiString ansiLine(
      MainWindow->STLOpenDialog->FileName.c_str());
    MainWindow->AddFigure(
      new STLLoadableFig(MainWindow->glContextWeak,
      ansiLine.c_str()), ansiLine.c_str());
  }
};

Node = TreeView1->Items->AddChild(Parent,
  "���������� ������");
Node->Data = (void*)[](){
  auto fig = MainWindow->glContext->GetSelectedFig();
  if(fig != nullptr)
    MainWindow->AddFigure(
      fig->VirtualCopy(),
      AnsiString(MainWindow->ListView1->Selected->
        Caption).c_str());
};
}

//�������� ������ � ������������ � ����������������
//� ������ ��� �������� ������
void TMainWindow::AddFigure(Figure3d_unit *fig,
  const char* name)    {
OwnedFigures.push_back(fig);
glContext->AddFigure(fig);
ListView1->AddItem(name, (TObject*)fig);
for(int i = 0; i<ListView1->Items->Count; ++i)  {
  if(ListView1->Items->Item[i]->Data == (void*)fig)   {
    Figure_fig_to_list[fig] = ListView1->Items->Item[i];
    ListView1->Selected = ListView1->Items->Item[i];
    break;
  }
}
}

//���������� ������� ��. ���������� ������ ��� ��������
//��� ������ �������� �������� (F2Mode!=F2Normal)
void __fastcall TMainWindow::Item_add_btnClick(
  TObject *Sender) {
switch (glContext->GetFigMode()) {
case F2CuttingFig:
  if(glContext->CancelOperationMode())    {
    UpdateButtons();
    glContext->DrawFrame();
  }
  break;
default:
  //���� ��� ������ ������ ���������� �������� - ���������
  if(TreeView1->Selected == NULL) return;
  ActionSelector sel =
    (ActionSelector)TreeView1->Selected->Data;
  if(sel != NULL)   {
    sel();
    glContext->DrawFrame();
  }
  break;
}
}

//���������� ������� ��. �������� ������
//������� ������ �� ������������ � ������ � �����
void __fastcall TMainWindow::Item_remove_btnClick(
  TObject *Sender)  {
Figure3d_unit *fig = glContext->GetSelectedFig();
if(fig != nullptr)  {
  auto it = Figure_fig_to_list.find(fig);
  if(it != Figure_fig_to_list.end())  {
    ListView1->Items->Delete(it->second->Index);
    Figure_fig_to_list.erase(it);
  }
  //�� ������ ����� ����� � ��������� ������
  glContext->RemoveFigure(fig);
  auto dl_it = std::remove(OwnedFigures.begin(),
    OwnedFigures.end(), fig);
  OwnedFigures.erase(dl_it, OwnedFigures.end());
  delete fig;
  glContext->DrawFrame();
}
}

//����� ������� ��������� ������ �� ��. "��������"
void __fastcall TMainWindow::Item_edit_btnClick(
  TObject *Sender)  {
Figure3d_unit *fig = glContext->GetSelectedFig();
if(fig != nullptr)  {
  //������� � ��������� ������ ��������� ����. ������
    //���� ���� ������ "��"
  TFigEditDialog *dialog = new TFigEditDialog(this, fig);
  if(dialog->Execute())   {
    double v[3];
    fig->GetScale(v);
    for(int i=0; i<3; ++i)  {
      if(std::fabs(dialog->Scale[i]-v[i]) > 1e-9) {
        fig->Resize(dialog->Scale);
        break;
      }
    }
    for(int i=0; i<3; ++i)  {
      if(std::fabs(dialog->Rotation[i]) > 1e-9) {
        fig->Rotate(dialog->Rotation);
        break;
      }
    }
    fig->GetCenterPos(v);
    for(int i=0; i<3; ++i)  {
      if(std::fabs(dialog->Offset[i]-v[i]) > 1e-9) {
        fig->Move(dialog->Offset);
        break;
      }
    }
    glContext->DrawFrame();
  }
  delete dialog;
}
}

//��������� ��������� ����� � ��������� ���������
void __fastcall TMainWindow::FormCreate(
  TObject *Sender)  {
//������� � ������� ��������� ���� � ��������� OpenGL
glContextWeak = glContext = AppGLContext::Create(
  Panel1->Handle, Panel1->Width, Panel1->Height);
//��������� ���������� �����
KeyPreview = true;
FillFigureList();
}

//������������ ��������� ��������� � ������
void __fastcall TMainWindow::FormDestroy(
  TObject *Sender)  {
//����������� �������� ���������
glContext.reset();
//������ ����������� ������
for(auto x: OwnedFigures) delete x;
OwnedFigures.clear();
}

//��������������� � ������������ ������������ ����
void __fastcall TMainWindow::FormResize(
  TObject *Sender)  {
glContext->Setup_viewPort(Panel1->Width, Panel1->Height);
}

//���������� ����������� ������������ ����
void __fastcall TMainWindow::FormPaint(TObject *Sender)  {
glContext->DrawFrame();
}

//���������� ������
void TMainWindow::UpdateButtons()    {
//���������� ������ "��������" ������� �� ������
switch(glContext->GetFigMode())  {
case F2CuttingFig:
  Item_add_btn->Caption = "������";
  Item_edit_btn->Enabled = false;
  Item_remove_btn->Enabled = false;
  break;
default:
  Item_add_btn->Caption = "��������";
  Item_edit_btn->Enabled = true;
  Item_remove_btn->Enabled = true;
  break;
}
}

//���������� ������ �������� � ��. ����
void __fastcall TMainWindow::Panel1MouseDown(TObject *Sender,
  TMouseButton Button, TShiftState Shift, int X, int Y) {
mouse_btn_state |= (1 << (int)Button);
//� ������ ������� ��� - ������� ������� �������
if(mouse_btn_state & 1) {
  mouse_move_dst = 0;
  simple_click = true;
}
}

//���������� ������� ��� � ���������� ��. ����
void __fastcall TMainWindow::Panel1MouseUp(TObject *Sender,
  TMouseButton Button, TShiftState Shift, int X, int Y) {
//��������� �������� ������� ��� ���������� ���
if(simple_click && (int)Button == 0 &&
  (mouse_btn_state & 1)) {
  Figure3d_unit *fig;
  const Figure3d_unit *fig_selection =
    glContext->GetSelectedFig();
  glContext->CtrlSimpleClick(X, Y);
  fig = glContext->GetSelectedFig();
  //� ������� ������ �������� ������ ������
  switch(glContext->GetFigMode())    {
  case F2Normal:
    //���� ������� ������ �� ��������, ������� �����
    if(fig == nullptr) ListView1->Selected = NULL;
    else {
      if(fig != fig_selection)  {
        auto it = Figure_fig_to_list.find(fig);
        if(it != Figure_fig_to_list.end())  {
          ListView1->Selected = it->second;
        }
        else {
          ListView1->Selected = NULL;
        }
        glContext->DrawFrame();
      }
    }
    break;
  default:
    glContext->DrawFrame(); //��������� ����
    break;
  }
  UpdateButtons();
}
mouse_btn_state &= ~(1 << (int)Button);
}

//���������� ����������� ������� � �������� ��. ����
void __fastcall TMainWindow::Panel1MouseMove(
  TObject *Sender, TShiftState Shift, int X, int Y)  {
static int mx, my; //������� ��������� �������
//���� ������� ������ ��� - ��������� � �������
if (!mouse_btn_state) {
  mx = -1;
  return;
}
//��� ������ �������� ���� � ������� �������
if(mx == -1)   {
  mx = X;
  my = Y;
}
//�������� � ��� - �������� ������
if(mouse_btn_state & 1) {
  //���� �������� ����������� - ���� �� �������
  mouse_move_dst += abs(X-mx)+abs(Y-my);
  if(mouse_move_dst > 5) simple_click = false;
  glContext->CtrlCamRotate(X-mx, Y-my);
}
//�������� � ��� �� ��������� - ��������� ������ (��. ����)
else if(mouse_btn_state & 2)    {
  glContext->CtrlCamZoom(Y-my);
}
//�������� � ��� �������� ����� ������ � ������ �� xOy
else if(mouse_btn_state & 4)    {
  glContext->CtrlCamMoveXoY(X-mx, Y-my);
}
//������������� ������� ������ � �������� ����
glContext->Setup_camera();
glContext->DrawFrame();
//��������� ���������� ��� ���������� ��������
mx = X;
my = Y;
}

//���������� ������� ��������� �������� ����
void __fastcall TMainWindow::Panel1MouseLeave(
  TObject *Sender) {
//���� ���� �������� ����� - ����� ������ ���� �
//�������� �������
mouse_btn_state = 0;
simple_click = false;
}

//���������� ������� ��. ���������� ������ ��� ��������
//��� ������ �������� �������� (F2Mode!=F2Normal)
void __fastcall TMainWindow::ListView1SelectItem(
  TObject *Sender, TListItem *Item, bool Selected)  {
if(glContext->CancelOperationMode())    {
  UpdateButtons();
}
if(!Selected) glContext->SetSelectedFig(nullptr);
else {
  glContext->SetSelectedFig((Figure3d_unit*)Item->Data);
  if(glContext->GetSelectedFig() == NULL)
    glContext->SetSelectedFig(nullptr);
}
glContext->DrawFrame(); //��������� ����
}

//����������� ������ � ����. ������
void __fastcall TMainWindow::ListView1DblClick(
  TObject *Sender) {
if(ListView1->Selected && ListView1->Selected->Data)   {
  double pos[3];
  const Figure3d_unit* fig =
    (Figure3d_unit*)ListView1->Selected->Data;
  //����������� ����� ������ � ������
  fig->GetCenterPos(pos);
  glContext->SetViewPos(pos[0], pos[1]);
  //������������� ������� ������ � �������� ����
  glContext->Setup_camera();
  glContext->DrawFrame();
}
}

