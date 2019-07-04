#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
static int qtd = 0;
struct msg {
  char  nome[200];
  char  mensagem[200];
  int msgId;
  msg * next = NULL;
};
struct msg * msgList = NULL;
struct msg * beginOfList = NULL;
// Replace with your network credentials
//const char * ssid = "GVT-77E3";
//const char * password = "S1EB045641";
const char * ssid = "a";
const char * password = "aaaaaaaa";
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
    Serial.print("mensagem recebida e armazenada como inicio da lista\n" + n + " " + m +"\n");
    beginOfList = ptr;
    msgList = ptr;
    ptr->next = NULL;
  } else {
    msgList-> next = (struct msg * ) malloc(sizeof(struct msg));
    msgList = msgList-> next;
    name_.toCharArray(msgList->nome, 200);
    content_.toCharArray(msgList->mensagem, 200);
    String n = String(msgList->nome);
    String m = String(msgList->mensagem);
    qtd++;
    msgList->msgId = qtd;
    msgList->next = NULL;
    String statusString = "mensagem recebida e adicionada a lista\n" + n + " " + m +"\n";
    Serial.print(statusString);
  }
  server.send(200, "text/plain", "Mensagem recebida");
}

String writeHTMLPage() {
String page = "<!DOCTYPE html><html lang='en'>  <head>    <meta charset='utf-8'>    <title>Chat Chat</title>  </head>  <body onload='myFunction();'> <div id='maindiv'>      </div>    <input type='text' placeholder='mensagem' id='inpute'></input>    <div id='btn'><span>Enviar</span></div> </div>  </body></html>";
page+="<style>body{font-family:arial}#maindiv{display:block;position:relative;width:100%;height:80vh;background-color:rgba(0,0,0,.05);overflow:auto}#inpute{display:block;height:10vh;width:70%;font-size:20pt;float:left}#btn{display:block;height:10vh;width:20%;float:right;background-color:rgba(0,255,0,.1);font-size:30pt;border-style:solid;border-color:rgba(0,255,0,.3);text-align:center}#btn span{display:block}.msg{display:block;font-size:20pt;border-radius:20px}.fromMe{border-top-right-radius:0;float:right;background-color:teal;padding:10px 25px 10px 25px;max-width:700pt;margin-top:15pt}.fromStranger{border-top-left-radius:0;float:left;background-color:rgba(128,128,128);padding:10px 25px 10px 25px;max-width:700pt;margin-top:15pt;overflow-wrap:break-word;word-wrap:break-word;hyphens:auto}.lineFromMe{display:block;overflow:auto;width:100%;margin-top:20px;word-wrap:break-word;overflow-wrap:break-word}.lineFromStranger{display:block;width:100%;margin-top:20px;word-wrap:break-word;overflow-wrap:break-word}</style>";

page +="<script type=\"text/javascript\">";

page +="myName = \"\";do{myName = prompt(\"Please enter your name\");}while(myName==\"\");myName+=\"#\"+Math.floor(Math.random() * 9999);alert(myName + \" entrou no chat\");msgArray = [];function imprimirMensagem(arg) {  main = document.getElementById(\"maindiv\");  line = document.createElement(\"div\"); if (arg.name == myName)   line.setAttribute(\"class\", \"lineFromMe\"); else    line.setAttribute(\"class\", \"lineFromStranger\"); main.appendChild(line); content = document.createElement(\"div\");  if (arg.name == myName)   content.setAttribute(\"class\", \"msg fromMe\");  else    content.setAttribute(\"class\", \"msg fromStranger\");  content.innerHTML = arg.content;  line.appendChild(content);}function msgExists(msg){ a = false;  msgArray.forEach(   function(item){     if(item.name == msg.name && item.content == msg.content)        a = true;   }); return a;}";
page +="function getMensagens() {xmlhttp = new XMLHttpRequest();xmlhttp.open('GET', '/msgList', true);xmlhttp.onreadystatechange = function(){if (xmlhttp.readyState == XMLHttpRequest.DONE) {if (xmlhttp.status == 200) {msgs = JSON.parse(xmlhttp.responseText);msgs.forEach(function(item) {if(!msgExists(item)){msgArray.push(item);imprimirMensagem(item);}});}}};xmlhttp.setRequestHeader('Content-Type', 'application/json');xmlhttp.send();}";
page +="function sendMessage() {  inputContent = document.getElementById(\"inpute\").value; if (inputContent != \"\") {   document.getElementById(\"inpute\").value = \"\";   xmlhttp = new XMLHttpRequest();   xmlhttp.open(\"POST\", \"/msg\", true);   xmlhttp.setRequestHeader(\"Content-Type\", \"application/json\");   xmlhttp.onreadystatechange = function() {     if (xmlhttp.readyState == XMLHttpRequest.DONE) {        if (xmlhttp.status == 200) {          document.getElementById(\"inpute\").value = \"\";                 } else if (xmlhttp.status == 400) {         alert('There was an error 400');        } else {          alert('something else other than 200 was returned');        }     }   };    json_upload = JSON.stringify({      name: myName,     content: inputContent   });   xmlhttp.send(json_upload);  }}";
page +="function myFunction(){document.getElementById(\"btn\").onclick = function(){sendMessage();setInterval(getMensagens, 500);};}";




page +="</script>";



return page;


}
String msgJson(){
  String msgString = "[";
  struct msg * myList = beginOfList;
  if(myList==NULL)
  return "[]";
  else
  do{
    String o = String(myList->nome);
    String p = String(myList->mensagem);
    msgString+="{\"name\":\""+o+"\",\"content\":\""+p+"\"}";
    myList = myList->next;
    if(myList!=NULL)
      msgString+=",";
    else  msgString+="]";
  }while(myList!=NULL);
  return msgString;
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
  server.on("/msgList",
    [] {
      server.send(200, "application/json", msgJson());
    });
    
  server.on("/msg",receiveMsg);

  server.begin();
}

void loop() {
  server.handleClient();
}
