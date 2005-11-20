unit options;

{
    Copyright (C) 2005 Olaf Klein, o.k.klein@t-online.de

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111 USA
}

interface

uses
  TypInfo, gnugettext, gnugettextDx,
  Windows, Messages, SysUtils, Classes, Graphics, Controls, Forms, Dialogs,
  StdCtrls, ExtCtrls, ComCtrls, Buttons,
  Common;

type
  TfrmOptions = class(TForm)
    pnBottom: TPanel;
    pnOptions: TPanel;
    btnOK: TBitBtn;
    btnCancel: TBitBtn;
    btnHelp: TBitBtn;
    mmWarning: TMemo;
    procedure CheckBoxClicked(Sender: TObject);
    procedure FormCreate(Sender: TObject);
    procedure btnHelpClick(Sender: TObject);
    procedure FormKeyDown(Sender: TObject; var Key: Word;
      Shift: TShiftState);
  private
    { Private declarations }
    FOpts: TStringList;
    FCanvas: TCanvas;
    FInitialOpts: string;
    procedure CreateStringOption(const x, y, tag: Integer; o: POption; xmax: Integer = -1);
    procedure CreateIntegerOption(const x, y, tag: Integer; o: POption; xmax: Integer = -1);
    procedure CreateFileOption(const x, y, tag: Integer; o: POption; IsInput: Boolean; xmax: Integer = -1);
    function FindUpDown(AControl: TControl): TUpDown;
    function GetOptsStr: string;
    procedure OptionOpenFile(Sender: TObject);
    procedure OptionSaveFile(Sender: TObject);
    function ParseOptsLine(const Line: string; List: TStrings): Integer;
    procedure SetOpts(AList: TStringList);
    procedure SetOptsStr(const AValue: string);
  public
    { Public declarations }
    FFormat: string;
    FIsInput: Boolean;
    constructor Create(AOwner: TComponent); override;
  property
    Opts: TStringList read FOpts write SetOpts;
    property OptsStr: string read GetOptsStr write SetOptsStr;
  end;

type
  eUnknownOption = class(Exception);
  eParserError = class(Exception);
  
var
  frmOptions: TfrmOptions;

implementation

uses
  utils, main;

{$R *.DFM}

constructor TfrmOptions.Create(AOwner: TComponent); // override;
begin
  inherited Create(AOwner);
  TranslateComponent(Self);
  FCanvas := Main.frmMain.stbMain.Canvas;
  mmWarning.Lines.Add(_('Be aware, that most options are made for the output side. '));
  mmWarning.Lines.Add(_('Currently we don''t have a flag which tells us which direction is used the options.'));
end;

procedure TfrmOptions.SetOpts(AList: TStringList);
var
  i, j: Integer;
  c: TComponent;
  wc: TControl;
  o: POption;
  chb: TCheckBox;
  xy: TPoint;
  xmax: Integer;
  lb: TLabel;
