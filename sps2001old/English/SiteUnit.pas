unit SiteUnit;

interface

uses   SysUtils, Windows, Messages, Classes, Graphics, Controls,
  StdCtrls, Forms, DBCtrls, DB, DBGrids, DBTables, Grids, ExtCtrls,Dialogs,inifiles;

type
  Tsiteform = class(TForm)
    telephone: TComboBox;
    autostart: TCheckBox;
    teleradio: TRadioButton;
    numberradio: TRadioButton;
    GroupBox1: TGroupBox;
    inok: TButton;
    incancel: TButton;
    portcombobox: TComboBox;
    Label1: TLabel;
    procedure inokClick(Sender: TObject);
    procedure incancelClick(Sender: TObject);
    procedure FormActivate(Sender: TObject);
    procedure FormClose(Sender: TObject; var Action: TCloseAction);
    procedure telephoneChange(Sender: TObject);
    procedure autostartClick(Sender: TObject);
    procedure teleradioClick(Sender: TObject);
  private
    { private declarations }
  public
    { public declarations }
  end;

var
  siteform: Tsiteform;
implementation
{$R *.DFM}
uses main;

procedure Tsiteform.inokClick(Sender: TObject);
var
   ss:pchar;//string[20];
//   tt:char;
begin
  mainform.applycommsettings;
  if mainform.commportdriver1.connect then
  begin
    inifile:=TIniFile.Create(mypath+'SPS.ini');
    inifile.writeinteger('PORT','COM',portcombobox.itemindex);
    inifile.free;
    if mainform.commportdriver1.getlinestatus=[] then mainform.commportdriver1.checklinestatus:=true;
    if INCHINA then mainform.statusline.simpletext:='已连接'
    else mainform.statusline.simpletext:='Connected';
    if siteform.teleradio.Checked then
    begin
      mainform.CommPortDriver1.ToggleDTR(true);
      mainform.CommPortDriver1.ToggleRTS(true);
//      ss:='abcdeghijklmnopqrstuvwxyz';
      mainform.CommPortdriver1.InputTimeout:=3000;
      mainform.CommPortdriver1.PausePolling;
      mainform.CommPortDriver1.FlushBuffers(true,true);
      mainform.CommPortDriver1.SendString('AT&F'+#13);
//      ss:='';
      mainform.CommPortdriver1.ReadData(ss,8);
//      ss[1]:='O';
//      ss[2]:='K';
      //if strlicomp(@ss[1],'AT&F'+#$d+#$a+#$D+#$A+'OK',9)=0 then
//      ss[0]:=9;
      if strpos(ss,'AT&F')<>nil then
      begin
        if INCHINA then mainform.StatusLine.SimpleText:='正在拨号,请等待...'
        else mainform.StatusLine.SimpleText:='Dialing...';
//        ss:='';
        mainform.CommPortDriver1.FlushBuffers(true,false);
        mainform.CommPortDriver1.InputTimeout:=5000;
        mainform.CommPortdriver1.SendString('ATDT'+siteform.telephone.Text+#13);
        mainform.CommPortdriver1.ReadData(ss,15);
        mainform.CommPortdriver1.InputTimeout:=60000;
        mainform.CommPortDriver1.FlushBuffers(true,true);
        mainform.CommPortdriver1.ReadData(ss,12);
        mainform.CommPortDriver1.ContinuePolling;
//        if strlicomp(@ss[1],#$D+#$A+'CONNECT',9)=0 then
        if strpos(ss,'CONN')<>nil then
        begin
          if INCHINA then mainform.StatusLine.SimpleText:='成功建立连接!'
          else mainform.StatusLine.SimpleText:='Line Connected.';
          connected:=true;
//          mainform.CommPortDriver1.Sendbyte(255);
//          mainform.commportdriver1.readchar(tt);
//          if tt='K' then
//          begin
            mainform.connectbtn.enabled:=false;
            mainform.disconnectbtn.enabled:=true;
            mainform.refresh.Enabled:=true;
            mainform.Timer1.Enabled:=true;
            siteform.ModalResult:=1;
//          end
//          else messagedlg('未能与控制器建立连接!',mterror,[mbok],0);
        end
        else messagedlg('Can''t Connect.'+#13+#10+ss,mterror,[mbok],0);
      end
      else messagedlg('MODEM no response.',mterror,[mbok],0);
    end
    else
    begin
//      mainform.CommPortDriver1.Sendbyte(strtoint(number.text));
//      mainform.CommPortDriver1.ReadChar(tt);
//      if tt='K' then
//      begin
	connected:=true;
        mainform.connectbtn.enabled:=false;
        mainform.disconnectbtn.enabled:=true;
	mainform.refresh.Enabled:=true;
	mainform.Timer1.Enabled:=true;
        siteform.ModalResult:=1;
//      end
//      else messagedlg('未能与控制器建立连接!',mterror,[mbok],0);
    end;
  end
  else
  begin
    messagebeep(0);
    if INCHINA then MessageDlg('串行口未打开.请检查串行口设置,然后重试.',mtError,[mbOK],0)
    else MessageDlg('Comport Error!',mtError,[mbOK],0);
  end;
end;

procedure Tsiteform.incancelClick(Sender: TObject);
begin
  siteform.ModalResult:=1;
end;

procedure Tsiteform.FormActivate(Sender: TObject);
begin
  with tinifile.create(mypath+'sps.ini') do
  begin
    readsection('telephone',telephone.items);
//    readsection('sitenumber',number.items);
    portcombobox.ItemIndex:=readinteger('PORT','COM',1);
    free;
  end;
  telephone.itemindex:=telephone.Items.Count-1;
//  number.itemindex:=number.Items.count-1;
  autostart.Checked:=autostarted;
end;

procedure Tsiteform.FormClose(Sender: TObject; var Action: TCloseAction);
var i:integer;
    ff:boolean;
begin
  if connected then
  begin
    with tinifile.create(mypath+'sps.ini') do
    begin
      ff:=true;
      for i:=0 to telephone.Items.Count do
      begin
        if telephone.Items[i]=telephone.Text then ff:=false;
      end;
      if ff then writestring('telephone',telephone.text,'');
//      ff:=true;
//      for i:=0 to number.Items.count do
//      begin
//        if number.Items[i]=number.Text then ff:=false;
//      end;
//      if ff then writestring('sitenumber',number.text,'');
      free;
    end;
  end;
end;

procedure Tsiteform.telephoneChange(Sender: TObject);
begin
  teleradio.Checked:=true;
end;

procedure Tsiteform.autostartClick(Sender: TObject);
begin
  autostarted:=autostart.checked;
end;

procedure Tsiteform.teleradioClick(Sender: TObject);
begin
  telephone.Enabled:=true;
end;

end.
