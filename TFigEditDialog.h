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

//Класс для делегата изменения формы
class field_action;

//Форма диалога изменения фигуры (Афинные преобразования)
class TFigEditDialog : public TForm  {
__published:  // IDE-managed Components
//Подписи рядов полей ввода
  TLabel *Label1;
  TLabel *ScaleL;
  TLabel *Label3;
  TLabel *Label4;
  TLabel *Label5;
//Поля ввода
  //Новая асолютная позиция центра фигуры
  TEdit *PosXAbs;
  TEdit *PosYAbs;
  TEdit *PosZAbs;
  //Смещение относительно текущего центра фигуры
  TEdit *PosXRel;
  TEdit *PosYRel;
  TEdit *PosZRel;
  //Новый масштаб фигуры
  TEdit *ScXAbs;
  TEdit *ScYAbs;
  TEdit *ScZAbs;
  //Масштаб относительно старого
  TEdit *ScXRel;
  TEdit *ScYRel;
  TEdit *ScZRel;
  //Поворот фигуры (эйлера XYZ)
  TEdit *RotZ;
  TEdit *RotX;
  TEdit *RotY;
  //Флаг режима однородного масштабирования
  TCheckBox *UniformCB;
  //Кнопки подтверждения изменений
  TBitBtn *OkBtn;
  TBitBtn *CancelBtn;
//События изменения значений в полях ввода
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
  //Указатель на изменяемую фигуру
  Figure3d_unit* figure;
  //Флаг для защиты от циклов при изменении полей
  bool edit_flag = false;
  //Проверить валидность всех полей ввода
  bool TestFields();
  //Обёртка обработки полей ввода
  void FieldActionWrap(const field_action &act);
public:    // User declarations
  double Offset[3]; //Новая позиция центра фигуры
  double Offset_old[3]; //Текущая позиция центра фигуры
  double Scale[3]; //Новый масштаб фигуры
  double Scale_old[3]; //Текущий масштаб фигуры
  double Rotation[3] = {0}; //Поворот фигуры (эйлера XYZ)
  bool Execute(); //Вызвать диалог (true лишь при "ОК")
  //Конструктор с родительской формой и изменяемой фигурой
  __fastcall TFigEditDialog(TComponent* Owner,
    Figure3d_unit* figure);
  //Поля ввода
  TEdit* Fields[15];
};
//----------------------------------------------------------
extern PACKAGE TFigEditDialog *FigEditDialog;
//----------------------------------------------------------
#endif
