#include <Arduino.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>


void conexaoWifi();
void reconexaoWifi();
String consultaApiCep(String cep);
bool cepValido(String cep);

const char *SSID = "SENAI EDUCACIONAL";
const char *SENHA = "info@134";

const unsigned long tempoConexao = 10000;
const unsigned long tempoReconexao = 5000;

const String enderecoApi = "https://brasilapi.com.br/api/cep/v2/";

//ex. 1
// pedir para o usuário digitar o cep na serial
// realizar a consulta apenas se for digitado um cep
// verificar se o cep é válido antes de fazer a consulta (8 caracteres)
//verificar se for 404 e exibir cep não encontrado 

void setup() {
  Serial.begin(115200);

  conexaoWifi();

  while (Serial.available() > 0){
    Serial.read();
  }

  String cep = "";

  Serial.println("Digite o seu CEP (8 dígitos)");

  while(!cepValido(cep)){
    

    while(Serial.available() == 0){
      delay(6000);
    }

    cep = Serial.readStringUntil('\n');
    cep.trim();

    if(cep.length() == 0){
      continue;
    }

    if(!cepValido(cep)){
      Serial.println("CEP inválido. Tente novamente.");
      Serial.println("Digite o seu CEP (8 dígitos)");
    }
  }

  String respostaApiCep = consultaApiCep(cep);

   

  if (respostaApiCep != "")
  {
    JsonDocument doc;

    DeserializationError erro = deserializeJson(doc, respostaApiCep);

    if(!erro){

      String rua = doc["street"];
      String bairro = doc["neighborhood"];
      String cidade = doc["city"];
      String estado = doc["state"];

      Serial.println();
      Serial.println("DADOS DO ENDEREÇO");
      Serial.println("-----------------");
      Serial.println("CEP:    " + cep);
      Serial.println("Rua:    " + rua);
      Serial.println("Bairro: " + bairro);
      Serial.println("Cidade: " + cidade);
      Serial.println("Estado: " + estado);

    }
    else{
      Serial.println("Erro de interpretar o JSON");
    }
  }
}

void loop() {

  reconexaoWifi();

}

bool cepValido(String cep){
  if (cep.length() != 8){
    return false;
  }

  for (size_t i = 0; i < cep.length(); i ++){
    if(!isDigit(cep[i])){
      return false;
    }
  }

  return true;

}

void conexaoWifi() {
  
  // Conexao WiFi
  Serial.printf("Conectando ao WiFi: %s", SSID);

  WiFi.begin(SSID, SENHA);

  unsigned long tempoWifi = millis();

  while (WiFi.status() != WL_CONNECTED && millis() - tempoWifi < tempoConexao)
  {
    Serial.print(".");
    delay(500);
  }

  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println("WiFi conectado com sucesso!");
    Serial.print("Endereço IP: ");
    Serial.println(WiFi.localIP());
  }
  else
  {
    Serial.println("Falha ao se conectar");
  }
 
}

void reconexaoWifi() {
  unsigned long tempoAtual = millis();
  static unsigned long tempoUltimaConexao = 0;

  if (tempoAtual - tempoUltimaConexao > tempoReconexao)
  {
    if (WiFi.status() != WL_CONNECTED)
    {
      Serial.println("Conexao WiFi perdida");
      conexaoWifi();
    }

    tempoUltimaConexao = tempoAtual;
    
  }
  
}

String consultaApiCep(String cep){
  HTTPClient http;
  http.begin(enderecoApi + cep);

  int codigoHttp = http.GET();
  String resposta = "";

  delay(3000);

  if (codigoHttp == 200)
  {
    resposta = http.getString();
  }
  else if(codigoHttp == 404){
    Serial.print("CEP não encontrado");
  }
  else{
    Serial.println("Erro. Código HTTP: " + String(codigoHttp));
  }
  http.end();

  return resposta;
  

}