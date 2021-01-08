unit Main;

interface

uses SysUtils, Windows, Messages, Classes, Graphics, Controls,
  Forms, Dialogs, StdCtrls, Buttons, ExtCtrls, Menus,  ToolWin,
   TeeProcs, Chart, iniFiles, CPDrv,Registry, TeEngine, TeeFunci, Series,
  Grids, ComCtrls;

  procedure sendreq(ch:byte);

type
  TMainForm = class(TForm)
    StatusLine: TStatusBar;
    ToolBar1: TToolBar;
    connectbtn: TSpeedButton;
    display: TPageControl;
    TabSheet1: TTabSheet;
    TabSheet2: TTabSheet;
    TabSheet3: TTabSheet;
    disconnectbtn: TSpeedButton;
    StringGrid1: TStringGrid;
    StringGrid2: TStringGrid;
    Chart1: TChart;
    Series2: TLineSeries;
    Series3: TLineSeries;
    Series4: TLineSeries;
    TeeFunction1: TAddTeeFunction;
    TeeFunction2: TAddTeeFunction;
    TeeFunction3: TAddTeeFunction;
    TeeFunction4: TAddTeeFunction;
    Series1: TLineSeries;
    exitbtn: TSpeedButton;
    aboutbtn: TSpeedButton;
    savebtn: TSpeedButton;
    SaveDialog1: TSaveDialog;
    CommPortDriver1: TCommPortDriver;
    refresh: TSpeedButton;
    Timer1: TTimer;
    procedure connect(Sender: TObject);
    procedure HelpAboutItemClick(Sender: TObject);
    procedure applycommsettings;
    procedure exitbtnClick(Sender: TObject);
    procedure disconnectbtnClick(Sender: TObject);
    procedure CommPortDriver1ReceivePacket(Sender: TObject;
      Packet: Pointer; DataSize: Cardinal);
    procedure FormActivate(Sender: TObject);
    procedure FormClose(Sender: TObject; var Action: TCloseAction);
    procedure savebtnClick(Sender: TObject);
    procedure refreshClick(Sender: TObject);
    procedure StringGrid1SelectCell(Sender: TObject; ACol, ARow: Integer;
      var CanSelect: Boolean);
    procedure Timer1Timer(Sender: TObject);
    procedure TabSheet1Show(Sender: TObject);
  end;

type variableitem=record
  site:byte;
  name:string;
  unitname:string;
  scale:integer;
  col,row:integer;
end;

 type now=record
     btv:smallint;
     sv:array[1..6]of smallint;
     loadv:smallint;
     temp:smallint;
     zz:smallint;
     loadi:smallint;
     bti:smallint;
     sri0:smallint;
     sri1:smallint;
     sri2:smallint;
     sri3:smallint;
     sri4:smallint;
     sri5:smallint;
     maxvoltage:smallint;
     minvoltage:smallint;
     chargeah:longint;
     dischargeah:longint;
     min:shortint;
     hour:shortint;
     status:shortint;
     load:shortint;
     BOOST:smallint;
     TAPER:smallint;
     FLOAT_MAX_S:smallint;
     FLOAT_MIN_S:smallint;
     BOOST_BELOW:smallint;
     COMP_TEMP:smallint;
     DELAY_CHANGE:smallint;
     BT_MIN:smallint;
     BT_RELOAD:smallint;
     BT_MAX:smallint;
     DELAY_DISCONNECT:smallint;
     LOCAL:smallint;
     eHOUR:smallint;
     MINUTE:smallint;
     AUTO_UPLOAD:smallint;
     UP_HOUR:smallint;
     UP_MIN:smallint;
     checksum:smallint;
end;
var
  MainForm: TMainForm;
  inifile:TIniFile;
//  RECeIVEFLAG:integer;
  Connected:boolean;
  autostarted,MODEMCONNECT:boolean;
  mypath:string;
//  nowrec:^now;
  timeout:integer;
  req:boolean;
  ch:byte;
  variablelist:array[1..100] of variableitem;
  sending:boolean;
  alarming:boolean;
  INCHINA:boolean;

