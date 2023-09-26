#include "dht.h" //INCLUSÃO DE BIBLIOTECA
#include <Wire.h> //INCLUSÃO DE BIBLIOTECA
#include <LiquidCrystal_I2C.h> //INCLUSÃO DE BIBLIOTECA
#include "WiFiEsp.h" //INCLUSÃO DA BIBLIOTECA
#include "SoftwareSerial.h"//INCLUSÃO DA BIBLIOTECA

SoftwareSerial Serial1(6, 7); //PINOS QUE EMULAM A SERIAL, ONDE O PINO 6 É O RX E O PINO 7 É O TX 
LiquidCrystal_I2C lcd(0x27,2,1,0,4,5,6,7,3, POSITIVE); //ENDEREÇO DO I2C E DEMAIS INFORMAÇÕES

// Sensor de umidade do ar
const int pinoDHT11 = A2; //PINO ANALÓGICO UTILIZADO PELO DHT11 
dht DHT; //VARIÁVEL DO TIPO DHT

// Configuração do wifi
char ssid[] = "leticia"; //VARIÁVEL QUE ARMAZENA O NOME DA REDE SEM FIO
char pass[] = "euthyrox";//VARIÁVEL QUE ARMAZENA A SENHA DA REDE SEM FIO

int status = WL_IDLE_STATUS; //STATUS TEMPORÁRIO ATRIBUÍDO QUANDO O WIFI É INICIALIZADO E PERMANECE ATIVO
//ATÉ QUE O NÚMERO DE TENTATIVAS EXPIRE (RESULTANDO EM WL_NO_SHIELD) OU QUE UMA CONEXÃO SEJA ESTABELECIDA
//(RESULTANDO EM WL_CONNECTED)
WiFiEspServer server(80); //CONEXÃO REALIZADA NA PORTA 80
RingBuffer buf(8); //BUFFER PARA AUMENTAR A VELOCIDADE E REDUZIR A ALOCAÇÃO DE MEMÓRIA

 
void setup(){
  Serial.begin(9600); //INICIALIZA A SERIAL
  lcd.begin (16,2); //SETA A QUANTIDADE DE COLUNAS(16) E O NÚMERO DE LINHAS(2) DO DISPLAY
  lcd.setBacklight(HIGH); //LIGA O BACKLIGHT (LUZ DE FUNDO)
  Serial1.begin(9600); //INICIALIZA A SERIAL PARA O ESP8266
  WiFi.init(&Serial1); //INICIALIZA A COMUNICAÇÃO SERIAL COM O ESP8266
  WiFi.config(IPAddress(10,0,0,120)); //COLOQUE UMA FAIXA DE IP DISPONÍVEL DO SEU ROTEADOR
  //INÍCIO - VERIFICA SE O ESP8266 ESTÁ CONECTADO AO ARDUINO, CONECTA A REDE SEM FIO E INICIA O WEBSERVER
  if(WiFi.status() == WL_NO_SHIELD){
    while (true);
  }
  while(status != WL_CONNECTED){
    status = WiFi.begin(ssid, pass);
  }
  server.begin();
  //FIM - VERIFICA SE O ESP8266 ESTÁ CONECTADO AO ARDUINO, CONECTA A REDE SEM FIO E INICIA O WEBSERVER

  delay(2000); //INTERVALO DE 2 SEGUNDO ANTES DE INICIAR
}

