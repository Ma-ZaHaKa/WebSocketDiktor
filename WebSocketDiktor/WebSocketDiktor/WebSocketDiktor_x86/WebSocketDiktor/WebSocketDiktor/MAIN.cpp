#include "DIKTOR_UWS\DIKTOR_UWS.h" // BY DIKTOR!!!
#include "Windows.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstring>

std::vector<std::string> GetErrorVector();
std::vector<std::string> GetErrorVectorSTR(std::string message);
bool CheckStartedPort(int port);
Websocket* GetSrvPtrByPort(int port);


std::vector<Websocket*> Servers;
bool sync_flag = false;
using OnMessageEvent = std::function<void(int, std::string)>; // void OnMessage(int port, std::string message) { std::cout << "Port: " << port << " MESSAGE: " << message << "\n"; }

//---USE
//NLOHMAN::JSON
//OnMessageEvent PROTO
//EXTERN FUNC PROTO + INIT ADDR


//-----------------------BY---DIKTOR--------------------------------------//
extern "C" __declspec(dllexport) bool Setup(int port, bool OnConnectHello)
{
	while (true) { if (sync_flag) { Sleep(10); } else { sync_flag = true; break; } }
	if (CheckStartedPort(port)) { sync_flag = false; return false; } // already started
	//Websocket uws(); // стек
	Websocket* uws = new Websocket();
	uws->Setup(port, OnConnectHello);
	Servers.push_back(uws);
	sync_flag = false;
	return true;
}

extern "C" __declspec(dllexport) bool SetupHandle(int port, bool OnConnectHello, OnMessageEvent onMessageCallback)
{
	while (true) { if (sync_flag) { Sleep(10); } else { sync_flag = true; break; } }
	if (CheckStartedPort(port)) { sync_flag = false; return false; } // already started
	//Websocket uws(); // стек
	Websocket* uws = new Websocket();
	uws->SetupHandle(port, OnConnectHello, onMessageCallback);
	Servers.push_back(uws);
	sync_flag = false;
	return true;
}

extern "C" __declspec(dllexport) bool Stop(int port)
{
	Websocket* uws = GetSrvPtrByPort(port);
	if (!uws) { return false; }
	uws->Stop();
	return true;
}

extern "C" __declspec(dllexport) bool GetInitStatus(int port)
{
	Websocket* uws = GetSrvPtrByPort(port);
	if (!uws) { return false; }
	return uws->IsInit;
}



extern "C" __declspec(dllexport) std::vector<std::string> GetMessages(int port)
{
	Websocket* uws = GetSrvPtrByPort(port);
	if (!uws) { return GetErrorVectorSTR("~srv pointer not found"); }
	if (!uws->IsInit) { return GetErrorVectorSTR("~srv not initalized"); }
	return uws->GetMessages();
}

extern "C" __declspec(dllexport) std::vector<std::string> GetMessagesCLS(int port) // clear all stack msg by port
{
	Websocket* uws = GetSrvPtrByPort(port);
	if (!uws) { return GetErrorVectorSTR("~srv pointer not found"); }
	if (!uws->IsInit) { return GetErrorVectorSTR("~srv not initalized"); }
	return uws->GetMessagesCLS();
}





extern "C" __declspec(dllexport) bool SendWebsocketSTR(int port, std::string text)
{
	Websocket* uws = GetSrvPtrByPort(port);
	if ((!uws) || (!uws->IsInit)) { return false; }
	return uws->SendWebsocketSTR(text);
}

extern "C" __declspec(dllexport) bool SendWebsocketJSON(int port, nlohmann::json json)
{
	Websocket* uws = GetSrvPtrByPort(port);
	if ((!uws) || (!uws->IsInit)) { return false; }
	return uws->SendWebsocketJSON(json);
}

extern "C" __declspec(dllexport) bool SendWebsocketSTRAsJSON(int port, std::string text) // строка в json["data"]
{
	Websocket* uws = GetSrvPtrByPort(port);
	if ((!uws) || (!uws->IsInit)) { return false; }
	return uws->SendWebsocketSTRAsJSON(text);
}





std::vector<std::string> GetErrorVectorSTR(std::string messages)
{
	std::vector<std::string> errorVec;
	errorVec.push_back(messages);
	return errorVec;
}

std::vector<std::string> GetErrorVector()
{
	std::vector<std::string> errorVec;
	errorVec.push_back("~NOT INIT DK_DLL~");
	return errorVec;
}

bool CheckStartedPort(int port)
{
	for (int i = 0; i < Servers.size(); i++) { if ((Servers[i]) && (Servers[i]->port == port)) { return true; } }
	return false;
}

Websocket* GetSrvPtrByPort(int port)
{
	for (int i = 0; i < Servers.size(); i++) { if ((Servers[i]) && (Servers[i]->port == port)) { return Servers[i]; } }
	return nullptr;
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