//---------------------------------------------------------------------------

#ifndef FakeLogicH
#define FakeLogicH
#include <jpeg.hpp>
#include "ThreadPool.h"
//---------------------------------------------------------------------------
typedef boost::function< void(int, String, void*) > CompletionHandler;
//---------------------------------------------------------------------------
class FakeLogic
{
public:
	static FakeLogic* getInstance();
	void login(String username, String password, CompletionHandler completion);

public:
	shared_ptr<TJPEGImage>FakeLogic::__createImageFromUrl(String url);
};
#endif
