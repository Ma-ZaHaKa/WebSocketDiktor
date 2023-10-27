#include "DIKTOR_UWS\DIKTOR_UWS.h" // BY DIKTOR
#include "Windows.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstring>

bool IsInit = false;
std::vector<std::string> MESSAGES;

//DWORD CALLBACK uWSThread(LPVOID lpParam);

static void OnMessage(std::string _msg) { if (_msg != "") { MESSAGES.push_back(_msg); } }

extern "C" __declspec(dllexport) std::vector<std::string> WGetMessages(bool isClearStack) // собираем буффер
{
	if (!IsInit) { std::vector<std::string> errorVec; errorVec.push_back("~NOT INIT DK_DLL~"); return errorVec; }
	if (!isClearStack) { return MESSAGES; }
	std::vector<std::string> copyOfMessages = MESSAGES;
	MESSAGES.clear();
	return copyOfMessages;
}

extern "C" __declspec(dllexport) bool WSendMessage(std::string _msg) // возвращаем запрос
{
	if (IsInit) { Websocket::SendWebsocketMessage(_msg); return true; } // просто текст
	return false;
}

extern "C" __declspec(dllexport) bool WSendMessageAsJson(std::vector<std::string> keys, std::vector<std::string> data) // возвращаем запрос
{
	if (IsInit) { Websocket::SendCrowdControlResponseAssocArrayJson(keys, data); return true; } // формировка json
	return false;
}

extern "C" __declspec(dllexport) bool WSendMessageAsDataJson(std::string _msg) // возвращаем запрос
{
	if (IsInit) { Websocket::SendCrowdControlResponse(_msg); return true; } // строка в json["data"]
	return false;
}

extern "C" __declspec(dllexport) bool WGetInitStatus() { return IsInit; }

extern "C" __declspec(dllexport) bool WStartuWS(int port)
{

	if (!IsInit)
	{
		Websocket::Setup(port, OnMessage);
		IsInit = true;
		return true;
	}
	return false;
}

//extern "C" __declspec(dllexport) bool WStartuWS(int port)
//{
//
//	if (!IsInit)
//	{
//		int* portPtr = new int(port);
//		CreateThread(NULL, 0, uWSThread, portPtr, 0, NULL);
//		return true;
//	}
//	return false;
//}
//
//DWORD CALLBACK uWSThread(LPVOID lpParam)
//{
//	int port = *(int*)lpParam;
//	delete (int*)lpParam;
//
//	Websocket::Setup(port, OnMessage);
//	IsInit = true;
//	return TRUE;
//}


static void Mbox(const std::string& msg, const std::string& title = "title")
{
	MessageBoxA(HWND_DESKTOP, msg.c_str(), title.c_str(), MB_SYSTEMMODAL | MB_ICONWARNING);
}



BOOL WINAPI DllMain(_In_ HINSTANCE hinstDLL, _In_ DWORD fdwReason, _In_ LPVOID lpvReserved)
{
	if (fdwReason == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(hinstDLL); // ускорение загрузки библиотеки
		/*CreateThread(NULL, 0, ThreadEntry, NULL, 0, NULL);*/
	}
	return TRUE;
}



//void Init()
//{
//	int port = 9002;
//	Websocket::Setup(port, OnMessage);
//	IsInit = true;
//}


//DWORD CALLBACK ThreadEntry(LPVOID lpParam) // lpParam apth
//{
//	Init();
//	return TRUE;
//}

//BOOL WINAPI DllMain(_In_ HINSTANCE hinstDLL, _In_ DWORD fdwReason, _In_ LPVOID lpvReserved)
//{
//	switch (fdwReason)
//	{
//	case DLL_PROCESS_ATTACH:
//		DisableThreadLibraryCalls(hinstDLL); // ускорение загрузки библиотеки
//		CreateThread(NULL, 0, ThreadEntry, NULL, 0, NULL);
//		//while (1) { Sleep(1000); }
//		// The DLL is being loaded into the virtual address space of a process.
//		// Perform initialization here.
//		break;
//	case DLL_PROCESS_DETACH:
//		// The DLL is being unloaded from the virtual address space of a process.
//		// Perform cleanup here.
//		break;
//	case DLL_THREAD_ATTACH:
//		// A new thread is being created in the current process.
//		// Perform per-thread initialization here.
//		break;
//
//	case DLL_THREAD_DETACH:
//		// A thread is exiting gracefully.
//		// Perform per-thread cleanup here.
//		break;
//	}
//
//	return TRUE;
//}


