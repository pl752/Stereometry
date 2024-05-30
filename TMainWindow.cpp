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

//Глобальный объект основной формы
TMainWindow *MainWindow;
//----------------------------------------------------------


//Конструктор с привязкой к создающему объекту
__fastcall TMainWindow::TMainWindow(TComponent* Owner)
  : TForm(Owner) {}

//Заполнить список фигур и операций пунктами
void TMainWindow::FillFigureList() {
TTreeNode *Parent, *Node;
Parent = TreeView1->Items->Add(0, "Платоновы тела");
Node = TreeView1->Items->AddChild(Parent, "Тетраэдр");
Node->Data = (void*)[](){
  MainWindow->AddFigure(
    ArrayFig::NewTetrahedron(MainWindow->glContext.get()),
    "Тетраэдр");
};
Node = TreeView1->Items->AddChild(Parent, "Куб");
Node->Data = (void*)[](){
  MainWindow->AddFigure(
    ArrayFig::NewCube(MainWindow->glContext.get()), "Куб");
};
Node = TreeView1->Items->AddChild(Parent, "Октаэдр");
Node->Data = (void*)[](){
  MainWindow->AddFigure(
    ArrayFig::NewOctahedron(MainWindow->glContext.get()),
    "Октаэдр");
};
Node = TreeView1->Items->AddChild(Parent, "Икосаэдр");
Node->Data = (void*)[](){
  MainWindow->AddFigure(
    ArrayFig::NewIcosahedron(MainWindow->glContext.get()),
    "Икосаэдр");
};
Node = TreeView1->Items->AddChild(Parent, "Додекаэдр");
Node->Data = (void*)[](){
  MainWindow->AddFigure(
    ArrayFig::NewDodecahedron(MainWindow->glContext.get()),
    "Додекаэдр");
};
Node = TreeView1->Items->
  AddChild(Parent, "Ромбододекаэдр");
Node->Data = (void*)[](){
  MainWindow->AddFigure(
    ArrayFig::NewRhombicdodecahedron(
      MainWindow->glContext.get()),
    "Ромбододекаэдр");
};

Parent = TreeView1->Items->Add(0, "Тела вращения");
Node = TreeView1->Items->AddChild(Parent, "Конус");
Node->Data = (void*)[](){
  MainWindow->AddFigure(new Cone_fig(
      MainWindow->glContext.get(), 100),
    "Конус");
};
Node = TreeView1->Items->AddChild(Parent, "Цилиндр");
Node->Data = (void*)[](){
  MainWindow->AddFigure(
    new Cylinder_fig(MainWindow->glContext.get(), 100),
    "Цилиндр");
};
Node = TreeView1->Items->AddChild(Parent, "Сфера (глобус)");
Node->Data = (void*)[](){
  MainWindow->AddFigure(
    new UV_Sphere_fig(MainWindow->glContext.get(), 100, 50),
    "Сфера (глобус)");
};
Node = TreeView1->Items->AddChild(Parent, "Бублик");
Node->Data = (void*)[](){
  MainWindow->AddFigure(
    new Torus_fig(
      MainWindow->glContext.get(), 100, 100, 1.0, 0.5),
    "Бублик");
};

Parent = TreeView1->Items->Add(0, "Операции");
Node = TreeView1->Items->AddChild(Parent, "Сечение");
Node->Data = (void*)[](){
  if(MainWindow->GetGLContext()->SetCuttingMode()) {
    MainWindow->UpdateButtons();
    MainWindow->glContext->DrawFrame();
  }
};

Node = TreeView1->Items->AddChild(Parent, "Загрузить STL");
Node->Data = (void*)[](){
  if(MainWindow->STLOpenDialog->Execute(
      MainWindow->Handle))  {
    AnsiString ansiLine(
      MainWindow->STLOpenDialog->FileName.c_str());
    MainWindow->AddFigure(
      new STLLoadableFig(MainWindow->glContext.get(),
      ansiLine.c_str()), ansiLine.c_str());
  }
};

Node = TreeView1->Items->AddChild(Parent,
  "Копировать фигуру");
Node->Data = (void*)[](){
auto fig = MainWindow->glContext->GetSelectedFig();
if(fig != nullptr)
  MainWindow->AddFigure(
    fig->VirtualCopy(),
    AnsiString(MainWindow->ListView1->Selected->
      Caption).c_str());
};
}

//Добавить фигуру в пространство и зарегистрировать
//в списке под заданным именем
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

//Обработчик нажатия кн. добавления фигуры или операции
//или отмены текущего действия (F2Mode!=F2Normal)
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
  //Если для пункта списка существует действие - выполнить
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

//Обработчик нажатия кн. удаления фигуры
//Удалить фигуру из пространства и списка в форме
void __fastcall TMainWindow::Item_remove_btnClick(
  TObject *Sender)  {
Figure3d_unit *fig = glContext->GetSelectedFig();
if(fig != nullptr)  {
  auto it = Figure_fig_to_list.find(fig);
  if(it != Figure_fig_to_list.end())  {
    ListView1->Items->Delete(it->second->Index);
    Figure_fig_to_list.erase(it);
  }
  //Не забыть снять выбор и почистить память
  glContext->RemoveFigure(fig);
  auto dl_it = std::remove(OwnedFigures.begin(),
    OwnedFigures.end(), fig);
  OwnedFigures.erase(dl_it, OwnedFigures.end());
  delete fig;
  glContext->DrawFrame();
}
}

