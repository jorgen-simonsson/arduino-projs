#include <Arduino.h>
#include <SPI.h>
#include <WiFi101.h>
#include <driver/source/nmasic.h>

#pragma region Type definitions
#define MAX_VARIABLE_CNT_IN_PAGE 30

typedef struct 
{
  String htmlTxt;
  int variableCount;
  char* variableStrings[MAX_VARIABLE_CNT_IN_PAGE];
} HtmlStruct;

#define PAIR_CLAYS  0
#define DOUBLE      1
#define SINGLE_CLAY 2

typedef struct 
{
  int type;
  int primClay;
  int secClay;
  char roundText[4];
} RoundStruct;

typedef struct 
{
  RoundStruct rounds[5];
} StationStruct;

typedef struct 
{
  int roundIndex;
  int stationIndex;
} CurrentRoundStruct;

typedef struct 
{
  int noOfShooters;
  int noOfRoundsOnStation;
  CurrentRoundStruct currentRound;
  StationStruct stations[5];
} CompakStruct;

#pragma endregion
#pragma region static variables

char ssid[] = "COMPAK_NET";
int keyIndex = 0;                 // your network key Index number (needed only for WEP)
WiFiServer server(80);
WiFiClient http_client;
int wifi_status = WL_IDLE_STATUS;
int http_port = 80;
byte relay_pump_status = 0;
int ledState = LOW;
unsigned long previousMillis = 0;
const long interval = 1000;           // blink interval (milliseconds)
CompakStruct compak;
#pragma endregion

// Html  content
#include <MainMenu.h>
#include <DropDown.h>
#include <table.h>

#pragma region Prototypes
void serveBlinkingLed();;
void printWiFiStatus();
void permanentError();
void wifi_connect_as_ap();
void _debug_wifi_status();
void process_http_requests();
void process_http_request(String path);
byte _is_a_get_request(String txt);
String _get_request_path(String txt); 
String _html_show_on_off_status(int status); 
void TableMenu();
void PutStringInTable(int station, int round, char* text);
void DropMenu();
void mainMenu();
void CheckForLatestFirmware();
void WriteLineByLine(HtmlStruct page);
void WriteHtmlPage(HtmlStruct page);
String RoundText(int station, int round);
#pragma endregion



void setup() 
{

  // put your setup code here, to run once:
  pinMode(LED_BUILTIN, OUTPUT);
  while (!Serial);
  delay(1000);
  Serial.begin(9600);

  if (WiFi.status() == WL_NO_SHIELD)
  {
      Serial.println("WiFi shield not present");
      while (true);       // don't continue
  }
  wifi_connect_as_ap();
  delay(1000);
  CheckForLatestFirmware();
}

void loop() 
{
  if (WiFi.status() != wifi_status)
  {
    wifi_status = WiFi.status();
    if (wifi_status == WL_AP_CONNECTED)
    {
       Serial.println("Device connected to AP");
       server.begin();
    }
    else
      Serial.println("Device disconnected from AP");
  }
  process_http_requests();
  serveBlinkingLed();
}

void serveBlinkingLed()
{
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) 
  {
    // save the last time you blinked the LED
    previousMillis = currentMillis;

    // if the LED is off turn it on and vice-versa:
    if (ledState == LOW) 
    {
      ledState = HIGH;
    } else 
    {
      ledState = LOW;
    }
    digitalWrite(LED_BUILTIN, ledState);    
  }
}



void process_http_requests() 
{
  // Listen for incoming http_clients
  http_client = server.available();
  byte tmp_request_is_a_get_request = 0;
  String tmp_get_request_line = "";

  if (http_client) 
  {                             // if you get a http_client,
    Serial.println("New client connected.");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the http_client
    while (http_client.connected()) 
    {            // loop while the http_client's connected
      if (http_client.available()) 
      {             // if there's bytes to read from the http_client,
        char c = http_client.read();             // read a byte, then
        // Serial.write(c);                    // print it out the serial monitor
        if (c == '\n') 
        {
          // If the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) 
          {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            http_client.println("HTTP/1.1 200 OK");
            http_client.println("Content-type:text/html");
            http_client.println();
            if (tmp_request_is_a_get_request == 1) 
            {
              process_http_request(_get_request_path(tmp_get_request_line));
            }
            break;
          }
          
          // We don't really care about other headers .. just the /GET ones
          if (_is_a_get_request(currentLine)) 
          {
            tmp_request_is_a_get_request = 1;
            tmp_get_request_line = currentLine;
          }
          // Clear the currentLine buffer
          currentLine = "";
        }
        else if (c != '\r') 
        {
          // Add to the currentLine buffer
          currentLine += c;
        }
      }
    }
    // close the connection:
    http_client.stop();
    Serial.println("Client disconnected.");
  }
}


