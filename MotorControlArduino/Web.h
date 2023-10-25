#include <Arduino.h>
//#include <ESP8266WebServer.h>
#include <ArduinoWebsockets.h>
#include <WiFiManager.h>

using namespace websockets;

//private variablen
String zwComand;
bool wasConnectet= false;
String tcpipNewComand;

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

    static void poll_tcpip_client(String* lastCommand);
    static WiFiClient tcpipClient;
    static WiFiServer tcpipServer; 
};

std::vector<WebsocketsClient> Web::all_clients;
WebsocketsClient *Web::client = nullptr;
ESP8266WebServer Web::http_server = ESP8266WebServer();
WebsocketsServer Web::ws_server = WebsocketsServer();
WiFiServer Web::tcpipServer=WiFiServer(5045);
WiFiClient Web::tcpipClient =WiFiClient();



void Web::startWS()
{
    ws_server.listen(81);
    tcpipServer.begin();
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
    zwComand=data;
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

void Web::poll_tcpip_client(String* lastCommand)
{
  
  if (tcpipClient.connected() == false)
  {
    if (wasConnectet == true)
    {
      Serial.println("Client verloren");
      wasConnectet = false;
    }
    tcpipClient = tcpipServer.available();
    if (tcpipClient.connected())
    {
      Serial.println("neuer Client");
      wasConnectet = true;
    }
  }
  else
  {
    if (tcpipClient.available() > 0)
    {
      char c = tcpipClient.read();    
      if (c == '\n')
      {
        *lastCommand = tcpipNewComand;
        tcpipNewComand = "";
      }
      else
      {tcpipNewComand += c;}
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

    poll_tcpip_client(lastCommand);
    poll_all_clients(lastCommand);
    http_server.handleClient();
}