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

#include "TFigEditDialog.h"
//----------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFigEditDialog *FigEditDialog;
//----------------------------------------------------------

constexpr double DEG2RAD = M_PI/180.0;

//Класс для делегата изменения формы
class field_action  {
protected:
  TFigEditDialog &form;
  int type;
public:
  field_action(TFigEditDialog &form, int type) :
    form(form), type(type) {}
  virtual bool operator()() const {return true;}
};

class PosAbs_action : public field_action {
public:
  PosAbs_action(TFigEditDialog &form, int type) :
    field_action(form, type) {}
  bool operator()() const override {
    //Изменяем парные поля для соответствия
    double val = StrToFloat(form.Fields[type]->Text);
    form.Fields[type+3]->Text =
      FloatToStrF(val-form.Offset_old[type],
      ffFixed, 15, 2);
    form.Offset[type] = val;
    return true;
    }
};

class PosRel_action : public field_action {
public:
  PosRel_action(TFigEditDialog &form, int type) :
    field_action(form, type) {}
  bool operator()() const override {
    //Изменяем парные поля для соответствия
    double val = StrToFloat(form.Fields[type+3]->Text)+
      form.Offset_old[type];
    form.Fields[type]->Text = FloatToStrF(val,
      ffFixed, 15, 2);
    form.Offset[type] = val;
    return true;
  }
};

class ScAbs_action : public field_action {
public:
  ScAbs_action(TFigEditDialog &form, int type) :
    field_action(form, type) {}
  bool operator()() const override {
    //Изменяем парные поля для соответствия
    double val = StrToFloat(form.Fields[type+6]->Text);
    if(val < 0.0) return false;
    form.Scale[type] = val;
    val /= form.Scale_old[type];
    form.Fields[type+9]->Text =
      FloatToStrF(val, ffFixed, 15, 2);
    if(form.UniformCB->Checked)  {
    //Однородный масштаб одинаков для отн. XYZ
      for(int i = 0; i<3; ++i)    {
        form.Fields[i+9]->Text =
          FloatToStrF(val, ffFixed, 15, 2);
        form.Scale[i] = val*form.Scale_old[i];
        form.Fields[i+6]->Text = FloatToStrF(
          form.Scale[i], ffFixed, 15, 2);
      }
    }
    return true;
  }
};

class ScRel_action : public field_action {
public:
  ScRel_action(TFigEditDialog &form, int type) :
    field_action(form, type) {}
  bool operator()() const override {
    //Изменяем парные поля для соответствия
    double val = StrToFloat(form.Fields[type+9]->Text);
    if(val < 0.0) return false;
    form.Scale[type] = val*form.Scale_old[type];
    form.Fields[type+6]->Text =
      FloatToStrF(form.Scale[type], ffFixed, 15, 2);
    if(form.UniformCB->Checked)  {
    //Однородный масштаб одинаков для отн. XYZ
      for(int i = 0; i<3; ++i)    {
        form.Fields[i+9]->Text =
          FloatToStrF(val, ffFixed, 15, 2);
        form.Scale[i] = val*form.Scale_old[i];
        form.Fields[i+6]->Text = FloatToStrF(
          form.Scale[i], ffFixed, 15, 2);
      }
    }
    return true;
  }
};

class Rot_action : public field_action {
public:
  Rot_action(TFigEditDialog &form, int type) :
    field_action(form, type) {}
  bool operator()() const override {
    form.Rotation[type] =
      DEG2RAD*StrToFloat(form.Fields[type+12]->Text);
    return true;
  }
};