begin
  FOpts := AList;

  xy.x := 0;
  xy.y := 8;
  xmax := 0;

  for i := 0 to FOpts.Count - 1 do
  begin
    o := Pointer(FOpts.Objects[i]);
    o.chb := nil;
    o.edit := nil;

    if (FFormat = '') then
    begin
      FFormat := o.format;
      btnHelp.Hint := 'http://www.gpsbabel.org/readme.html#' + FFormat;
      btnHelp.ShowHint := True;
    end;

    if FIsInput and (
       (AnsiPos('generate ', o.hint) <> 0) or
       (AnsiPos(' generate', o.hint) <> 0) or
       (AnsiPos('output ', o.hint) <> 0) or
       (AnsiPos(' output', o.hint) <> 0) or
       (AnsiPos('write', o.hint) <> 0) or
       (AnsiPos(' write', o.hint) <> 0)) then Continue;

    chb := TCheckBox.Create(nil);
    o.chb := chb;
    chb.Name := '___' + o.name;
    chb.OnClick := CheckBoxClicked;
    chb.Tag := i + 1;

    InsertComponent(chb);

    chb.ParentFont := False;
    chb.Font := Main.frmMain.stbMain.Font;
    chb.Left := 8;
    chb.Top := xy.y;
    chb.Caption := dgettext('Options', o.Hint);
    chb.Alignment := taRightJustify;
    chb.Checked := False;
    chb.Parent := pnOptions;

    chb.Hint := SysUtils.Format(_('Short "%s"'), [o.name]);
    chb.ShowHint := True;
    chb.Width := 21 + Trunc(FCanvas.TextWidth(chb.Caption) * 1.25);

    xy.y := xy.y + chb.Height + 8;
    if (o.otype <> 4) then
      if (chb.Width > xy.x) then xy.x := chb.Width;
    if (chb.Width > xy.x) then
      xmax := chb.Width;
  end;

  xy.y := 8;
  xy.X := xy.X + 8;
  if (xy.X < 42) then xy.X := 42;

  for i := 0 to FOpts.Count - 1 do
  begin
    o := Pointer(FOpts.Objects[i]);
    o.edit := nil;
    
    if (o.chb = nil) then Continue;

    // ('unknown', 'integer', 'float', 'string', 'boolean', 'file', 'outfile');
    case o.otype of
      1: CreateIntegerOption(xy.X, xy.Y - 2, i + 1, o, xmax);
      2, 3: CreateStringOption(xy.X, xy.Y - 2, i + 1, o, xmax);
      4: ;
      5: if FIsInput then CreateFileOption(xy.X, xy.Y - 2, i + 1, o, True, xmax);
      6: if not FIsInput then CreateFileOption(xy.X, xy.Y - 2, i + 1, o, False, xmax);
    end;
    if (o.edit <> nil) then
      o.edit.Enabled := False;
    xy.y := xy.y + chb.Height + 8;
  end;

  xy.X := 0;
  xy.Y := 0;

  for i := 0 to ComponentCount - 1 do
  begin
    c := Components[i];
    if not c.InheritsFrom(TControl) then Continue;
    if (c is TPanel) then Continue;
    wc := Pointer(c);
    if (wc.Parent <> pnOptions) then Continue;

    j := wc.Left + wc.Width;
    if (j > xy.X) then xy.X := j;
    j := wc.Top + wc.Height;
    if (j > xy.Y) then xy.Y := j;
    if ( wc.Name = '' ) then Continue;
  end;

  Self.Width := xy.X + 8 + (Self.Width - Self.ClientWidth);
  Self.Height := xy.Y + 8 +
    mmWarning.Height + + pnBottom.Height +
    (Self.Height - Self.ClientHeight);

  i := btnCancel.Left - btnOK.Left;
  btnCancel.Left := pnBottom.Width - btnCancel.Width - btnHelp.Left;
  btnOK.Left := btnCancel.Left - i;
end;

function TfrmOptions.GetOptsStr: string;
var
  i: Integer;
  o: POption;
  c: TComponent;
  s: string;
begin
  Result := '';
  for i := 0 to FOpts.Count - 1 do
  begin
    o := Pointer(FOpts.Objects[i]);
    if (o.chb = nil) or not(o.chb.Checked) then Continue;
    if (Result <> '') then
      Result := Result + ',';
    Result := Result + o.name;
    if (o.edit = nil) then Continue;
    s := GetStrProp(o.edit, 'Text');
    if (Pos(' ', s) <> 0) or (Pos('"', s) <> 0) or (Pos(',', s) <> 0) then
      s := SysUtils.AnsiQuotedStr(s, '"');
    Result := SysUtils.Format('%s=%s', [Result, s]);
  end;
end;

procedure TfrmOptions.SetOptsStr(const AValue: string);
var
  l: TStrings;
  i, j: Integer;
  s, name, value: string;
  o: POption;
  ud: TUpDown;
begin
  l := TStringList.Create;
  try

    try
      ParseOptsLine(AValue, l);
    except
      on E: exception do
        raise eParserError.Create(_('Invalid line format!'));
    end;

    for i := 0 to l.Count - 1 do
    begin
      s := l.Strings[i];
      j := Pos('=', s);
      if (j > 0) then
      begin
        name := Copy(s, 1, j - 1);
        value := Copy(s, j + 1, Length(s) - j);
      end
        else
      begin
        Name := s;
        Value := '';
      end;
      if (name = '') then Continue;

      j := FOpts.IndexOf(name);
      if (j < 0) then
        raise eUnknownOption.CreateFmt(_('Unknown option "%s"!'), [name]);

      o := Pointer(FOpts.Objects[j]);
      if (o.edit <> nil) then
      begin
        o.chb.Checked := True;
        ud := FindUpDown(o.Edit);
        if (ud <> nil) then
          ud.Position := StrToInt(Value)
        else
          SetStrProp(o.edit, 'Text', Value);
      end
      else if (o.otype = 4) then
        o.chb.Checked := (value = '') or (value <> '0');
    end;
  finally
    l.Free;
  end;
  FInitialOpts := GetOptsStr;