CONST
     DIAL=1;
     ASKSITE=2;
     CURRENT=3;
     HISTORY=4;
     SETUPDATA=5;
     INIT=0;
     timeoutvalue=100;

implementation
{$R *.DFM}

uses SiteUnit, about, CoverUnit, modi;

procedure TMainForm.connect(Sender: TObject);
begin
    siteform:=tsiteform.create(application);
    siteform.ShowModal;
end;


procedure TMainForm.HelpAboutItemClick(Sender: TObject);
begin
  showaboutbox(application);
//  AboutBox:=taboutbox.create(application);
//  AboutBox.showmodal;
end;

procedure tmainform.applycommsettings;
begin
//  inifile:=Tinifile.create(mypath+'SPS.ini');
//  with inifile do
//  begin
    commportdriver1.port:=tportnumber(siteform.portcombobox.itemindex+1);//inifile.readinteger('PORT','COM',1)+1);
    commportdriver1.BaudRate:=tbaudrate(7);//inifile.readinteger('PORT','BAUDRATE',12)+1);
//  end;
//  inifile.free;
  commportdriver1.databits:=db8bits;
  commportdriver1.parity:=ptnone;
  commportdriver1.hwflow:=hfnone;
  commportdriver1.swflow:=sfnone;
  commportdriver1.CheckLineStatus:=TRUE;
  commportdriver1.EnableDTROnOpen:=true;
end;

procedure TMainForm.exitbtnClick(Sender: TObject);
begin
disconnectbtnclick(application);
close;
end;


procedure TMainForm.disconnectbtnClick(Sender: TObject);
begin
//     commportdriver1.SendChar('3');
  timer1.Enabled:=false;
     commportdriver1.Disconnect;
       connectbtn.Enabled:=true;
       disconnectbtn.enabled:=false;
//     SETUPBtn.Enabled:=true;
       connected:=false;
       refresh.Enabled:=false;
end;

//串口数据包接收，每包为5个字节
//byte1:start code=0xaa
//byte2:opertion code
//byte3,4:data word,high byte on back
//byte5:checksum,four byte addition
procedure TMainForm.CommPortDriver1ReceivePacket(Sender: TObject;
  Packet: Pointer; DataSize: Cardinal);

  type tresult=record
    begincode:byte;
    varicode:byte;
    variable:smallint;
    checksum:byte;
  end;
  type bb=record //array[1..5] of byte;
    b1,b2,b3,b4,b5:byte;
  end;
var
  p:^tresult;
  bbb:^bb;
  te:byte;
begin
//     messagebeep(0);
     if Datasize<>5 then
     begin
//       reply:=true;
       exit;
     end;
  bbb:=Packet;
//     te:=bbb[4]+bbb[1]+bbb[2]+bbb[3];
     te:=byte(bbb.b1+bbb.b2+bbb.b3+bbb.b4);
     if te<>bbb.b5 then exit;
     p:=Packet;
     if p.varicode=$fe then                //Alarm signal
     begin
       case p.variable of
         0:
         begin
           Statusline.Color:=clbtnFace;
           if INCHINA then Statusline.SimpleText:='蓄电池电压正常'
             else Statusline.SimpleText:='Battery Normal';
           alarming:=false;
         end;
         1:
         begin
           Statusline.Color:=clRed;
           if INCHINA then Statusline.SimpleText:='蓄电池电压过高！'
             else Statusline.SimpleText:='Battery voltage HIGH!';
           messagebeep(MB_OK);
         end;
         2:
         begin
           Statusline.Color:=clRed;
           if INCHINA then Statusline.SimpleText:='蓄电池欠电压！'
             else Statusline.SimpleText:='Battery Voltage LOW!';
           messagebeep(MB_OK);
         end;
       end;
     end;
     if p.varicode>60 then                 //History data
     begin
       stringgrid2.Cells[((p.varicode-68) mod 4)+1,(p.varicode - 64) div 4]:=floattostr(p.variable/10);
       if (p.varicode-68)mod 4=0 then series1.Add(p.variable/10,'',clteecolor);
       if (p.varicode-68)mod 4=1 then series2.Add(p.variable/10,'',clteecolor);
       if (p.varicode-68)mod 4=2 then series3.Add(p.variable/10,'',clteecolor);
       if (p.varicode-68)mod 4=3 then series4.Add(p.variable/10,'',clteecolor);
       if p.varicode=195 then savebtn.Enabled:=true;
       exit;
     end;
     for te:=1 to 100 do
     begin
       if variablelist[te].site=p.varicode then break;
       if variablelist[te].site=$ff then break;
     end;
     if te<100 then
     stringgrid1.Cells[variablelist[te].col+2,variablelist[te].row]:=floattostr(p.variable/variablelist[te].scale);
 //    case p.varicode of
 //      0..21:stringgrid1.Cells[1,p.varicode+1]:=inttostr(p.variable);
 //      $20..$2e:stringgrid1.Cells[4,p.varicode-$20]:=inttostr(p.variable);
 //      67..127:stringgrid2.Cells[p.varicode mod 4,p.varicode div 4]:=inttostr(p.variable);
