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

//Тип указателя для действий кнопки "Добавить"
typedef void (*ActionSelector)();

//Основная форма приложения
class TMainWindow : public TForm  {
__published:  // IDE-managed Components
  TPanel *Panel1; //Вьюпорт (графическое окно)
  //Список фигур и операций по разделам
  TTreeView *TreeView1;
  //Список созданных фигур
  TListView *ListView1;
  //Добавить фигуру или операцию или отменить
  //текущую операцию
  TButton *Item_add_btn;
  TButton *Item_edit_btn; //Изменить выбр. фигуру (диалог)
  TButton *Item_remove_btn;//Удалить выбр. фигуру (диалог)
  TPanel *Panel2; //Панель с TreeView1 и кн. "Добавить"
  //Панель с ListView1 и кн. "Изменить" и "Удалить"
  TPanel *Panel3;
  TOpenDialog *STLOpenDialog;
  //Настройка элементов формы и контекста отрисовки
  void __fastcall FormCreate(TObject *Sender);
  //Освобождение контекста отрисовки и памяти
  void __fastcall FormDestroy(TObject *Sender);
  //Обновление содержимого графического окна
  void __fastcall FormPaint(TObject *Sender);
  //Масштабирование и нормализация графического окна
  void __fastcall FormResize(TObject *Sender);
  //Обработчик начала операций с кн. мыши
  void __fastcall Panel1MouseDown(TObject *Sender,
    TMouseButton Button, TShiftState Shift,
    int X, int Y);
  //Обработчик нажатий ЛКМ и отпускания кн. мыши
  void __fastcall Panel1MouseUp(TObject *Sender,
    TMouseButton Button, TShiftState Shift,
    int X, int Y);
  //Обработчик перемещения курсора с зажатыми кн. мыши
  void __fastcall Panel1MouseMove(TObject *Sender,
    TShiftState Shift, int X, int Y);
  //Обработчик события покидания курсором окна
  void __fastcall Panel1MouseLeave(TObject *Sender);
  //Обработчик нажатия кн. добавления фигуры или операции
  //или отмены текущего действия (F2Mode!=F2Normal)
  void __fastcall Item_add_btnClick(TObject *Sender);
  //Обработчик нажатия кн. удаления фигуры
  //Удалить фигуру из пространства и списка в форме
  void __fastcall Item_remove_btnClick(TObject *Sender);
  //Выбор фигуры в простр. в соотв. с выбором в списке
  void __fastcall ListView1SelectItem(TObject *Sender,
    TListItem *Item, bool Selected);
  //Перемещение камеры к выбр. фигуре
  void __fastcall ListView1DblClick(TObject *Sender);
  //Вызов диалога изменения фигуры по кн. "Изменить"
  void __fastcall Item_edit_btnClick(TObject *Sender);
private:  // User declarations
  //Контекст отрисовки и окна (OpenGL) (владеющий указатель)
  std::unique_ptr<AppGLContext> glContext;
  //Список фигур для очистки (владеющий)
  std::vector<Figure3d_unit*> OwnedFigures;
  //Соответствие фигуры пункту в списке созд. фигур
  std::map<Figure3d_unit*, TListItem*> Figure_fig_to_list;
  //Манхеттенская дистанция перемещения мыши от т. зажатия
  unsigned int mouse_move_dst = 0;
  //Флаги зажатых клавиш (0b1-ЛКМ, 0b10-ПКМ, 0b100-СКМ)
  char mouse_btn_state = 0;
  //Флаг отличия простого нажатия ЛКМ от перетаскивания
  bool simple_click = false;
  //Заполнить список фигур и операций пунктами
  void FillFigureList();
  //Обновление кнопок
  void UpdateButtons();
public:    // User declarations
  //Получить контекст OpenGL
  inline AppGLContext* GetGLContext() const {
  return glContext.get();
  }
  //Добавить фигуру в пространство и зарегистрировать
  //в списке под заданным именем
  void AddFigure(Figure3d_unit*, const char*);
  //Конструктор с привязкой к создающему объекту
  __fastcall TMainWindow(TComponent* Owner);
};
//----------------------------------------------------------
//Глобальный объект основной формы
extern PACKAGE TMainWindow *MainWindow;
//----------------------------------------------------------
#endif