end;

procedure TfrmOptions.CheckBoxClicked(Sender: TObject);
var
  i: Integer;
  c: TComponent;
  chb: TCheckBox;
  ctrl: TWinControl;
begin
  if (Sender = nil) or not (Sender is TCheckBox) then Exit;
  chb := Pointer(Sender);

  for i := 0 to ComponentCount - 1 do
  begin
    c := Components[i];
    if (c = chb) or not(c.InheritsFrom(TWinControl)) then Continue;
    if (c.Tag <> chb.Tag) then Continue;
    ctrl := Pointer(c);
    ctrl.Enabled := chb.Checked;
  end;
end;

procedure TfrmOptions.CreateStringOption(const x, y, tag: Integer; o: POption; xmax: Integer);
var
  ed: TEdit;
begin
  ed := TEdit.Create(Self);
  o.edit := ed;

  ed.Left := x;
  ed.Top := y;
  ed.Tag := tag;
  ed.Parent := pnOptions;

  if (o.def <> nil) then
    ed.Text := string(o.def);
end;

procedure TfrmOptions.CreateIntegerOption(const x, y, tag: Integer; o: POption; xmax: Integer);
var
  ed: TEdit;
  cb: TComboBox;
  up: TUpDown;
  i:  Integer;
begin
  if (o.min <> nil) and (o.max <> nil) and
     ((StrToInt(o.max) - StrToInt(o.min)) < 32) then
  begin
    cb := TComboBox.Create(Self);
    o.edit := cb;
    cb.Left := x;
    cb.Top := y;
    cb.Tag := tag;
    if (cb.Left + cb.Width < xmax) then
      cb.Left := xmax - cb.Width;
    cb.Parent := pnOptions;

    for i := StrToInt(o.min) to StrToInt(o.max) do
      cb.Items.Add(IntToStr(i));
    if (o.def <> nil) then
      cb.Text := o.def
    else
      cb.ItemIndex := 0;
    Exit;
  end;

  ed := TEdit.Create(Self);
  o.edit := ed;

  ed.Left := x;
  ed.Top := y;
  ed.Tag := tag;
  ed.Parent := pnOptions;

  up := TUpDown.Create(Self);
  up.Parent := pnOptions;

  ed.Width := ed.Width - up.Width;
  up.Left := ed.Left + ed.Width;
  up.Top := ed.Top;
  if (o.min <> nil) then
    up.Min := StrToInt(o.min)
  else
    up.Min := -(MAXSHORT-1);
  if (o.max <> nil) then
    up.Max := StrToInt(o.max)
  else
    up.Max := MAXSHORT;
  if (o.def <> nil) then
    up.Position := StrToInt(o.def);
  up.Associate := ed;
end;

procedure TfrmOptions.FormCreate(Sender: TObject);
begin
  TranslateComponent(Self);
end;

procedure TfrmOptions.btnHelpClick(Sender: TObject);
begin
  WinOpenFile('http://www.gpsbabel.org/readme.html#' + FFormat);
end;

procedure TfrmOptions.CreateFileOption(const x, y, tag: Integer; o: POption; IsInput: Boolean; xmax: Integer = -1);
var
  ed: TEdit;
  btn: TSpeedButton;
begin
  ed := TEdit.Create(Self);
  o.edit := ed;

  ed.Left := x;
  ed.Top := y;
  ed.Tag := tag;
  ed.Parent := pnOptions;

  btn := TSpeedButton.Create(Self);
  btn.Parent := pnOptions;
  btn.Tag := tag;
  ed.Width := ed.Width - btn.Width;
  btn.Left := ed.Left + ed.Width;
  btn.Top := ed.top;

  if IsInput then
  begin
    btn.OnClick := Self.OptionOpenFile;
    frmMain.ImageList1.GetBitmap(15, btn.Glyph);
  end
    else
  begin
    btn.OnClick := Self.OptionSaveFile;
    frmMain.ImageList1.GetBitmap(17, btn.Glyph);
  end;
