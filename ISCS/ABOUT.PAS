unit about;

interface

uses Windows, SysUtils, Classes, Graphics, Forms, Controls, StdCtrls,
  Buttons, ExtCtrls,Registry;

type
  TAboutBox = class(TForm)
    OKButton: TButton;
    Image1: TImage;
    Version: TLabel;
    Button1: TButton;
    procedure FormActivate(Sender: TObject);
    procedure Button1Click(Sender: TObject);
  private
    { Private declarations }
  public
    { Public declarations }
  end;

var
  AboutBox:TAboutBox;
    
  procedure showaboutbox(Sender: TObject);

implementation

uses main;
{$R *.DFM}

procedure showaboutbox(Sender: TObject);
begin
  with taboutbox.create(application) do
    try
      showmodal;
    finally
      free;
  end;
end;
      
function GetVersion(filename:string):string;
var
  InfoSize, Wnd: DWORD;
  VerBuf: Pointer;
  szName: array[0..255] of Char;
  Value: Pointer;
  Len: UINT;
  TransString:string;
begin
  InfoSize := GetFileVersionInfoSize(PChar(FileName), Wnd);
  if InfoSize <> 0 then
  begin
    GetMem(VerBuf, InfoSize);
    try
      if GetFileVersionInfo(PChar(FileName), Wnd, InfoSize, VerBuf) then
      begin
        Value :=nil;
        VerQueryValue(VerBuf, '\VarFileInfo\Translation', Value, Len);
        if Value <> nil then
           TransString := IntToHex(MakeLong(HiWord(Longint(Value^)), LoWord(Longint(Value^))), 8);
        Result := '';
        StrPCopy(szName, '\StringFileInfo\'+Transstring+'\FileVersion');
        if VerQueryValue(VerBuf, szName, Value, Len) then
           Result := StrPas(PChar(Value));
      end;
    finally
      FreeMem(VerBuf);
    end;
  end;
end;


procedure TAboutBox.FormActivate(Sender: TObject);
begin
  version.Caption:='Version:'+getversion(application.exename);
  if INCHINA then Button1.Caption:='ENG'
    else Button1.Caption:='CHS';
end;

procedure TAboutBox.Button1Click(Sender: TObject);
var
  Reg:TRegistry;
begin
  Reg:=TRegistry.Create;
  Reg.RootKey:=HKEY_CURRENT_USER;
  Reg.OpenKey('\Software\Borland\Locales',True);
  if INCHINA then Reg.WriteString(Application.exename,'eng')
    else Reg.WriteString(Application.exename,'chs');
  Reg.Free;  
end;

end.

