#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>  // Убедитесь, что установлена библиотека ArduinoJson

const char* ssid = "WIFIname";
const char* password = "passwordWIFI";
const char* telegramBotToken = "token";
const char* chatId = "Telegram id";
const char* message = "Mirmir the best!!!!!!!!!";
bool sendMessage = true;
unsigned long previousMillis = 0;
const long interval = 10000; // 10 секунд

void sendTelegramMessage(); // Прототип функции для отправки сообщения в Telegram
void checkForStopCommand(); // Прототип функции для проверки команды остановки


void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  checkForStopCommand();  // Проверяем наличие команды /stop

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval && sendMessage) {
    previousMillis = currentMillis;
    sendTelegramMessage();
  }
}

void sendTelegramMessage() {
  if (WiFi.status() == WL_CONNECTED && sendMessage) {
    HTTPClient http;
    String url = "https://api.telegram.org/bot" + String(telegramBotToken) + "/sendMessage?chat_id=" + String(chatId) + "&text=" + String(message);
    http.begin(url);
    int httpResponseCode = http.GET();
    if (httpResponseCode == 200) {
      Serial.println("Message successfully sent!");
    } else {
      Serial.printf("Failed to send message. HTTP response code: %d\n", httpResponseCode);
    }
    http.end();
  }
}

void checkForStopCommand() {
  HTTPClient http;
  String url = "https://api.telegram.org/bot" + String(telegramBotToken) + "/getUpdates";
  http.begin(url);
  int httpResponseCode = http.GET();
  if (httpResponseCode == 200) {
    String response = http.getString();
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, response);
    JsonArray result = doc["result"];
    for (JsonObject obj : result) {
      String text = obj["message"]["text"].as<String>();
      if (text == "/stop") {
        sendMessage = false;
        Serial.println("Received stop command. Stopping messages.");
        break;
      }
    }
  } else {
    Serial.printf("Failed to check for commands. HTTP response code: %d\n", httpResponseCode);
  }
  http.end();
}
