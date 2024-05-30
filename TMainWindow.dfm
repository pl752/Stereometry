object MainWindow: TMainWindow
  Left = 0
  Top = 0
  Caption = #1057#1090#1077#1088#1077#1086#1084#1077#1090#1088#1080#1103
  ClientHeight = 473
  ClientWidth = 891
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OnCreate = FormCreate
  OnDestroy = FormDestroy
  OnPaint = FormPaint
  OnResize = FormResize
  DesignSize = (
    891
    473)
  TextHeight = 13
  object Panel1: TPanel
    Left = 191
    Top = 8
    Width = 499
    Height = 457
    Anchors = [akLeft, akTop, akRight, akBottom]
    TabOrder = 0
    OnMouseDown = Panel1MouseDown
    OnMouseLeave = Panel1MouseLeave
    OnMouseMove = Panel1MouseMove
    OnMouseUp = Panel1MouseUp
    ExplicitWidth = 495
    ExplicitHeight = 456
  end
  object Panel2: TPanel
    Left = 697
    Top = 8
    Width = 186
    Height = 457
    Anchors = [akTop, akRight, akBottom]
    TabOrder = 1
    ExplicitLeft = 693
    ExplicitHeight = 456
    DesignSize = (
      186
      457)
    object Item_edit_btn: TButton
      Left = 0
      Top = 423
      Width = 81
      Height = 34
      Anchors = [akLeft, akBottom]
      Caption = #1048#1079#1084#1077#1085#1080#1090#1100
      TabOrder = 0
      OnClick = Item_edit_btnClick
      ExplicitTop = 422
    end
    object Item_remove_btn: TButton
      Left = 96
      Top = 423
      Width = 89
      Height = 34
      Anchors = [akRight, akBottom]
      Caption = #1059#1076#1072#1083#1080#1090#1100
      TabOrder = 1
      OnClick = Item_remove_btnClick
      ExplicitTop = 422
    end
    object ListView1: TListView
      Left = 1
      Top = 1
      Width = 184
      Height = 417
      Align = alTop
      Anchors = [akLeft, akTop, akRight, akBottom]
      Columns = <>
      TabOrder = 2
      ViewStyle = vsList
      OnDblClick = ListView1DblClick
      OnSelectItem = ListView1SelectItem
      ExplicitHeight = 416
    end
  end
  object Panel3: TPanel
    Left = 8
    Top = 8
    Width = 178
    Height = 457
    Anchors = [akLeft, akTop, akBottom]
    TabOrder = 2
    ExplicitHeight = 456
    DesignSize = (
      178
      457)
    object TreeView1: TTreeView
      Left = 1
      Top = 1
      Width = 176
      Height = 415
      Align = alTop
      Anchors = [akLeft, akTop, akRight, akBottom]
      Indent = 19
      TabOrder = 0
      OnDblClick = Item_add_btnClick
      ExplicitHeight = 414
    end
    object Item_add_btn: TButton
      Left = 0
      Top = 423
      Width = 178
      Height = 34
      Anchors = [akLeft, akRight, akBottom]
      Caption = #1044#1086#1073#1072#1074#1080#1090#1100
      TabOrder = 1
      OnClick = Item_add_btnClick
      ExplicitTop = 422
    end
  end
  object STLOpenDialog: TOpenDialog
    Filter = '*.stl|*.STL'
    Left = 32
    Top = 376
  end
end
