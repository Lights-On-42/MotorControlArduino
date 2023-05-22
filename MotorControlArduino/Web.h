#include <Arduino.h>
#include <ESP8266WebServer.h>
#include <ArduinoWebsockets.h>
#include <WiFiManager.h>

using namespace websockets;
String zwComand;

class Web
{
public:
    static void startHTTP();
    static void startWS();
    static void listen_ws();
    static void tick(String* lastCommand);
    static void send(String message);

private:
    static ESP8266WebServer http_server;
    static WebsocketsServer ws_server;
    static std::function<void(WebsocketsClient &, String, String)> ws_listener;
    static void handler(WebsocketsClient &client, WebsocketsMessage message);
    static void serve_index();
    static WebsocketsClient *client;
    static std::vector<WebsocketsClient> all_clients;
    static void poll_all_clients(String* lastCommand);
};

std::vector<WebsocketsClient> Web::all_clients;
WebsocketsClient *Web::client = nullptr;
ESP8266WebServer Web::http_server = ESP8266WebServer();
WebsocketsServer Web::ws_server = WebsocketsServer();

void Web::startWS()
{
    ws_server.listen(81);
    Serial.println("WS server started");
}

void Web::send(String message)
{
}

void Web::serve_index()
{
    http_server.send(200, "text/html",indexhtml);// Data::index);
}

void Web::startHTTP()
{
    http_server.begin(80);
    http_server.on("/", serve_index);
    Serial.println("HTTP server started");
}


void Web::handler(WebsocketsClient &client, WebsocketsMessage message)
{
    WSInterfaceString data = message.data();

    //Serial.println(data);
    zwComand=data;
/*
    int index = data.indexOf(":");
    if (index > 0)
    {
        String first = data.substring(0, index);
        String second = data.substring(index+1);
    }*/

    client.send("status:" + data);
}

void Web::poll_all_clients(String* lastCommand)
{

    for (auto &client : all_clients)
    {
        if(client.poll())
        {
          *lastCommand = zwComand;
          break;
        }
    }
}

void Web::tick(String* lastCommand)
{
    if (ws_server.available())
    {
        if (ws_server.poll())
        {
            WebsocketsClient c = ws_server.accept();
            Serial.println("Client accepted");
            c.onMessage(handler);
            all_clients.push_back(c);
        }
    }

    poll_all_clients(lastCommand);
    http_server.handleClient();
}