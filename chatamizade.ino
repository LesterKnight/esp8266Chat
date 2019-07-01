#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
static int qtd = 0;
struct msg {
  char  nome[200];
  char  mensagem[200];
  int msgId;
  msg * next;
};

struct msg * msgList;
struct msg * beginOfList;
// Replace with your network credentials
const char * ssid = "GVT-77E3";
const char * password = "S1EB045641";
ESP8266WebServer server;

void receiveMsg() {
  String input = server.arg("plain");
  StaticJsonDocument < 256 > doc;
  deserializeJson(doc, input);
  String name_ = doc["name"];
  String content_ = doc["content"];

  if (msgList == NULL) {
    Serial.print("msgList esta nulo\n");
    struct msg* ptr = (struct msg * ) malloc(sizeof(struct msg));
    name_.toCharArray(ptr->nome, 200);
    content_.toCharArray(ptr->mensagem, 200);
    ptr->msgId = qtd;
    String n = String(ptr->nome);
    String m = String(ptr->mensagem);
    Serial.print("mensagem recebida e armazenada como inicio da fila\n" + n + " " + m +"\n");
    msgList = ptr;
    beginOfList = ptr;
    
  } else {
    msgList-> next = (struct msg * ) malloc(sizeof(struct msg));
    msgList = msgList-> next;
    name_.toCharArray(msgList->nome, 200);
    content_.toCharArray(msgList->mensagem, 200);
    String n = String(msgList->nome);
    String m = String(msgList->mensagem);
    qtd++;
    msgList->msgId = qtd;
    Serial.print("mensagem recebida e adicionada a fila\n" + n + " " + m +"\n");
    Serial.print(qtd);
  }
  server.send(200, "text/plain", "success");
  msgJson();
}

String writeHTMLPage() {
  return "<!DOCTYPE html><htm><head><title>teste</title></head><body><h1>Title</h1></body></hmtl>";
}

void msgJson(){
const int capacity = JSON_OBJECT_SIZE(3);
StaticJsonDocument<capacity> doc;
StaticJsonDocument<capacity> doc1;
StaticJsonDocument<capacity> doc2;
JsonArray arr = doc.to<JsonArray>();

String n = String(msgList->nome);
String m = String(msgList->mensagem);

doc1["name"] = n;
doc1["content"] = m;
doc1["n"] = msgList->msgId;

char output[128];
serializeJson(doc, output);
Serial.print(output);
}
  
void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println(WiFi.localIP());

  server.on("/",
    [] {
      server.send(200, "text/html", writeHTMLPage());
    });
  server.on("/msg",receiveMsg);

  server.begin();
}

void loop() {
  server.handleClient();
}
