#include "DIKTOR_UWS.h" // низя докидывать либы бо будут конфликты, подключай всё в header

using namespace std;

void EnableCMD()
{
	AllocConsole();

	//SetConsoleOutputCP(866);
	setlocale(LC_ALL, "Russian");
	SetConsoleOutputCP(1251);
	SetConsoleCP(1251);


	freopen("CONIN$", "r", stdin);
	freopen("CONOUT$", "w", stdout);

	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN);
}


//void Websocket::SetupWebsocketThread(int port)
void Websocket::SetupWebsocketThread(int port, const OnMessageEvent& onMessageCallback)
{
	bool OnConnectMsg = false;
	auto app = uWS::App();
	auto socketConfig = uWS::App::WebSocketBehavior<EmptySocketData>();

	socketConfig.open = [OnConnectMsg](auto* ws) // hello on connect
	{
		ws->subscribe("broadcast");
		if (OnConnectMsg) { globalApp->publish("broadcast", "hello", uWS::OpCode::TEXT, true); }
	};

	//socketConfig.message = [](auto* ws, std::string_view message, uWS::OpCode opCode) { CallFunction(std::string(message)); };
	socketConfig.message = [onMessageCallback](auto* ws, std::string_view message, uWS::OpCode opCode) { CallFunction(std::string(message), onMessageCallback); };


	app.ws<EmptySocketData>("/*", std::move(socketConfig));

	app.listen(port, [](auto* listenSocket) {});

	globalApp = &app;
	loop = uWS::Loop::get();

	app.run();
}

//void Websocket::Setup(int port)
void Websocket::Setup(int port, const OnMessageEvent& onMessageCallback)
{
	std::thread setupThread([port, onMessageCallback]() { SetupWebsocketThread(port, onMessageCallback); });
	setupThread.detach(); // не ждать его
}


//void Websocket::CallFunction(std::string text) // on message
void Websocket::CallFunction(std::string text, const OnMessageEvent& onMessageCallback)
{
	try
	{
		//if (text.length() != 0) { if (onMessageCallback) { onMessageCallback(text); } }
		if (text != "") { if (onMessageCallback) { onMessageCallback(text); } }
		else { SendWebsocketMessage("INVALID STRING!"); }
	}
	catch (exception e) { SendWebsocketMessage("ERROR PROCESSING!"); }

}

void Websocket::SendWebsocketMessageJson(nlohmann::json json) // send answer
{
	loop->defer([json]() { globalApp->publish("broadcast", json.dump(), uWS::OpCode::TEXT, true); });
}

void Websocket::SendWebsocketMessage(std::string text) // send answer
{
	loop->defer([text]() { globalApp->publish("broadcast", text, uWS::OpCode::TEXT, true); });
}

void Websocket::SendCrowdControlResponse(std::string text)
{
	nlohmann::json json;

	//json["type"] = "CrowdControl";
	//json["data"]["response"] = text;
	json["data"] = text;

	SendWebsocketMessageJson(json);
}

void Websocket::SendCrowdControlResponseAssocArrayJson(std::vector<std::string> keys, std::vector<std::string> data)
{
	if (keys.size() != data.size()) { return; }
	nlohmann::json json;

	//json["type"] = "CrowdControl";
	//json["data"]["response"] = text;
	for (int i = 0; i < keys.size(); ++i) { json[keys[i]] = data[i]; }
	SendWebsocketMessageJson(json);
}