//     end;
     inc(ch);
     if variablelist[ch].site=$ff then
     begin
       timer1.Enabled:=false;
       ch:=1;
       sendreq(67);
     end;
     req:=false;
end;

procedure TMainForm.FormActivate(Sender: TObject);
var
  i:integer;
  Reg:TRegistry;
begin
  mypath:=extractfilepath(application.exename);
  Reg:=TRegistry.Create;
  Reg.RootKey:=HKEY_CURRENT_USER;
  Reg.OpenKey('\Software\Borland\Locales',True);
  INCHINA:=False;
//  mypath:=Reg.ReadString('');
  if Reg.Readstring(application.exename)='chs' then INCHINA:=true;
  INCHINA:=false;
  Reg.Free;
  if INCHINA then
  begin
    stringgrid1.Cells[0,0]:='参数名称';
    stringgrid1.Cells[1,0]:='单位';
    stringgrid1.Cells[2,0]:='数值';
    stringgrid1.Cells[4,0]:='设置参数名称';
    stringgrid1.Cells[5,0]:='单位';
    stringgrid1.Cells[6,0]:='数值(单击进行修改)';
    stringgrid2.Cells[0,0]:='距今日天数';
    stringgrid2.Cells[1,0]:='当日最高电压(V)';
    stringgrid2.Cells[2,0]:='当日最低电压(V)';
    stringgrid2.Cells[3,0]:='当日充电电量(AH)';
    stringgrid2.Cells[4,0]:='当日放电电量(AH)';
  end
  else
  begin
    stringgrid1.Cells[0,0]:='Name';
    stringgrid1.Cells[1,0]:='Unit';
    stringgrid1.Cells[2,0]:='Value';
    stringgrid1.Cells[4,0]:='Setup';
    stringgrid1.Cells[5,0]:='Unit';
    stringgrid1.Cells[6,0]:='Value';
    stringgrid2.Cells[0,0]:='Date';
    stringgrid2.Cells[1,0]:='Maximum Voltage(V)';
    stringgrid2.Cells[2,0]:='Minimum Voltage(V)';
    stringgrid2.Cells[3,0]:='Charge(AH)';
    stringgrid2.Cells[4,0]:='Discharge(AH)';
  end;
  for i := 1 to 32 do
  begin
    stringgrid2.Cells[0,i]:=inttostr(i-1);
  end;
//  mainform.StringGrid1.Cells[0,1]:='蓄电池电压';
//  stringgrid1.Cells[1,1]:='V';
//  mainform.StringGrid1.Cells[0,2]:='负载电压';
//  mainform.StringGrid1.Cells[0,3]:='蓄电池温度';
//  mainform.StringGrid1.Cells[0,4]:='充电电流';
//  mainform.StringGrid1.Cells[0,5]:='放电电流';
//  mainform.StringGrid1.Cells[0,6]:='方阵1充电电流';
//  mainform.StringGrid1.Cells[0,7]:='方阵2充电电流';
//  mainform.StringGrid1.Cells[0,8]:='方阵3充电电流';
//  mainform.StringGrid1.Cells[0,9]:='方阵4充电电流';
//  mainform.StringGrid1.Cells[0,10]:='方阵5充电电流';
 // mainform.StringGrid1.Cells[0,11]:='方阵6充电电流';
