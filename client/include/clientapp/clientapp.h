#include <kdvtype.h>

#ifdef _EXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT __declspec(dllimport)
#endif

namespace CLIENTAPP
{
	// Osp��ʼ��
	DLL_EXPORT void Init();
	// ��ײ㷢����Ϣ
	DLL_EXPORT void Post(u16 eventType, const void *content, u16 length);
	// ���ûص�����
	DLL_EXPORT void SetCallBackFunc(void (*callBack)(u16 eventType, const void* content, u16 length));
}



