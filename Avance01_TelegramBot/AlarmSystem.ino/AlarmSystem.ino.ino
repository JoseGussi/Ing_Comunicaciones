#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>



int DISTANCIA = 0;
int pinEco=12;
int triggerPin=13;
int contador = 0;
int Buzzer = 14;

const char* ssid = "Redmi 10";
const char* password = "enfacundi";



// Inicializa Bot Telegram
#define BOTtoken "6366266446:AAEQk7YnWnVnrQsEHMVbhMVM8uSGhWl19HA"  // Tu Bot Token (Obtener de Botfather)


#define CHAT_ID "1076728481"

WiFiClientSecure client;

UniversalTelegramBot bot(BOTtoken, client);

long readUltrasonicDistance(int triggerPin, int echoPin)
{
  //Iniciamos el pin del emisor de reuido en salida
  pinMode(triggerPin, OUTPUT);
  //Apagamos el emisor de sonido
  digitalWrite(triggerPin, LOW);
  //Retrasamos la emision de sonido por 2 milesismas de segundo
  delayMicroseconds(2);
  // Comenzamos a emitir sonido
  digitalWrite(triggerPin, HIGH);
  //Retrasamos la emision de sonido por 2 milesismas de segundo
  delayMicroseconds(10);
  //Apagamos el emisor de sonido
  digitalWrite(triggerPin, LOW);
  //Comenzamos a escuchar el sonido
  pinMode(echoPin, INPUT);
  // Calculamos el tiempo que tardo en regresar el sonido
  return pulseIn(echoPin, HIGH);
}


void handleNewMessages(int numNewMessages, int distancia) {

  for (int i=0; i<numNewMessages; i++) {
    // Chat id of the requester
    String chat_id = String(bot.messages[i].chat_id);
    if (chat_id != CHAT_ID){
      bot.sendMessage(CHAT_ID, "Usuario no autorizado", "");
      continue;
    }
   
    String text = bot.messages[i].text;

    if (text == "/start") {
      bot.sendMessage(chat_id, "Bot iniciado", "");
      
    }


    if(text== "/revise")
    {
      if (contador==0)
      {
        bot.sendMessage(chat_id,"Todo chill nada se mueve por acá");

      }
      else{
      String msg = "Se detectó movimiento " + String(contador)+ " veces, venga que me roban ";
      bot.sendMessage(chat_id, msg,"");
      contador = 0;

      }
    }
  }
}


void setup() {
  Serial.begin(115200);
  pinMode(Buzzer, OUTPUT);
  

  // Conecta a red WiFi con SSID y password
  Serial.print("Conectado a ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  client.setCACert(TELEGRAM_CERTIFICATE_ROOT);


  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Muestra IP local 
  Serial.println("");
  Serial.println("WiFi conectado.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  bot.sendMessage(CHAT_ID, "BIenvenido Al bot de Alarma. Tienes opciones de activación y solicitud de datos \n 1. Empezar: /start \n 2. /revise", "");

}


void loop() {
  int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
  //Calculamos la distancia en cm
  DISTANCIA = 0.01723 * readUltrasonicDistance(triggerPin, pinEco);
  //Mostramos la disstancia
  Serial.println(DISTANCIA);
  delay(100);

  if (DISTANCIA<=50 || DISTANCIA >= 100) 
  {
    if(contador <=5){

    
      bot.sendMessage(CHAT_ID, "Alguien entró", "");
      Serial.print("La distancia es: ");
      Serial.println(DISTANCIA);
      contador+=1;
    }
    if(contador>3){
      Serial.println("ALARMA!!! WIUUU WIUUU WIUUU");
      digitalWrite(Buzzer, HIGH);
      delay(500);
      digitalWrite(Buzzer, LOW);
      delay(500);
      digitalWrite(Buzzer, HIGH);
      delay(500);
      digitalWrite(Buzzer, LOW);
      delay(500);
      digitalWrite(Buzzer, HIGH);
      delay(500);
      digitalWrite(Buzzer, LOW);
      
    }
  }

  while(numNewMessages) {
    handleNewMessages(numNewMessages, DISTANCIA);
    numNewMessages = bot.getUpdates(bot.last_message_received + 1);
  }
}
