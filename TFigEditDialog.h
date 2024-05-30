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
 
#ifndef TFigEditDialogH
#define TFigEditDialogH
//----------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.Buttons.hpp>
//----------------------------------------------------------
#include <Figures/figure3d_unit.h>

//����� ��� �������� ��������� �����
class field_action;

//����� ������� ��������� ������ (������� ��������������)
class TFigEditDialog : public TForm  {
__published:  // IDE-managed Components
//������� ����� ����� �����
  TLabel *Label1;
  TLabel *ScaleL;
  TLabel *Label3;
  TLabel *Label4;
  TLabel *Label5;
//���� �����
  //����� ��������� ������� ������ ������
  TEdit *PosXAbs;
  TEdit *PosYAbs;
  TEdit *PosZAbs;
  //�������� ������������ �������� ������ ������
  TEdit *PosXRel;
  TEdit *PosYRel;
  TEdit *PosZRel;
  //����� ������� ������
  TEdit *ScXAbs;
  TEdit *ScYAbs;
  TEdit *ScZAbs;
  //������� ������������ �������
  TEdit *ScXRel;
  TEdit *ScYRel;
  TEdit *ScZRel;
  //������� ������ (������ XYZ)
  TEdit *RotZ;
  TEdit *RotX;
  TEdit *RotY;
  //���� ������ ����������� ���������������
  TCheckBox *UniformCB;
  //������ ������������� ���������
  TBitBtn *OkBtn;
  TBitBtn *CancelBtn;
//������� ��������� �������� � ����� �����
  void __fastcall PosXAbsChange(TObject *Sender);
  void __fastcall PosYAbsChange(TObject *Sender);
  void __fastcall PosZAbsChange(TObject *Sender);
  void __fastcall PosXRelChange(TObject *Sender);
  void __fastcall PosYRelChange(TObject *Sender);
  void __fastcall PosZRelChange(TObject *Sender);
  void __fastcall RotXChange(TObject *Sender);
  void __fastcall RotYChange(TObject *Sender);
  void __fastcall RotZChange(TObject *Sender);
  void __fastcall ScXAbsChange(TObject *Sender);
  void __fastcall ScYAbsChange(TObject *Sender);
  void __fastcall ScZAbsChange(TObject *Sender);
  void __fastcall ScXRelChange(TObject *Sender);
  void __fastcall ScYRelChange(TObject *Sender);
  void __fastcall ScZRelChange(TObject *Sender);
private:  // User declarations
  //��������� �� ���������� ������
  Figure3d_unit* figure;
  //���� ��� ������ �� ������ ��� ��������� �����
  bool edit_flag = false;
  //��������� ���������� ���� ����� �����
  bool TestFields();
  //������ ��������� ����� �����
  void FieldActionWrap(const field_action &act);
public:    // User declarations
  double Offset[3]; //����� ������� ������ ������
  double Offset_old[3]; //������� ������� ������ ������
  double Scale[3]; //����� ������� ������
  double Scale_old[3]; //������� ������� ������
  double Rotation[3] = {0}; //������� ������ (������ XYZ)
  bool Execute(); //������� ������ (true ���� ��� "��")
  //����������� � ������������ ������ � ���������� �������
  __fastcall TFigEditDialog(TComponent* Owner,
    Figure3d_unit* figure);
  //���� �����
  TEdit* Fields[15];
};
//----------------------------------------------------------
extern PACKAGE TFigEditDialog *FigEditDialog;
//----------------------------------------------------------
#endif