//  mainform.StringGrid1.Cells[0,12]:='当天最高电压';
//  mainform.StringGrid1.Cells[0,13]:='当天最低电压';
 // mainform.StringGrid1.Cells[0,14]:='当天充电电量';
//  mainform.StringGrid1.Cells[0,15]:='当天放电电量';
//  mainform.StringGrid1.Cells[0,16]:='控制器时间';
//  mainform.StringGrid1.Cells[0,17]:='输出状态';
//  mainform.StringGrid1.Cells[0,18]:='充电状态';
//  stringgrid1.Cells[3,1]:='强充最大电压';
//  stringgrid1.Cells[3,2]:='强充递减电压';
//  stringgrid1.cells[3,3]:='浮充电压上限';
//  stringgrid1.Cells[3,4]:='浮充电压下限';
 // stringgrid1.Cells[3,5]:='返回强充电压';
//  stringgrid1.Cells[3,6]:='温度补偿电压';
//  stringgrid1.Cells[3,7]:='告警电压上限';
//  stringgrid1.Cells[3,8]:='告警电压下限';
//  stringgrid1.Cells[3,9]:='欠压告警解除电压';
//  stringgrid1.Cells[3,10]:='告警前延时';
//  stringgrid1.Cells[3,11]:='充电状态转换延时';
//  mainform.StringGrid2.Cells[0,1]:='天数';
//  mainform.StringGrid2.Cells[1,1]:='当天最高电压';
//  mainform.StringGrid2.Cells[2,1]:='当天最低电压';
//  mainform.StringGrid2.Cells[3,1]:='当天充电电量';
//  mainform.StringGrid2.Cells[4,1]:='当天放电电量';;
  if INCHINA then
  begin
    with tinifile.Create(mypath+'variable.ini') do
    begin
      for i:=1 to 100 do
      begin
        variablelist[i].site:=readinteger(inttostr(i),'SITE',$FF);
        if variablelist[i].site=$ff then break;
        variablelist[i].name:=readstring(inttostr(i),'VARIABLE NAME','');
        variablelist[i].unitname:=readstring(inttostr(i),'UNIT','');
        variablelist[i].scale:=readinteger(inttostr(i),'SCALE',1);
        variablelist[i].col:=readinteger(inttostr(i),'COL',0);
        variablelist[i].row:=readinteger(inttostr(i),'ROW',0);
        stringgrid1.cells[variablelist[i].col,variablelist[i].row]:=variablelist[i].name;
        stringgrid1.Cells[variablelist[i].col+1,variablelist[i].row]:=variablelist[i].unitname;
      end;
    free;
    end;
  end
  else
  begin
    with tinifile.Create(mypath+'evariable.ini') do
    begin
      for i:=1 to 100 do
      begin
        variablelist[i].site:=readinteger(inttostr(i),'SITE',$FF);
        if variablelist[i].site=$ff then break;
        variablelist[i].name:=readstring(inttostr(i),'VARIABLE NAME','');
        variablelist[i].unitname:=readstring(inttostr(i),'UNIT','');
        variablelist[i].scale:=readinteger(inttostr(i),'SCALE',1);
        variablelist[i].col:=readinteger(inttostr(i),'COL',0);
        variablelist[i].row:=readinteger(inttostr(i),'ROW',0);
        stringgrid1.cells[variablelist[i].col,variablelist[i].row]:=variablelist[i].name;
        stringgrid1.Cells[variablelist[i].col+1,variablelist[i].row]:=variablelist[i].unitname;
      end;
    free;
    end;
  end;
  CoverForm.Hide;
  CoverForm.free;
  ch:=1;
  sending:=false;
  with tinifile.Create(mypath+'SPS.INI') do
  begin
    autostarted:=readbool('START','START',TRUE);
    if autostarted then
    begin
      siteform:=tsiteform.create(application);
      siteform.ShowModal;
    end;
    free;
  end;
