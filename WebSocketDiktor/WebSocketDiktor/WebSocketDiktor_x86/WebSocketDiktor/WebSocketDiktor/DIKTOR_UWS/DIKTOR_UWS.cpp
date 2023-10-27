#include "DIKTOR_UWS.h" // низя докидывать либы бо будут конфликты, подключай всё в header

using namespace std;


Websocket::Websocket() {}

//void Websocket::SetupWebsocketThread(int port)
void Websocket::SetupWebsocketThread()
{
	auto app = uWS::App();
	auto socketConfig = uWS::App::WebSocketBehavior<EmptySocketData>();

	socketConfig.open = [this](auto* ws) // hello on connect
	{
		ws->subscribe("broadcast");
		if (this->OnConnectHello) { this->globalApp->publish("broadcast", "hello", uWS::OpCode::TEXT, true); }
	};

	//socketConfig.message = [](auto* ws, std::string_view message, uWS::OpCode opCode) { CallFunction(std::string(message)); };
	if ((this->IsExternalHandler) && onMessageCallbackExtern) { socketConfig.message = [this](auto* ws, std::string_view message, uWS::OpCode opCode) {  this->onMessageCallbackExtern(this->port, std::string(message)); }; }
	else if (this->IsExternalHandler) { this->IsError = true; this->ErrorMsg = "Extern Handler Error!"; this->Stop(); }
	else { socketConfig.message = [this](auto* ws, std::string_view message, uWS::OpCode opCode) { this->OnMessage(std::string(message)); }; }


	app.ws<EmptySocketData>("/*", std::move(socketConfig));

	app.listen(port, [](auto* listenSocket) {});

	this->globalApp = &app;
	this->loop = uWS::Loop::get();

	app.run();
}


void Websocket::Setup(int port, bool OnConnectHello)
{
	if (this->IsInit) { return; }
	this->port = port;
	this->OnConnectHello = OnConnectHello;
	std::thread setupThread([this]() { this->SetupWebsocketThread(); });
	setupThread.detach(); // не ждать его
	this->IsInit = true;
}

void Websocket::SetupHandle(int port, bool OnConnectHello, const OnMessageEvent& onMessageCallback)
{
	if (this->IsInit) { return; }
	this->port = port;
	this->OnConnectHello = OnConnectHello;
	this->IsExternalHandler = true;
	this->onMessageCallbackExtern = onMessageCallback;
	std::thread setupThread([this, onMessageCallback]() { this->SetupWebsocketThread(); });
	setupThread.detach(); // не ждать его
	this->IsInit = true;
}


void Websocket::Stop() { try { this->globalApp->close(); } catch (...) {} try { this->loop->free(); } catch (...) {} }
void Websocket::OnMessage(std::string message) { if (message != "") { this->MESSAGES.push_back(message); this->IsMessagesChanged = true; } }


std::vector<std::string> Websocket::GetMessages()
{
	if (!this->IsInit) { std::vector<std::string> errorVec; errorVec.push_back("~NOT INIT WEB SOCKET"); return errorVec; }
	this->IsMessagesChanged = false;
	return this->MESSAGES;
}

std::vector<std::string> Websocket::GetMessagesCLS()
{
	if (!this->IsInit) { std::vector<std::string> errorVec; errorVec.push_back("~NOT INIT WEB SOCKET"); return errorVec; }
	std::vector<std::string> copyOfMessages = this->MESSAGES;
	this->MESSAGES.clear();
	this->IsMessagesChanged = false;
	return copyOfMessages;
}


bool Websocket::SendWebsocketSTR(std::string text) // send STRING
{
	if (!this->IsInit) { return false; }
	if (text != "") { this->loop->defer([this, text]() { this->globalApp->publish("broadcast", text, uWS::OpCode::TEXT, true); }); return true; }
	return false;
}

bool Websocket::SendWebsocketJSON(nlohmann::json json) // send JSON
{
	if (!this->IsInit) { return false; }
	this->loop->defer([this, json]() { this->globalApp->publish("broadcast", json.dump(), uWS::OpCode::TEXT, true); });
	return true;
}

bool Websocket::SendWebsocketSTRAsJSON(std::string text)
{
	if (!this->IsInit) { return false; } // x2 check
	nlohmann::json json;
	//json["data"]["response"] = text;
	json["data"] = text;
	return this->SendWebsocketJSON(json);
}