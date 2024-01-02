#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>                        // firebase kütüphanesini 
#include "ThingSpeak.h"                              // thingspeak kütüphanesi
#include <ESP8266HTTPClient.h>                        //whatsapp
#include <UrlEncode.h>                                 //UrlEncode


int ADC_okunan=0;           // adc'den okunana ham değer
float sensor_gerilim=0.0;   // sensorden okunan degerin gerilim karsiligi mV cinsinden
float sicaklik=0.0;         // okunan sıcaklık değeri

const char *ssid = "*******";                 // bağlanılacak olan kablosuz ağın SSID si
const char *password = "*******";                  // bağlanılacak olan kablosuz ağın şifresi

unsigned long kanal_numarasi = *****;   //thingspeak kanal numarası
const char * myWriteAPIKey = "*******";  //thingspeakteki writeAPIKey

String phoneNumber = "+*********"; 
String apiKey = "*********"; //wp bot apiKey

WiFiClient  wifi_istemci;

#define FIREBASE_HOST "******************"     // veritabanının linki
#define FIREBASE_AUTH "***********************"              // veritabanı secret key(şifre)
//------------------------------------------------------------------------------------
void setup() 
{
  pinMode(D0,OUTPUT);                             // D0 pinini çıkış yap, LED bağlı

  
  Serial.begin(9600);                             //Seri iletişim hızı 9600 bps olarak ayarlanarak başlatılıyor.
  delay(1000);
  Serial.println("Wifi agina baglaniliyor");

  WiFi.mode(WIFI_STA);                           //ESP8266 istasyon moduna alınıyor. 
  WiFi.begin(ssid, password);                    //SSID ve şifre girilierek wifi başlatılıyor.

  //------- Wifi ağına bağlanıncaya kadar beklenilen kısım ------------
  while (WiFi.status() != WL_CONNECTED) 
  { 
    delay(500);                                 // Bu arada her 500 ms de bir seri ekrana yan yana noktalar yazdırılarak
    Serial.print(".");                          //görsellik sağlanıyor.
  }
  //-------------------------------------------------------------------

  Serial.println("");                           //Bir alt satıra geçiliyor.
  Serial.println("Bağlantı sağlandı...");       //seri ekrana bağlantının kurulduğu bilgisi gönderiliyor.

  Serial.print("Alınan IP addresi: ");          // kablosuz ağdan alınan IP adresi
  Serial.println(WiFi.localIP());               // kablosuz ağdan alınan IP adresi

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH); // firabase i başlat 
  ThingSpeak.begin(wifi_istemci);  // Initialize ThingSpeak
  
  
  
  
}
//------------------------------------------------------------------------------------
void loop() 
{
  sicaklik_olc();
  // Int  yazdırma, Sicaklik etiketi altına yazdırma
  Firebase.setInt("Sicaklik", sicaklik);
// handle error
 
  if (Firebase.failed()) {
      Serial.print("yazdirma hatasi:");
      Serial.println(Firebase.error());  
      return;}
delay(1000);
  
  if(sicaklik>=25){  //eşik değeri 25 olarak belirledim
    digitalWrite(D0, HIGH);
    sendMessage("Sıcaklık değeri 25'i aştı!");
  }
  else {
   digitalWrite(D0, LOW);
  }

   ThingSpeak.writeField(kanal_numarasi, 1, sicaklik, myWriteAPIKey); //thingspeake verileri yazar
  
  
}
void sicaklik_olc()
{
  ADC_okunan=analogRead(A0);              // ADC yi oku, A0 girişine bağlı LM35
  sensor_gerilim=(ADC_okunan*1000.0)/1023.0; 
  sicaklik=sensor_gerilim/10.0;  
}
void sendMessage(String message){

  // Data to send with HTTP POST
  String url = "http://api.callmebot.com/whatsapp.php?phone=" + phoneNumber + "&apikey=" + apiKey + "&text=" + urlEncode(message);
  HTTPClient http;
  http.begin(url);

  // Specify content-type header
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  
  // Send HTTP POST request
  int httpResponseCode = http.POST(url); //HTTP gönderi isteğini gönderir ve yanıt kodunu kaydeder:
  if (httpResponseCode == 200){
    Serial.print("Message sent successfully");
  }
  else{
    Serial.println("Error sending the message");
    Serial.print("HTTP response code: ");
    Serial.println(httpResponseCode);
  }

  // Free resources
  http.end();
}