void loop(){
  WiFiEspClient client = server.available(); //ATENDE AS SOLICITAÇÕES DO CLIENTE
  if (client) { //SE CLIENTE TENTAR SE CONECTAR, FAZ
    buf.init(); //INICIALIZA O BUFFER
    while (client.connected()){ //ENQUANTO O CLIENTE ESTIVER CONECTADO, FAZ
      if(client.available()){ //SE EXISTIR REQUISIÇÃO DO CLIENTE, FAZ
        char c = client.read(); //LÊ A REQUISIÇÃO DO CLIENTE
        buf.push(c); //BUFFER ARMAZENA A REQUISIÇÃO
 
        //IDENTIFICA O FIM DA REQUISIÇÃO HTTP E ENVIA UMA RESPOSTA
        if(buf.endsWith("\r\n\r\n")) {
          sendHttpResponse(client);
          break;
        }
        if(buf.endsWith("GET /H")){ //SE O PARÂMETRO DA REQUISIÇÃO VINDO POR GET FOR IGUAL A "H", FAZ 
        }
        else{ //SENÃO, FAZ
          if (buf.endsWith("GET /L")) { //SE O PARÂMETRO DA REQUISIÇÃO VINDO POR GET FOR IGUAL A "L", FAZ

          }
        }
      }
    }
    client.stop(); //FINALIZA A REQUISIÇÃO HTTP E DESCONECTA O CLIENTE
  }


  DHT.read11(pinoDHT11); //LÊ AS INFORMAÇÕES DO SENSOR
  Serial.print("Umidade: " + String(DHT.humidity,0) + "% / Temperatura: " + String(DHT.temperature,0) + "C \n");
  lcd.setCursor(0,0); //SETA A POSIÇÃO DO CURSOR
  lcd.print("Umidade: " + String(DHT.humidity,0) +"%");
  lcd.setCursor(0,1); //SETA A POSIÇÃO DO CURSOR
  lcd.print("Temperatura: " + String(DHT.temperature,0) + "C");

  delay(2000); //INTERVALO DE 2 SEGUNDOS * NÃO DIMINUIR ESSE VALOR
}


//MÉTODO DE RESPOSTA A REQUISIÇÃO HTTP DO CLIENTE
void sendHttpResponse(WiFiEspClient client){
  client.println("HTTP/1.1 200 OK"); //ESCREVE PARA O CLIENTE A VERSÃO DO HTTP
  client.println("Content-Type: text/html"); //ESCREVE PARA O CLIENTE O TIPO DE CONTEÚDO(texto/html)
  client.println("");
  client.println("<!DOCTYPE HTML>"); //INFORMA AO NAVEGADOR A ESPECIFICAÇÃO DO HTML
  client.println("<html>"); //ABRE A TAG "html"
  client.println("<head>"); //ABRE A TAG "head"
  client.println("<link rel='icon' type='image/png' href='https://blogmasterwalkershop.com.br/arquivos/artigos/sub_wifi/icon_mws.png'/>"); //DEFINIÇÃO DO ICONE DA PÁGINA
  client.println("<link rel='stylesheet' type='text/css' href='https://blogmasterwalkershop.com.br/arquivos/artigos/sub_wifi/webpagecss.css' />"); //REFERENCIA AO ARQUIVO CSS (FOLHAS DE ESTILO)
  client.println("<title>MasterWalker Shop - Adaptador Conector 3.3V / 5V para o Modulo WiFi ESP8266 ESP-01</title>"); //ESCREVE O TEXTO NA PÁGINA
  client.println("</head>"); //FECHA A TAG "head"
  
  //AS LINHAS ABAIXO CRIAM A PÁGINA HTML
  client.println("<body>"); //ABRE A TAG "body"
  client.println("<img alt='masterwalkershop' src='https://blogmasterwalkershop.com.br/arquivos/artigos/sub_wifi/logo_mws.png' height='156' width='700' />"); //LOGO DA MASTERWALKER SHOP
  client.println("<p style='line-height:2'><font>Adaptador Conector 3.3V / 5V para o Modulo WiFi ESP8266 ESP-01</font></p>"); //ESCREVE O TEXTO NA PÁGINA
  client.println("<font>ESTADO ATUAL DO LED</font>"); //ESCREVE O TEXTO NA PÁGINA
  
  client.println("<p style='line-height:0'><font color='green'>LIGADO</font></p>"); //ESCREVE "LIGADO" NA PÁGINA
  client.println("<a href=\"/L\">APAGAR</a>"); //COMANDO PARA APAGAR O LED (PASSA O PARÂMETRO /L)
  
  client.println("<hr />"); //TAG HTML QUE CRIA UMA LINHA NA PÁGINA
  client.println("<hr />"); //TAG HTML QUE CRIA UMA LINHA NA PÁGINA
  client.println("</body>"); //FECHA A TAG "body"
  client.println("</html>"); //FECHA A TAG "html"
  delay(1); //INTERVALO DE 1 MILISSEGUNDO
}

