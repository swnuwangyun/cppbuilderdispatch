//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "UnitLoginForm.h"
#include "FakeLogic.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TLoginForm *LoginForm;
//---------------------------------------------------------------------------
__fastcall TLoginForm::TLoginForm(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TLoginForm::ButtonloginClick(TObject *Sender)
{
	String username = this->Editusername->Text;
	String password = this->Editpassword->Text;

	struct handler {
		static void completion(TLoginForm *thiz, int status, String error, void* obj) {
			Screen->Cursor = crDefault;
			switch(status){
				case 0:
					Application->MessageBox(error.c_str(), L"cppbuilderdispatch", MB_OK);
					break;
				case 1:
					Application->MessageBox(L"Login succeed", L"cppbuilderdispatch", MB_OK);
					break;
				case 2:
					{
						thiz->Labelverify->Visible = true;
						thiz->Editverify->Visible = true;
						thiz->Imageverify->Visible = true;
						thiz->Imageverify->Picture->Assign((TJPEGImage*)obj);
                    }
					break;
			}
			thiz->Buttonlogin->Enabled = true;
		}
	};

	Screen->Cursor = crHourGlass;
	this->Buttonlogin->Enabled = false;
	FakeLogic::getInstance()->login(username, password, boost::bind(&handler::completion, this, _1, _2, _3));
}
//---------------------------------------------------------------------------