end;

procedure TMainForm.FormClose(Sender: TObject; var Action: TCloseAction);
begin
  with tinifile.Create(mypath+'SPS.INI') do
  begin
    writebool('START','START',autostarted);
    free;
  end;
end;

procedure TMainForm.savebtnClick(Sender: TObject);
var f1:Textfile;
    i:integer;
begin
  savedialog1.InitialDir:=mypath;
  if savedialog1.Execute then
  begin
    Assignfile(f1,savedialog1.filename);
    rewrite(f1);
    if INCHINA then writeln(f1,'<当前状态记录>')
      else writeln(f1,'<Current State>');
    for i:=1 to 100 do
    begin
      if variablelist[i].site=$ff then break;
      writeln(f1,variablelist[i].name+' = '+stringgrid1.Cells[variablelist[i].col+2,variablelist[i].row]+' '+variablelist[i].unitname);
    end;
//  writeln(f1,'连续运行时间:'+stringgrid1.cells[1,16]);
//  writeln(f1,'蓄电池电压 = '+stringgrid1.cells[1,1]);
//  writeln(f1,'蓄电池充电电流 = '+stringgrid1.cells[1,4]);
//  writeln(f1,'负载电压 = '+stringgrid1.cells[1,2]);
//  writeln(f1,'负载电流 = '+stringgrid1.cells[1,5]);
//  writeln(f1,'蓄电池温度 = '+stringgrid1.cells[1,3]);
//  writeln(f1,'输出状态 = '+stringgrid1.cells[1,17]);
//  writeln(f1,'充电状态 = '+stringgrid1.cells[1,18]);
//  writeln(f1,'太阳能电池第1组电流 = '+stringgrid1.cells[1,6]);
//  writeln(f1,'太阳能电池第2组电流 = '+stringgrid1.cells[1,7]);
//  writeln(f1,'太阳能电池第3组电流 = '+stringgrid1.cells[1,8]);
//  writeln(f1,'太阳能电池第4组电流 = '+stringgrid1.cells[1,9]);
//  writeln(f1,'太阳能电池第5组电流 = '+stringgrid1.cells[1,10]);
//  writeln(f1,'太阳能电池第6组电流 = '+stringgrid1.cells[1,11]);
//  writeln(f1,'当日累计充电电量 = '+stringgrid1.cells[1,14]);
//  writeln(f1,'当日累计放电电量 = '+stringgrid1.cells[1,15]);
//  writeln(f1,'当日蓄电池最高电压 = '+stringgrid1.cells[1,12]);
//  writeln(f1,'当日蓄电池最低电压 = '+stringgrid1.cells[1,13]);
//  writeln(f1,'<设置状态记录>');
//  writeln(f1,'强充最大电压 = '+stringgrid1.cells[4,1]);
//  writeln(f1,'强充递减电压 = '+stringgrid1.cells[4,2]);
//  writeln(f1,'浮充电压上限 = '+stringgrid1.cells[4,3]);
//  writeln(f1,'浮充电压下限 = '+stringgrid1.cells[4,4]);
//  writeln(f1,'返回强充电压 = '+stringgrid1.cells[4,5]);
//  writeln(f1,'温度补偿电压 = '+stringgrid1.cells[4,6]);
//  writeln(f1,'告警电压上限 = '+stringgrid1.cells[4,7]);
//  writeln(f1,'告警电压下限 = '+stringgrid1.cells[4,8]);
//  writeln(f1,'负载重连接电压 = '+stringgrid1.cells[4,9]);
//  writeln(f1,'断开负载前延时 = '+stringgrid1.cells[4,10]);
//  writeln(f1,'状态转换延时 = '+stringgrid1.cells[4,11]);
    writeln(f1,'');
    if INCHINA then writeln(f1,'<历史数据记录>')
      else writeln(f1,'<History>');
    if INCHINA then writeln(f1,'天数'+chr(9)+chr(9)+'最高电压'+chr(9)+chr(9)+'最低电压'+chr(9)+chr(9)+'充电电量'+chr(9)+chr(9)+'放电电量')
    else writeln(f1,'DATE'+chr(9)+chr(9)+'MAX. Voltage'+chr(9)+chr(9)+'MIN. Voltage'+chr(9)+chr(9)+'Charge'+chr(9)+chr(9)+'Discharge');
	for i:=0 to 31 do
	if INCHINA then writeln(f1,'第'+inttostr(i)+'天'+chr(9)+chr(9)+mainform.StringGrid2.cells[1,i+1]+'V'+chr(9)+chr(9)+mainform.StringGrid2.cells[2,i+1]+'V'+chr(9)+chr(9)+mainform.StringGrid2.cells[3,i+1]+'AH'+chr(9)+chr(9)+mainform.StringGrid2.cells[4,i+1]+'AH')
          else writeln(f1,inttostr(i)+'day'+chr(9)+chr(9)+mainform.StringGrid2.cells[1,i+1]+'V'+chr(9)+chr(9)+mainform.StringGrid2.cells[2,i+1]+'V'+chr(9)+chr(9)+mainform.StringGrid2.cells[3,i+1]+'AH'+chr(9)+chr(9)+mainform.StringGrid2.cells[4,i+1]+'AH');
    closefile(f1);
  end;