/*int main()
{
	CreateThread(NULL, 0, ThreadEntry, NULL, 0, NULL);
	system("pause");
	return 0;
}*/


//============CALL EXAMPLE
/*
typedef std::vector<std::string>(*WGetMessagesFunction)(bool);
WGetMessagesFunction WGetMessages;

typedef bool (*WSendMessageFunction)(std::string);
WSendMessageFunction WSendMessage;

typedef bool (*WSendMessageAsJsonFunction)(std::vector<std::string>, std::vector<std::string>);
WSendMessageAsJsonFunction WSendMessageAsJson;

typedef bool (*WSendMessageAsDataJsonFunction)(std::string);
WSendMessageAsDataJsonFunction WSendMessageAsDataJson;

bool isStartUWS = false;

void UWS()
{
	while (true) { if (!IsInitEnd) { Sleep(1000); } else { break; } }

	//typedef std::vector<std::string>(*WGetMessagesFunction)(bool);
	//typedef bool (*WSendMessageFunction)(std::string);

	ghDiktorUWS = LoadLibraryA("WebSocketDiktor.dll");
	Sleep(SteamInitDelay); // init my lib

	WGetMessages = reinterpret_cast<WGetMessagesFunction>(GetProcAddress(ghDiktorUWS, "WGetMessages"));
	WSendMessage = reinterpret_cast<WSendMessageFunction>(GetProcAddress(ghDiktorUWS, "WSendMessage"));
	WSendMessageAsJson = reinterpret_cast<WSendMessageAsJsonFunction>(GetProcAddress(ghDiktorUWS, "WSendMessageAsJson"));
	WSendMessageAsDataJson = reinterpret_cast<WSendMessageAsDataJsonFunction>(GetProcAddress(ghDiktorUWS, "WSendMessageAsDataJson"));
	Sleep(50);
	isStartUWS = true;
	return;

	//WGetMessagesFunction getMessagesFunction = reinterpret_cast<WGetMessagesFunction>(GetProcAddress(ghDiktorUWS, "WGetMessages"));
	//WSendMessageFunction sendMessageFunction = reinterpret_cast<WSendMessageFunction>(GetProcAddress(ghDiktorUWS, "WSendMessage"));
	//isStartUWS = true;

	//bool result = sendMessageFunction("MSG!");
	//while (true)
	//{// print stack
	//	std::vector<std::string> messages = getMessagesFunction(true);
	//	if (!messages.empty()) {
	//		for (const std::string& message : messages) {
	//			std::cout << "UWS_MES: " << message << std::endl;
	//		}
	//	}
	//	Sleep(1000);
	//}
}
if(isStartUWS) { WSendMessage(std::to_string(outIdDIV2));} // sms master with result ID/2
*/











//=============CALLING EXPORT
/*
int main()
{
	const char* inputString = "Hello, DLL!";

	HMODULE dllHandle = LoadLibrary("your_dll_name.dll");
	if (dllHandle)
	{
		// ѕолучаем указатель на функцию ProcessString
		auto processStringFunc = reinterpret_cast<std::string(*)(const char*)>(GetProcAddress(dllHandle, "ProcessString"));

		if (processStringFunc)
		{
			std::string result = processStringFunc(inputString);
			std::cout << "Processed String: " << result << std::endl;
		}
		else
		{
			std::cerr << "Failed to get the function pointer." << std::endl;
		}

		FreeLibrary(dllHandle);
	}
	else
	{
		std::cerr << "Failed to load the DLL." << std::endl;
	}

	return 0;
}
*/