void process_http_request(String path) 
{
  if (path == "/") 
  {
    mainMenu();
  } 
  else if (path == "/pump_relay_off") 
  {
    relay_pump_status = 0;
    mainMenu();
  } 
  else if (path == "/pump_relay_on") 
  {
    relay_pump_status = 1;
    mainMenu();
  } 
  else if (path == "/drop") 
  {
    DropMenu();
  } 
  else if (path == "/table") 
  {
    TableMenu();
  } 

  else 
  {
    http_client.println("404");
  }
}
void TableMenu()
{
  PutStringInTable(3,3,(char*)"<strong>C+B</strong>");
  WriteHtmlPage(tableStruct);
}

void DropMenu()
{
  WriteHtmlPage(dropStruct);
}


void mainMenu()
{
  if (relay_pump_status ==0)
  {
    mainMenuStruct.variableStrings[0] = (char*) "red";
    mainMenuStruct.variableStrings[1] = (char*) "OFF";
  }
  else
  {
    mainMenuStruct.variableStrings[0] = (char*) "green";
    mainMenuStruct.variableStrings[1] = (char*) "ON";
  }
  WriteHtmlPage(mainMenuStruct);
}


byte _is_a_get_request(String txt) 
{
  String tmp = _get_request_path(txt);
  if (tmp == "N/A") 
  {
    return 0;
  }
  return 1;
}

String _get_request_path(String txt) 
{
  if (txt.endsWith("HTTP/1.1")) 
  {
    txt = txt.substring(0, txt.indexOf("HTTP/1.1"));
    if (txt.startsWith("GET")) 
    {
      txt = txt.substring(3);
      txt.trim();
      return txt;
    }
  }
  return "N/A";
}

void wifi_connect_as_ap() 
{
  // Is it IDLE ?
  if (wifi_status == WL_IDLE_STATUS) 
  {
    // Try to connect
    wifi_status = WiFi.beginAP(ssid);
    // Wait 10 seconds for connection
    delay(4000);
    _debug_wifi_status();
    // Is it connected?
    wifi_connect_as_ap();
  }
}


void printWiFiStatus() 
{
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
  Serial.print("signal strength (RSSI):");
  Serial.print(WiFi.RSSI());
  Serial.println(" dBm");
}
void permanentError()
{
  while(true)
  {
    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(100);                       // wait for a second
    digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
    delay(100);                       // wait for a second

  }
}


void _debug_wifi_status() 
{
    // print the SSID of the network you're attached to:
    Serial.print("SSID: ");
    Serial.println(WiFi.SSID());
  
    // print your WiFi shield's IP address:
    IPAddress ip = WiFi.localIP();
    Serial.print("IP Address: ");
    Serial.println(ip);
  
    // print the received signal strength:
    long rssi = WiFi.RSSI();
    Serial.print("signal strength (RSSI):");
    Serial.print(rssi);
    Serial.println(" dBm");
    // print where to go in a browser:
    Serial.print("To see this page in action, open a browser to http://");
    Serial.print(ip);
    Serial.print(":");
    Serial.print(http_port);
    Serial.println();
}

