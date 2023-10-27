#pragma once //BY DIKTOR
#pragma comment(lib, "zlib.lib")
#pragma comment(lib, "uv.lib")
#pragma comment(lib, "uSockets.lib")
#include <thread>
#include <string>
#include <functional>
#include <uwebsockets/App.h>
#include <nlohmann/json.hpp>
#include <vector>


class Websocket
{
	using OnMessageEvent = std::function<void(int, std::string)>;  // внешний обработчик на сообщение port+msg

	uWS::App* globalApp;
	uWS::Loop* loop;
	OnMessageEvent onMessageCallbackExtern;
	bool IsExternalHandler = false;
	//std::thread WebSocketThread; // ничё пока не даст
	//bool isSetup = false;

	struct EmptySocketData {};

	void SetupWebsocketThread();
	void OnMessage(std::string message); // message handler. saved to MESSAGES buffer

public:
	int port = 9007;
	bool OnConnectHello = false;
	bool IsInit = false;
	bool IsMessagesChanged = false;

	//--ON-ERROR
	bool IsError = false;
	std::string ErrorMsg = "";

	std::vector<std::string> MESSAGES; // NOT WORK IF Setuped by SetupHandle

	Websocket(); // Конструктор

	void Setup(int port, bool OnConnectHello);
	void SetupHandle(int port, bool OnConnectHello, const OnMessageEvent& onMessageCallback);
	void Stop();

	std::vector<std::string> GetMessages();
	std::vector<std::string> GetMessagesCLS();

	bool SendWebsocketSTR(std::string text); // answer text
	bool SendWebsocketJSON(nlohmann::json json); // answer json
	bool SendWebsocketSTRAsJSON(std::string text); // answer text as json
};
