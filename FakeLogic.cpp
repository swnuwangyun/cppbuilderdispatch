//---------------------------------------------------------------------------

#pragma hdrstop

#include "FakeLogic.h"
#include <IdHTTP.hpp>
//---------------------------------------------------------------------------
#pragma package(smart_init)

FakeLogic* FakeLogic::getInstance()
{
	static FakeLogic logic;
	return &logic;
}

void FakeLogic::login(String username, String password, CompletionHandler completion)
{
	struct task {
		static void execute(FakeLogic * thiz, String username, String password, CompletionHandler completion) {
			Sleep(2000);
			if(password==L"123456"){
				dispatch_main(boost::bind(completion, 1, L"", (void*)0));
			}else if(password==L"000000"){
				shared_ptr<TJPEGImage> image = thiz->__createImageFromUrl(L"http://sms.cr6868.com/CheckCode.aspx?now=1433426912204");
				dispatch_main(boost::bind(completion, 2, L"", (void*)image.get()));
				image.reset();
			}else{
				dispatch_main(boost::bind(completion, 0, L"Incorrect password!", (void*)0));
            }
		}
	};

	dispatch_async(boost::bind(&task::execute, this, username, password, completion));
}

shared_ptr<TJPEGImage> FakeLogic::__createImageFromUrl(String url) {
	shared_ptr<TJPEGImage>image(new TJPEGImage());
	scoped_ptr<TMemoryStream>ms(new TMemoryStream());
	scoped_ptr<TIdHTTP>http(new TIdHTTP());

	try {
		http->Get(url, ms.get());
		ms->Seek(0, soFromBeginning);
		image->LoadFromStream(ms.get());
	}
	__finally {
	}

	return image;
}
