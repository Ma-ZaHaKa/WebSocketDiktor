#pragma once //BY DIKTOR
#pragma comment(lib, "zlib.lib")
#pragma comment(lib, "uv.lib")
#pragma comment(lib, "uSockets.lib")
#include <thread>
#include <string>
#include <functional>
#include <uwebsockets/App.h>
#include <nlohmann/json.hpp>


class Websocket
{
	using OnMessageEvent = std::function<void(std::string)>;

	static inline uWS::App* globalApp;
	static inline uWS::Loop* loop;

	struct EmptySocketData {};

	//static void SetupWebsocketThread(int port);
	static void SetupWebsocketThread(int port, const OnMessageEvent& onMessageCallback);

public:
	//static void Setup(int port);
	//static void CallFunction(std::string text); // on message
	static void Setup(int port, const OnMessageEvent& onMessageCallback);
	static void CallFunction(std::string text, const OnMessageEvent& onMessageEvent); // on message
	static void SendWebsocketMessageJson(nlohmann::json json); // answer json
	static void SendWebsocketMessage(std::string text); // answer json
	static void SendCrowdControlResponse(std::string text); // anser string
	static void SendCrowdControlResponseAssocArrayJson(std::vector<std::string> keys, std::vector<std::string> data); // anser string
};
