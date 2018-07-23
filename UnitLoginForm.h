//---------------------------------------------------------------------------

#ifndef UnitLoginFormH
#define UnitLoginFormH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.Imaging.jpeg.hpp>
//---------------------------------------------------------------------------
class TLoginForm : public TForm
{
__published:	// IDE-managed Components
	TEdit *Editusername;
	TButton *Buttonlogin;
	TLabel *Label1;
	TLabel *Label2;
	TEdit *Editpassword;
	TEdit *Editverify;
	TImage *Imageverify;
	TLabel *Labelverify;
	void __fastcall ButtonloginClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TLoginForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TLoginForm *LoginForm;
//---------------------------------------------------------------------------
#endif
