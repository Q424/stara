object DEBUGGER: TDEBUGGER
  Left = 224
  Top = 23
  BorderStyle = bsNone
  Caption = 'DEBUGGER'
  ClientHeight = 673
  ClientWidth = 942
  Color = 11711154
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  FormStyle = fsStayOnTop
  OldCreateOrder = False
  Position = poScreenCenter
  Visible = True
  OnCreate = FormCreate
  OnDblClick = FormDblClick
  OnPaint = FormPaint
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object SpeedButton1: TSpeedButton
    Left = 808
    Top = 632
    Width = 121
    Height = 30
    Caption = 'Uruchom'
    Font.Charset = EASTEUROPE_CHARSET
    Font.Color = clWindowText
    Font.Height = -19
    Font.Name = 'Gabriola'
    Font.Style = []
    ParentFont = False
    OnClick = SpeedButton1Click
  end
  object Shape1: TShape
    Left = 8
    Top = 8
    Width = 153
    Height = 25
    Brush.Color = clGray
    Pen.Color = 4802889
    Pen.Width = 0
    Shape = stRoundRect
  end
  object Label1: TLabel
    Left = 32
    Top = 8
    Width = 51
    Height = 24
    Caption = 'Opcje'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = 3552822
    Font.Height = -19
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
    Transparent = True
  end
  object Shape3: TShape
    Left = 568
    Top = 8
    Width = 153
    Height = 25
    Brush.Color = clGray
    Pen.Color = 4802889
    Shape = stRoundRect
  end
  object Label3: TLabel
    Left = 592
    Top = 8
    Width = 106
    Height = 24
    Caption = 'O programie'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = 3552822
    Font.Height = -19
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
    Transparent = True
  end
  object Shape4: TShape
    Left = 736
    Top = 8
    Width = 153
    Height = 25
    Brush.Color = clGray
    Pen.Color = 4802889
    Shape = stRoundRect
  end
  object Label4: TLabel
    Left = 760
    Top = 8
    Width = 93
    Height = 24
    Caption = 'O autorach'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = 3552822
    Font.Height = -19
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
    Transparent = True
  end
  object Shape5: TShape
    Left = 8
    Top = 48
    Width = 921
    Height = 2
    Brush.Color = clSilver
    Pen.Style = psDot
  end
  object Bevel1: TBevel
    Left = 8
    Top = 40
    Width = 921
    Height = 9
    Shape = bsTopLine
  end
  object SpeedButton2: TSpeedButton
    Left = 680
    Top = 632
    Width = 121
    Height = 30
    Caption = 'Wyjd�'
    Font.Charset = EASTEUROPE_CHARSET
    Font.Color = clWindowText
    Font.Height = -19
    Font.Name = 'Gabriola'
    Font.Style = []
    ParentFont = False
    OnClick = SpeedButton2Click
  end
  object Bevel2: TBevel
    Left = 8
    Top = 624
    Width = 921
    Height = 9
    Shape = bsTopLine
  end
  object Shape6: TShape
    Left = 176
    Top = 8
    Width = 153
    Height = 25
    Brush.Color = clGray
    Pen.Color = 4802889
    Shape = stRoundRect
  end
  object Label5: TLabel
    Left = 200
    Top = 8
    Width = 90
    Height = 24
    Caption = 'Multiplayer'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = 3552822
    Font.Height = -19
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
    Transparent = True
  end
  object lversion: TLabel
    Left = 8
    Top = 628
    Width = 47
    Height = 16
    Caption = 'lversion'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = 5460819
    Font.Height = -13
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
    Transparent = True
  end
  object Label6: TLabel
    Left = 8
    Top = 648
    Width = 72
    Height = 13
    Caption = 'http://eu07.pl'
    Color = clBlue
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clGray
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsItalic]
    ParentColor = False
    ParentFont = False
    Transparent = True
  end
  object Label7: TLabel
    Left = 88
    Top = 648
    Width = 70
    Height = 13
    Caption = 'http:/eu07.es'
    Color = clBlue
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clGray
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsItalic]
    ParentColor = False
    ParentFont = False
    Transparent = True
  end
  object Shape2: TShape
    Left = 896
    Top = 8
    Width = 41
    Height = 25
    Brush.Color = clGray
    Pen.Color = 4802889
    Shape = stCircle
  end
  object Label2: TLabel
    Left = 910
    Top = 8
    Width = 14
    Height = 24
    Caption = 'X'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = 3552822
    Font.Height = -19
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
    Transparent = True
    OnClick = Label2Click
  end
  object LBSCN: TListBox
    Left = 8
    Top = 64
    Width = 177
    Height = 481
    Color = 14535609
    Font.Charset = EASTEUROPE_CHARSET
    Font.Color = 8404992
    Font.Height = -9
    Font.Name = 'MS Serif'
    Font.Style = []
    ItemHeight = 11
    Items.Strings = (
      'GL_ARB_vertex_buffer_object'
      'GL_ARB_vertex_program'
      'GL_ARB_vertex_shader'
      'GL_ARB_vertex_type_10f_11f_11f_rev'
      'GL_ARB_vertex_type_2_10_10_10_rev'
      'GL_ARB_viewport_array'
      'GL_ARB_window_pos'
      'GL_ATI_draw_buffers'
      'GL_ATI_texture_float'
      'GL_ATI_texture_mirror_once'
      'GL_S3_s3tc'
      'GL_EXT_texture_env_add'
      'GL_EXT_abgr'
      'GL_EXT_bgra'
      'GL_EXT_bindable_uniform'
      'GL_EXT_blend_color'
      'GL_EXT_blend_equation_separate'
      'GL_EXT_blend_func_separate'
      'GL_EXT_blend_minmax'
      'GL_EXT_blend_subtract'
      'GL_EXT_compiled_vertex_array'
      'GL_EXT_Cg_shader'
      'GL_EXT_depth_bounds_test'
      'GL_EXT_direct_state_access'
      'GL_EXT_draw_buffers2'
      'GL_EXT_draw_instanced'
      'GL_EXT_draw_range_elements'
      'GL_EXT_fog_coord'
      'GL_EXT_framebuffer_blit'
      'GL_EXT_framebuffer_multisample'
      'GL_EXTX_framebuffer_mixed_formats'
      'GL_EXT_framebuffer_multisample_blit_scaled'
      'GL_EXT_framebuffer_object'
      'GL_EXT_framebuffer_sRGB'
      'GL_EXT_geometry_shader4'
      'GL_EXT_gpu_program_parameters'
      'GL_EXT_gpu_shader4'
      'GL_EXT_multi_draw_arrays'
      'GL_EXT_packed_depth_stencil'
      'GL_EXT_packed_float'
      'GL_EXT_packed_pixels'
      'GL_EXT_pixel_buffer_object'
      'GL_EXT_point_parameters'
      'GL_EXT_polygon_offset_clamp')
    ParentFont = False
    TabOrder = 0
    OnClick = LBSCNClick
  end
  object ListBox1: TListBox
    Left = 192
    Top = 400
    Width = 737
    Height = 145
    Color = 14535609
    Font.Charset = DEFAULT_CHARSET
    Font.Color = 8404992
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ItemHeight = 13
    ParentFont = False
    TabOrder = 1
  end
  object Panel1: TPanel
    Left = 192
    Top = 64
    Width = 737
    Height = 337
    BevelInner = bvRaised
    BevelOuter = bvLowered
    TabOrder = 2
    object Image1: TImage
      Left = 496
      Top = 8
      Width = 225
      Height = 168
      Center = True
      Stretch = True
    end
    object Label8: TLabel
      Left = 8
      Top = 8
      Width = 12
      Height = 20
      Caption = '...'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -16
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
    end
  end
  object TrackBar1: TTrackBar
    Left = 304
    Top = 632
    Width = 361
    Height = 29
    Max = 100
    Min = 1
    Orientation = trHorizontal
    Frequency = 1
    Position = 90
    SelEnd = 0
    SelStart = 0
    TabOrder = 3
    TickMarks = tmBottomRight
    TickStyle = tsAuto
    Visible = False
    OnChange = TrackBar1Change
  end
  object ScrollBox1: TScrollBox
    Left = 8
    Top = 544
    Width = 921
    Height = 75
    TabOrder = 4
  end
  object Timer1: TTimer
    Enabled = False
    OnTimer = Timer1Timer
    Left = 24
    Top = 552
  end
end