void CheckForLatestFirmware()
{

  // Print firmware version on the shield

  String fv = WiFi.firmwareVersion();

  String latestFv;

  Serial.print("Firmware version installed: ");

  Serial.println(fv);

  if (REV(GET_CHIPID()) >= REV_3A0) 
  {

    // model B
    Serial.println("This chip is a model 'B'");

    latestFv = WIFI_FIRMWARE_LATEST_MODEL_B;

  } else 
  {

    // model A
    Serial.println("This chip is a model 'A'");

    latestFv = WIFI_FIRMWARE_LATEST_MODEL_A;

  }

  // Print required firmware version

  Serial.print("Latest firmware version available : ");

  Serial.println(latestFv);

  // Check if the latest version is installed

  Serial.println();

  if (fv >= latestFv) 
  {

    Serial.println("Check result: PASSED");

  } else 
  {

    Serial.println("Check result: NOT PASSED");

    Serial.println(" - The firmware version on the shield do not match the");

    Serial.println("   version required by the library, you may experience");

    Serial.println("   issues or failures.");
  }


}

void WriteLineByLine(HtmlStruct page)
{
  int pos=0;
  int line = 0;
  char newline = '\n';
  int index;
  int variableCnt = page.variableCount;
  do 
  {
    index = page.htmlTxt.indexOf(newline, pos);
    if (index != -1)
    {
        String linestr = page.htmlTxt.substring(pos, index+1);
        for (int i = 0; i< variableCnt; ++i)
        {

          char searchStr[20];
          sprintf(searchStr, "$@%03d", i);
          Serial.print("Search string:");
          Serial.println(searchStr);
          linestr.replace(searchStr, page.variableStrings[i]);
        }
        Serial.print("Line no:");
        Serial.print(line);
        Serial.print(" ");
        Serial.print(linestr);
    }
    pos = index+1;
    ++line;
  } while (index != -1);
}

void WriteHtmlPage(HtmlStruct page)
{
  int pos=0;
  char newline = '\n';
  int index;
  do 
  {
    index = page.htmlTxt.indexOf(newline, pos);
    if (index != -1)
    {
        String linestr = page.htmlTxt.substring(pos, index+1);
        for (int i = 0; i< page.variableCount; ++i)
        {
          char searchStr[20];
          sprintf(searchStr, "$@%03d", i);
          linestr.replace(searchStr, page.variableStrings[i]);
        }
        http_client.print(linestr);
    }
    pos = index+1;
  } while (index != -1);
}

void MoveToNextRound()
{
  ++compak.currentRound.stationIndex;
  if(compak.currentRound.stationIndex >= compak.noOfShooters)
  {
    compak.currentRound.stationIndex=0;
    ++compak.currentRound.roundIndex;
    if (compak.currentRound.roundIndex >= compak.noOfRoundsOnStation)
        compak.currentRound.roundIndex =0;
  }
}

void SetTextInCompakStructure()
{

}

void RoundText(int station, int round, char*result)
{

  if(station <1 || station >5)
    return ;
  if(round <1 || round >5)
    return ;

  int type = compak.stations[station-1].rounds[round-1].type;
  if (type == PAIR_CLAYS)
  {
      result[0] = 0x41+compak.stations[station-1].rounds[round-1].primClay;
      result[1] = '+';
      result[2] = 0x41+compak.stations[station-1].rounds[round-1].secClay;
  }
  if (type == DOUBLE)
  {
      result[0] = 0x41+compak.stations[station-1].rounds[round-1].primClay;
      result[1] = '|';
      result[2] = 0x41+compak.stations[station-1].rounds[round-1].secClay;
  }
  if (type == SINGLE_CLAY)
  {
      result[0] = 0x41+compak.stations[station-1].rounds[round-1].primClay;
      result[1] = ' ';
      result[2] = ' ';
  }
  result[3]=0;
}

void PutRoundsInHtmlPage()
{
  char text[4];

    for(int station=1 ; station<=5 ;++station)
    {
      for(int round=1 ; round<=5 ;++round)
      {
        PutStringInTable(station, round, (char*) RoundText(station, round))    
      }
    }
}
void PutTextInTable(int station, int round, char* text)
{ 
  if ((station > 5 ) || (station < 1 ))
    return;
  if ((round > 5 ) || (round < 1 ))
    return;
  int index = (station-1) + ((round-1) *5);
  tableStruct.variableStrings[index] = text;
}