end;

procedure TMainForm.StringGrid1SelectCell(Sender: TObject; ACol,
  ARow: Integer; var CanSelect: Boolean);
begin
  if ACol<>6 then
  begin
    if INCHINA then showmessage('此字段不可更改！')
    else showmessage('This field not allow modify.');
  end
  else
  begin
    modiform:=tmodiform.Create(Application);
    if INCHINA then modiform.label1.Caption:='当前变量:'+stringgrid1.Cells[ACol-2,ARow]
    else modiform.label1.Caption:=stringgrid1.Cells[ACol-2,ARow];
    if INCHINA then modiform.label2.Caption:='当前值:'+stringgrid1.Cells[Acol,ARow]+stringgrid1.cells[ACol-1,ARow]
    else modiform.label2.Caption:='Current:'+stringgrid1.Cells[Acol,ARow]+stringgrid1.cells[ACol-1,ARow];
    modiform.edit1.text:=stringgrid1.Cells[Acol,ARow];
    modiform.tag:=ARow;
    timer1.Enabled:=false;
    modiform.showmodal;
  end;
end;

procedure sendreq(ch:byte);
var
  checksum:byte;
begin
  while sending=true do ;
  sending:=true;
  mainform.CommPortDriver1.SendByte($aa);
//  checksum:=$aa;
  mainform.CommPortDriver1.SendByte(1);
  checksum:=$ab;
  mainform.CommPortDriver1.SendByte(ch);
  checksum:=checksum+ch;
  mainform.CommPortDriver1.SendByte(0);
  mainform.CommPortDriver1.SendByte(0);
  mainform.CommPortDriver1.SendByte(checksum);
  sending:=false;
end;

procedure TMainForm.refreshClick(Sender: TObject);
begin
  if Connected then
  begin
    ch:=1;
    timer1.Enabled:=true;
    series1.Clear;
    series2.Clear;
    series3.Clear;
    series4.Clear;
//    req:=true;
//    sendreq(67);
  end;
end;


procedure TMainForm.Timer1Timer(Sender: TObject);
begin
  if Connected then
  begin
    if req=true then
    begin
      inc(timeout);
      if timeout>timeoutvalue then
      begin
//	showmessage('Timeout!');
//	if INCHINA then mainform.StatusLine.SimpleText:='超时错误.'
//        else mainform.StatusLine.SimpleText:='Communicate timeout.';
	req:=false;
	timeout:=0;
      end;
    end
    else
    begin
      sendreq(variablelist[ch].site);
      req:=true;
//      mainform.StatusLine.SimpleText:=pchar(variablelist[ch].name);
      timeout:=0;
    end;
  end;
end;

procedure TMainForm.TabSheet1Show(Sender: TObject);
begin
//  timer1.Enabled:=true;
end;

end.