end;

procedure TfrmOptions.OptionOpenFile(Sender: TObject);
var
  c: TControl;
  i: Integer;
  o: POption;
  d: TOpenDialog;
begin
  if (Sender = nil) or not(Sender is TControl) then Exit;

  c := Pointer(Sender);

  for i := 0 to FOpts.Count - 1 do
  begin
    o := Pointer(FOpts.Objects[i]);
    if (o.chb = nil) or (o.chb.Tag <> c.Tag) then Continue;

    d := TOpenDialog.Create(Self);
    try
      d.FileName := GetStrProp(o.edit, 'Text');
      if d.Execute then
        SetStrProp(o.edit, 'Text', d.FileName);
    finally
      d.Free;
    end;
  end;
end;

procedure TfrmOptions.OptionSaveFile(Sender: TObject);
var
  c: TControl;
  i: Integer;
  o: POption;
  d: TSaveDialog;
begin
  if (Sender = nil) or not(Sender is TControl) then Exit;
  
  c := Pointer(Sender);

  for i := 0 to FOpts.Count - 1 do
  begin
    o := Pointer(FOpts.Objects[i]);
    if (o.chb = nil) or (o.chb.Tag <> c.Tag) then Continue;

    d := TSaveDialog.Create(Self);
    try
      d.FileName := GetStrProp(o.edit, 'Text');
      if d.Execute then
        SetStrProp(o.edit, 'Text', d.FileName);
    finally
      d.Free;
    end;
  end;
end;

function TfrmOptions.ParseOptsLine(const Line: string; List: TStrings): Integer;
var
  s, name, val: string;
  cin, cend: PChar;
  c1, c2: PChar;
  ins: Boolean;
begin
  List.Clear;
  s := Trim(line) + ',';

  cin := PChar(s);
  cend := cin + StrLen(cin);

  while (cin < cend) do
  begin
    c1 := StrScan(cin, '=');
    c2 := StrScan(cin, ',');
    if (c1 > c2) then c1 := nil;

    if (c1 <> nil) then
    begin
      c1^ := #0;
      name := string(cin);
      val := '';

      c1 := c1 + 1;
      while (c1^ > #0) and (c1^ <= ' ') do c1 := c1 + 1;

      if (c1^ = '"') then // dequote
      begin
        while (c1 < cend) do
        begin
          c1 := c1 + 1;
          if (c1^ = '"') then
          begin
            if ((c1+1)^ = '"') then
              c1 := c1 + 1
            else
              Break;
          end;
          val := val + c1^;
        end;
        c2 := StrScan(c1 + 1, ',');
      end
        else
      begin
        c2^ := #0;
        val := string(c1);
      end;
    end
      else
    begin
      c2^ := #0;
      name := string(cin);
    end;

    if (name <> '') then
    begin
      if (val <> '') then
        list.Add(Format('%s=%s', [name, val]))
      else
        list.Add(name);
    end;

    if (c2 = nil) then
      Break
    else
      cin := c2 + 1;
  end;

  Result := List.Count;
end;

function TfrmOptions.FindUpDown(AControl: TControl): TUpDown;
var
  i: Integer;
  c: TComponent;
begin
  Result := nil;
  for i := 0 to ComponentCount - 1 do
  begin
    c := Components[i];
    if c.InheritsFrom(TUpDown) and (TUpDown(c).Associate = AControl) then
    begin
      Result := Pointer(c);
      Exit;
    end;
  end;
end;

procedure TfrmOptions.FormKeyDown(Sender: TObject; var Key: Word;
  Shift: TShiftState);
var
  str: string;
begin
  if (Key <> 27) then Exit;

  str := GetOptsStr;
  if (str <> FInitialOpts) then
  begin
    if not(MessageDlg(_('Discard changes?'), mtWarning, mbOKCancel, 0) = mrOK) then
      Exit; 
  end;
  ModalResult := mrCancel;
end;

end.
