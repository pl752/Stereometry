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
#include <tchar.h>
//----------------------------------------------------------
USEFORM("TMainWindow.cpp", MainWindow);
USEFORM("TFigEditDialog.cpp", FigEditDialog);
//---------------------------------------------------------------------------
int WINAPI _tWinMain(HINSTANCE, HINSTANCE, LPTSTR, int) {
try {
  Application->Initialize();
  Application->MainFormOnTaskBar = true;
  Application->CreateForm(__classid(TMainWindow),
    &MainWindow);
  Application->CreateForm(__classid(TFigEditDialog),
    &FigEditDialog);
  Application->Run();
}
catch (Exception &exception) {
  Application->ShowException(&exception);
}
catch (...) {
  try {
    throw Exception("");
  }
  catch (Exception &exception) {
    Application->ShowException(&exception);
  }
}
return 0;
}
//----------------------------------------------------------
