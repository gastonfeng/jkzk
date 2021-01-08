unit modi;

interface

uses
  Windows, Messages, SysUtils, Classes, Graphics, Controls, Forms, Dialogs,
  StdCtrls, Buttons,grids;

type
  Tmodiform = class(TForm)
    Label1: TLabel;
    Label2: TLabel;
    Label3: TLabel;
    BitBtn1: TBitBtn;
    BitBtn2: TBitBtn;
    Edit1: TEdit;
    procedure BitBtn1Click(Sender: TObject);
    procedure BitBtn2Click(Sender: TObject);
  private
    { Private declarations }
  public
    { Public declarations }
  end;

var
  modiform: Tmodiform;
 
implementation

uses main;

{$R *.DFM}

procedure Tmodiform.BitBtn1Click(Sender: TObject);
var
  i:integer;
//  checksum:byte;
begin
  for i:=18 to 28 do
  begin
    if tag=variablelist[i].row then break;
  end;
  while sending=true do ;
  sending:=true;
  mainform.commportdriver1.sendbyte($aa);
  mainform.commportdriver1.sendbyte(2);
  mainform.commportdriver1.sendbyte(variablelist[i].site);
  mainform.commportdriver1.sendbyte(round((strtofloat(edit1.Text)*variablelist[i].scale)) div 256);
  mainform.commportdriver1.sendbyte(round((strtofloat(edit1.text)*variablelist[i].scale)) mod 256);
//  checksum:=$aa+2+variablelist[i].site+(strtoint(edit1.Text)*10 div 256)+(strtoint(edit1.text)*10 mod 256);
  mainform.commportdriver1.sendbyte(byte($aa+2+variablelist[i].site+(round(strtofloat(edit1.Text)*variablelist[i].scale) div 256)+(round(strtofloat(edit1.text)*variablelist[i].scale) mod 256)));
  sending:=false;
  modalresult:=1;
end;

procedure Tmodiform.BitBtn2Click(Sender: TObject);
begin
  modalresult:=1;
end;


end.