//Вызов диалога изменения фигуры по кн. "Изменить"
void __fastcall TMainWindow::Item_edit_btnClick(
  TObject *Sender)  {
Figure3d_unit *fig = glContext->GetSelectedFig();
if(fig != nullptr)  {
  //Создать и выполнить диалог изменения выбр. фигуры
    //Если было нажато "ОК"
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

//Настройка элементов формы и контекста отрисовки
void __fastcall TMainWindow::FormCreate(
  TObject *Sender)  {
//Создать и выбрать контексты окна и отрисовки OpenGL
glContext = std::make_unique<AppGLContext>(Panel1->Handle,
    Panel1->Width, Panel1->Height);
//Завершить подготовку формы
KeyPreview = true;
FillFigureList();
}

//Освобождение контекста отрисовки и памяти
void __fastcall TMainWindow::FormDestroy(
  TObject *Sender)  {
//Освобождаем контекст отрисовки
glContext.reset();
//Чистим собственные фигуры
for(auto x: OwnedFigures) delete x;
OwnedFigures.clear();
}

//Масштабирование и нормализация графического окна
void __fastcall TMainWindow::FormResize(
  TObject *Sender)  {
glContext->Setup_viewPort(Panel1->Width, Panel1->Height);
}

//Обновление содержимого графического окна
void __fastcall TMainWindow::FormPaint(TObject *Sender)  {
glContext->DrawFrame();
}

//Обновление кнопок
void TMainWindow::UpdateButtons()    {
//Назначение кнопки "Добавить" зависит от режима
switch(glContext->GetFigMode())  {
case F2CuttingFig:
  Item_add_btn->Caption = "Отмена";
  Item_edit_btn->Enabled = false;
  Item_remove_btn->Enabled = false;
  break;
default:
  Item_add_btn->Caption = "Добавить";
  Item_edit_btn->Enabled = true;
  Item_remove_btn->Enabled = true;
  break;
}
}

//Обработчик начала операций с кн. мыши
void __fastcall TMainWindow::Panel1MouseDown(TObject *Sender,
  TMouseButton Button, TShiftState Shift, int X, int Y) {
mouse_btn_state |= (1 << (int)Button);
//В начале нажатия ЛКМ - считаем нажатие простым
if(mouse_btn_state & 1) {
  mouse_move_dst = 0;
  simple_click = true;
}
}

//Обработчик нажатий ЛКМ и отпускания кн. мыши
void __fastcall TMainWindow::Panel1MouseUp(TObject *Sender,
  TMouseButton Button, TShiftState Shift, int X, int Y) {
//Обработка простого нажатия при отпускании ЛКМ
if(simple_click && (int)Button == 0 &&
  (mouse_btn_state & 1)) {
  Figure3d_unit *fig;
  const Figure3d_unit *fig_selection =
    glContext->GetSelectedFig();
  glContext->CtrlSimpleClick(X, Y);
  fig = glContext->GetSelectedFig();
  //В обычном режиме выбираем фигуру кликом
  switch(glContext->GetFigMode())    {
  case F2Normal:
    //Если никакая фигура не кликнута, снимаем выбор
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
    glContext->DrawFrame(); //Обновляем кадр
    break;
  }
  UpdateButtons();
}
mouse_btn_state &= ~(1 << (int)Button);
}

//Обработчик перемещения курсора с зажатыми кн. мыши
void __fastcall TMainWindow::Panel1MouseMove(
  TObject *Sender, TShiftState Shift, int X, int Y)  {
static int mx, my; //Прошлое положение курсора
//Если зажатых кнопок нет - подмечаем и выходим
if (!mouse_btn_state) {
  mx = -1;
  return;
}
//Для начала движения мыши с зажатой кнопкой
if(mx == -1)   {
  mx = X;
  my = Y;
}
//движение с ЛКМ - вращение камеры
if(mouse_btn_state & 1) {
  //Если движение значительно - клик не простой
  mouse_move_dst += abs(X-mx)+abs(Y-my);
  if(mouse_move_dst > 5) simple_click = false;
  glContext->CtrlCamRotate(X-mx, Y-my);
}
//движение с ПКМ по вертикали - отдаление камеры (дв. вниз)
else if(mouse_btn_state & 2)    {
  glContext->CtrlCamZoom(Y-my);
}
//движение с СКМ движение точки обзора и камеры по xOy
else if(mouse_btn_state & 4)    {
  glContext->CtrlCamMoveXoY(X-mx, Y-my);
}
//Перенастроить матрицу обзора и обновить кадр
glContext->Setup_camera();
glContext->DrawFrame();
//Запомнить координаты для следующего движения
mx = X;
my = Y;
}

//Обработчик события покидания курсором окна
void __fastcall TMainWindow::Panel1MouseLeave(
  TObject *Sender) {
//Если мышь покинула форму - сброс флагов мыши и
//простого нажатия
mouse_btn_state = 0;
simple_click = false;
}

//Обработчик нажатия кн. добавления фигуры или операции
//или отмены текущего действия (F2Mode!=F2Normal)
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
glContext->DrawFrame(); //Обновляем кадр
}

//Перемещение камеры к выбр. фигуре
void __fastcall TMainWindow::ListView1DblClick(
  TObject *Sender) {
if(ListView1->Selected && ListView1->Selected->Data)   {
  double pos[3];
  const Figure3d_unit* fig =
    (Figure3d_unit*)ListView1->Selected->Data;
  //Перемещение точки обзора к фигуре
  fig->GetCenterPos(pos);
  glContext->SetViewPos(pos[0], pos[1]);
  //Перенастроить матрицу обзора и обновить кадр
  glContext->Setup_camera();
  glContext->DrawFrame();
}
}