//Конструктор с родительской формой и изменяемой фигурой
__fastcall TFigEditDialog::TFigEditDialog(TComponent* Owner,
  Figure3d_unit* figure) : TForm(Owner), figure(figure) {
figure->GetCenterPos(Offset);
figure->GetScale(Scale);
figure->GetCenterPos(Offset_old);
figure->GetScale(Scale_old);
edit_flag = true;
//Наполнение полей текущими параметрами
PosXAbs->Text = FloatToStrF(Offset[0], ffFixed, 15, 2);
PosYAbs->Text = FloatToStrF(Offset[1], ffFixed, 15, 2);
PosZAbs->Text = FloatToStrF(Offset[2], ffFixed, 15, 2);
PosXRel->Text = FloatToStrF(0.0, ffFixed, 15, 2);
PosYRel->Text = FloatToStrF(0.0, ffFixed, 15, 2);
PosZRel->Text = FloatToStrF(0.0, ffFixed, 15, 2);
RotX->Text = FloatToStrF(0.0, ffFixed, 15, 2);
RotY->Text = FloatToStrF(0.0, ffFixed, 15, 2);
RotZ->Text = FloatToStrF(0.0, ffFixed, 15, 2);
ScXAbs->Text = FloatToStrF(Scale[0], ffFixed, 15, 2);
ScYAbs->Text = FloatToStrF(Scale[1], ffFixed, 15, 2);
ScZAbs->Text = FloatToStrF(Scale[2], ffFixed, 15, 2);
ScXRel->Text = FloatToStrF(1.0, ffFixed, 15, 2);
ScYRel->Text = FloatToStrF(1.0, ffFixed, 15, 2);
ScZRel->Text = FloatToStrF(1.0, ffFixed, 15, 2);
edit_flag = false;
const TEdit* fields[15] = {
  PosXAbs, PosYAbs, PosZAbs, PosXRel, PosYRel, PosZRel,
  ScXAbs, ScYAbs, ScZAbs, ScXRel, ScYRel, ScZRel,
  RotX, RotY, RotZ
};
std::memcpy(Fields, fields, sizeof(Fields));
}

//Вызвать диалог (true лишь при "ОК")
bool TFigEditDialog::Execute()  {
ShowModal();
return (ModalResult == mrOk);
}

//Обёртка обработки полей ввода
void TFigEditDialog::FieldActionWrap(
  const field_action &act)  {
if(edit_flag) return;
edit_flag = true;
try {
  if(act())   {
    if(!OkBtn->Enabled) OkBtn->Enabled = TestFields();
  }
  else {
    OkBtn->Enabled = false;
  }
  edit_flag = false;
} catch (...) {
//Ошибка парсинга - блокируем ОК
  OkBtn->Enabled = false;
  edit_flag = false;
}
}

//События изменения значений в полях ввода
void __fastcall TFigEditDialog::PosXAbsChange(
  TObject *Sender)  {
FieldActionWrap(PosAbs_action(*this, 0));
}

void __fastcall TFigEditDialog::PosYAbsChange(
  TObject *Sender)  {
FieldActionWrap(PosAbs_action(*this, 1));
}

void __fastcall TFigEditDialog::PosZAbsChange(
  TObject *Sender)  {
FieldActionWrap(PosAbs_action(*this, 2));
}

void __fastcall TFigEditDialog::PosXRelChange(
  TObject *Sender)  {
FieldActionWrap(PosRel_action(*this, 0));
}

void __fastcall TFigEditDialog::PosYRelChange(
  TObject *Sender)  {
FieldActionWrap(PosRel_action(*this, 1));
}

void __fastcall TFigEditDialog::PosZRelChange(
  TObject *Sender)  {
FieldActionWrap(PosRel_action(*this, 2));
}

void __fastcall TFigEditDialog::RotXChange(
  TObject *Sender)  {
FieldActionWrap(Rot_action(*this, 0));
}

void __fastcall TFigEditDialog::RotYChange(
  TObject *Sender)  {
FieldActionWrap(Rot_action(*this, 1));
}

void __fastcall TFigEditDialog::RotZChange(
  TObject *Sender)  {
FieldActionWrap(Rot_action(*this, 2));
}

void __fastcall TFigEditDialog::ScXAbsChange(
  TObject *Sender)  {
FieldActionWrap(ScAbs_action(*this, 0));
}

void __fastcall TFigEditDialog::ScYAbsChange(
  TObject *Sender)  {
FieldActionWrap(ScAbs_action(*this, 1));
}

void __fastcall TFigEditDialog::ScZAbsChange(
  TObject *Sender)  {
FieldActionWrap(ScAbs_action(*this, 2));
}

void __fastcall TFigEditDialog::ScXRelChange(
  TObject *Sender)  {
FieldActionWrap(ScRel_action(*this, 0));
}

void __fastcall TFigEditDialog::ScYRelChange(
  TObject *Sender)  {
FieldActionWrap(ScRel_action(*this, 1));
}

void __fastcall TFigEditDialog::ScZRelChange(
  TObject *Sender)  {
FieldActionWrap(ScRel_action(*this, 2));
}

//Проверить валидность всех полей ввода
bool TFigEditDialog::TestFields()   {
try {
  volatile double val;
  int i=0;
  for(; i<6; ++i)  {
    val = StrToFloat(Fields[i]->Text);
  }
  for(; i<12; ++i)  {
    val = StrToFloat(Fields[i]->Text);
    if(val < 0.0) return false;
  }
  for(; i<15; ++i)  {
    val = StrToFloat(Fields[i]->Text);
  }
  return true;
} catch(...)  {
  return false;
}
}

