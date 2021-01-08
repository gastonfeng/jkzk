program sps;

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
  Application.Title := 'Solar charger';
  Application.CreateForm(TMainForm, MainForm);
  Application.CreateForm(TCoverForm, CoverForm);
  Application.CreateForm(Tsiteform, siteform);
  Application.Run;
end.
