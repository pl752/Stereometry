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
 
#ifndef TMainWindowH
#define TMainWindowH
//----------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.ComCtrls.hpp>

#include "AppGLContext.h"
#include "Figures/figure3d_unit.h"
#include <Vcl.Dialogs.hpp>

#include <vector>
#include <map>
#include <memory>
//----------------------------------------------------------

//��� ��������� ��� �������� ������ "��������"
typedef void (*ActionSelector)();

//�������� ����� ����������
class TMainWindow : public TForm  {
__published:  // IDE-managed Components
  TPanel *Panel1; //������� (����������� ����)
  //������ ����� � �������� �� ��������
  TTreeView *TreeView1;
  //������ ��������� �����
  TListView *ListView1;
  //�������� ������ ��� �������� ��� ��������
  //������� ��������
  TButton *Item_add_btn;
  TButton *Item_edit_btn; //�������� ����. ������ (������)
  TButton *Item_remove_btn;//������� ����. ������ (������)
  TPanel *Panel2; //������ � TreeView1 � ��. "��������"
  //������ � ListView1 � ��. "��������" � "�������"
  TPanel *Panel3;
  TOpenDialog *STLOpenDialog;
  //��������� ��������� ����� � ��������� ���������
  void __fastcall FormCreate(TObject *Sender);
  //������������ ��������� ��������� � ������
  void __fastcall FormDestroy(TObject *Sender);
  //���������� ����������� ������������ ����
  void __fastcall FormPaint(TObject *Sender);
  //��������������� � ������������ ������������ ����
  void __fastcall FormResize(TObject *Sender);
  //���������� ������ �������� � ��. ����
  void __fastcall Panel1MouseDown(TObject *Sender,
    TMouseButton Button, TShiftState Shift,
    int X, int Y);
  //���������� ������� ��� � ���������� ��. ����
  void __fastcall Panel1MouseUp(TObject *Sender,
    TMouseButton Button, TShiftState Shift,
    int X, int Y);
  //���������� ����������� ������� � �������� ��. ����
  void __fastcall Panel1MouseMove(TObject *Sender,
    TShiftState Shift, int X, int Y);
  //���������� ������� ��������� �������� ����
  void __fastcall Panel1MouseLeave(TObject *Sender);
  //���������� ������� ��. ���������� ������ ��� ��������
  //��� ������ �������� �������� (F2Mode!=F2Normal)
  void __fastcall Item_add_btnClick(TObject *Sender);
  //���������� ������� ��. �������� ������
  //������� ������ �� ������������ � ������ � �����
  void __fastcall Item_remove_btnClick(TObject *Sender);
  //����� ������ � ������. � �����. � ������� � ������
  void __fastcall ListView1SelectItem(TObject *Sender,
    TListItem *Item, bool Selected);
  //����������� ������ � ����. ������
  void __fastcall ListView1DblClick(TObject *Sender);
  //����� ������� ��������� ������ �� ��. "��������"
  void __fastcall Item_edit_btnClick(TObject *Sender);
private:  // User declarations
  //�������� ��������� � ���� (OpenGL) (��������� ���������)
  std::unique_ptr<AppGLContext> glContext;
  //������ ����� ��� ������� (���������)
  std::vector<Figure3d_unit*> OwnedFigures;
  //������������ ������ ������ � ������ ����. �����
  std::map<Figure3d_unit*, TListItem*> Figure_fig_to_list;
  //������������� ��������� ����������� ���� �� �. �������
  unsigned int mouse_move_dst = 0;
  //����� ������� ������ (0b1-���, 0b10-���, 0b100-���)
  char mouse_btn_state = 0;
  //���� ������� �������� ������� ��� �� ��������������
  bool simple_click = false;
  //��������� ������ ����� � �������� ��������
  void FillFigureList();
  //���������� ������
  void UpdateButtons();
public:    // User declarations
  //�������� �������� OpenGL
  inline AppGLContext* GetGLContext() const {
  return glContext.get();
  }
  //�������� ������ � ������������ � ����������������
  //� ������ ��� �������� ������
  void AddFigure(Figure3d_unit*, const char*);
  //����������� � ��������� � ���������� �������
  __fastcall TMainWindow(TComponent* Owner);
};
//----------------------------------------------------------
//���������� ������ �������� �����
extern PACKAGE TMainWindow *MainWindow;
//----------------------------------------------------------
#endif
