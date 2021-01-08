unit about;

interface

uses Windows, SysUtils, Classes, Graphics, Forms, Controls, StdCtrls,
  Buttons, ExtCtrls;

type
  TAboutBox = class(TForm)
    OKButton: TButton;
    Image1: TImage;
    Version: TLabel;
    procedure FormActivate(Sender: TObject);
  private
    { Private declarations }
  public
    { Public declarations }
  end;

  procedure showaboutbox(Sender: TObject);

implementation
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
end;

end.
 
