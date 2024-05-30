object FigEditDialog: TFigEditDialog
  Left = 0
  Top = 0
  BorderStyle = bsDialog
  Caption = #1048#1079#1084#1077#1085#1080#1090#1100
  ClientHeight = 177
  ClientWidth = 532
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  TextHeight = 13
  object Label1: TLabel
    Left = 8
    Top = 9
    Width = 56
    Height = 16
    Caption = #1055#1086#1079#1080#1094#1080#1103
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object ScaleL: TLabel
    Left = 8
    Top = 63
    Width = 60
    Height = 16
    Caption = #1052#1072#1089#1096#1090#1072#1073
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object Label3: TLabel
    Left = 8
    Top = 36
    Width = 69
    Height = 16
    Caption = #1057#1084#1077#1097#1077#1085#1080#1077
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object Label4: TLabel
    Left = 8
    Top = 117
    Width = 67
    Height = 16
    Caption = #1042#1088#1072#1097#1077#1085#1080#1077
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object Label5: TLabel
    Left = 8
    Top = 90
    Width = 82
    Height = 16
    Caption = #1056#1072#1089#1090#1103#1078#1077#1085#1080#1077
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object PosXAbs: TEdit
    Left = 128
    Top = 8
    Width = 97
    Height = 21
    TabOrder = 0
    Text = 'PosXAbs'
    OnChange = PosXAbsChange
  end
  object PosYAbs: TEdit
    Left = 231
    Top = 8
    Width = 97
    Height = 21
    TabOrder = 1
    Text = 'PosYAbs'
    OnChange = PosYAbsChange
  end
  object PosZAbs: TEdit
    Left = 334
    Top = 8
    Width = 97
    Height = 21
    TabOrder = 2
    Text = 'PosZAbs'
    OnChange = PosZAbsChange
  end
  object PosXRel: TEdit
    Left = 128
    Top = 35
    Width = 97
    Height = 21
    TabOrder = 3
    Text = 'PosXRel'
    OnChange = PosXRelChange
  end
  object PosYRel: TEdit
    Left = 231
    Top = 35
    Width = 97
    Height = 21
    TabOrder = 4
    Text = 'PosYRel'
    OnChange = PosYRelChange
  end
  object PosZRel: TEdit
    Left = 334
    Top = 35
    Width = 97
    Height = 21
    TabOrder = 5
    Text = 'PosZRel'
    OnChange = PosZRelChange
  end
  object ScXAbs: TEdit
    Left = 128
    Top = 62
    Width = 97
    Height = 21
    TabOrder = 6
    Text = 'ScXAbs'
    OnChange = ScXAbsChange
  end
  object ScYAbs: TEdit
    Left = 231
    Top = 62
    Width = 97
    Height = 21
    TabOrder = 7
    Text = 'ScYAbs'
    OnChange = ScYAbsChange
  end
  object ScZAbs: TEdit
    Left = 334
    Top = 62
    Width = 97
    Height = 21
    TabOrder = 8
    Text = 'ScZAbs'
    OnChange = ScZAbsChange
  end
  object ScXRel: TEdit
    Left = 128
    Top = 89
    Width = 97
    Height = 21
    TabOrder = 9
    Text = 'ScXRel'
    OnChange = ScXRelChange
  end
  object ScYRel: TEdit
    Left = 231
    Top = 89
    Width = 97
    Height = 21
    TabOrder = 10
    Text = 'ScYRel'
    OnChange = ScYRelChange
  end
  object ScZRel: TEdit
    Left = 334
    Top = 89
    Width = 97
    Height = 21
    TabOrder = 11
    Text = 'ScZRel'
    OnChange = ScZRelChange
  end
  object RotZ: TEdit
    Left = 334
    Top = 116
    Width = 97
    Height = 21
    TabOrder = 12
    Text = 'RotZ'
    OnChange = RotZChange
  end
  object RotX: TEdit
    Left = 128
    Top = 116
    Width = 97
    Height = 21
    TabOrder = 13
    Text = 'RotX'
    OnChange = RotXChange
  end
  object RotY: TEdit
    Left = 231
    Top = 116
    Width = 97
    Height = 21
    TabOrder = 14
    Text = 'RotY'
    OnChange = RotYChange
  end
  object UniformCB: TCheckBox
    Left = 437
    Top = 91
    Width = 97
    Height = 17
    Caption = #1054#1076#1085#1086#1088#1086#1076#1085#1086#1077
    Checked = True
    State = cbChecked
    TabOrder = 15
  end
  object OkBtn: TBitBtn
    Left = 453
    Top = 145
    Width = 75
    Height = 25
    Kind = bkOK
    NumGlyphs = 2
    TabOrder = 16
  end
  object CancelBtn: TBitBtn
    Left = 372
    Top = 145
    Width = 75
    Height = 25
    Caption = #1054#1090#1084#1077#1085#1072
    Kind = bkCancel
    NumGlyphs = 2
    TabOrder = 17
  end
end
