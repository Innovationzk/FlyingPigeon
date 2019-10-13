#include <kdvtype.h>

#ifdef _EXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT __declspec(dllimport)
#endif

namespace CLIENTAPP
{
	// Osp初始化
	DLL_EXPORT void Init();
	// 向底层发送消息
	DLL_EXPORT void Post(u16 eventType, const void *content, u16 length);
	// 设置回调函数
	DLL_EXPORT void SetCallBackFunc(void (*callBack)(u16 eventType, const void* content, u16 length));
}



