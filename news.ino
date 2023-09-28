#include <M5StickCPlus.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>
#include "ArduinoJson.h"

#include <efontEnableJa.h> //efontはめちゃくちゃメモリ食うので使いたい日本語フォントを先においてメモリ節約
#include "efontESP32.h"  
#include <efont.h>

WiFiMulti WiFiMulti;
HTTPClient http;

const char* api = "https://newsapi.org/v2/top-headlines?apiKey=7ffc5a6a5c64457a9b3d75eefd437a23&country=jp"; //使いたいAPIのアドレス


void setup() {

  M5.begin();
  M5.Lcd.setRotation(3);
  M5.Lcd.setCursor(0, 0, 2);

  WiFiMulti.addAP("7C131DBFE04D-2G","dd29ndx7st2fes"); // 使いたい2.4GHz帯のWi-Fi
  M5.Lcd.print("Connecting");


  while(WiFiMulti.run() != WL_CONNECTED) { 
    M5.Lcd.print(".");
    delay(1000);
  }

  delay(500);
}

void loop() {
    String title1;
  String title2;
  String data1;
  String data2;
  String ptitle;

  if ((WiFi.status() == WL_CONNECTED)) { 
    M5.update();
    M5.Lcd.setCursor(0,0);
    
    http.begin(api); //引数内のURLにアクセス
    int httpCode = http.GET(); //リクエストを送信
    
    if (httpCode > 0) { //リターンコードが負の数だとエラーらしい
        
        String payload = http.getString();//レスポンスをすべて格納 
        DynamicJsonDocument doc(1024); //動的にメモリを確保してるらしい
        deserializeJson(doc,payload); //直列化されたデータを構造化されたデータに直しているらしい
        JsonObject obj = doc.as<JsonObject>(); // 構造化されたデータをJsonObjectに代入

         String title1 = doc["articles"][0]["title"];//最新のニュース
         String data1 = doc["articles"][0]["publishedAt"];

         String title2 = doc["articles"][1]["title"];//ひとつ前のニュース
         String data2 = doc["articles"][1]["publishedAt"];

        String title = title1;
        String data = data1;


        if(M5.BtnA.isPressed()){//ホームボタンが押されている間はひとつ前のニュースを表示

          title = title2;
          data = data2;

        }

        if(title != ptitle){
          
          M5.Lcd.fillScreen(BLACK);
          M5.Lcd.setCursor(0,0);
          printEfont((char*)title.c_str());
          M5.Lcd.print("\n"); 
          printEfont((char*)data.c_str());
          ptitle = title;
           
        }
      
    }
      else {
      M5.Lcd.println("error");
      return;
    }
    http.end(); //HTTP通信を終了
  }
}