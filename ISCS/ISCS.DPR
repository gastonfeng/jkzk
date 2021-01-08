program ISCS;

uses
  Forms,
  main in 'main.pas' {MainForm},
  about in 'about.pas' {AboutBox},
  SiteUnit in 'SiteUnit.pas' {siteform},
  CoverUnit in 'CoverUnit.pas' {CoverForm},
  modi in 'modi.pas' {modiform};

{$R *.RES}

begin
  CoverForm:=TCoverForm.Create(Application);
  CoverForm.Show;
  CoverForm.Update;
  Application.Title := 'Intelligent Solar Charge Controller';
  Application.CreateForm(TMainForm, MainForm);
  Application.Run;
end.
