 #include <PubSubClient.h>
#include <WiFi.h>
#include "EmonLib.h" 

//Thingsboard
#define WIFI_AP "LCA-ROBOTICA"
#define WIFI_PASSWORD "RedeRoboticaLca2015"
#define TOKEN "GKiq3WyVJ1ul6qlt2ckG"
//Medicao
#define Vrede 220     // define a tensão RMS da rede(valor lido com multimetro)
#define V_calibration_1  98.124       //Rd=7.8 e Rt=12.58 Vcal_1 = RT*RD
#define V_calibration_2  92.742       //Rd=7.8 e Rt=11.89 Vcal_2 = RT*RD
#define V_calibration_3  112.164      //Rd=7.8 e Rt=112.164 Vcal_3 = RT*RD
#define I_calibration  42.5         // 2000/47 (ganho sensor/ resistor carga) fator de calibração da corrente - leia minhas dicas no tutorial
#define emonTxV3

// Define pinos Corrente
#define InputSCT_1 36 // ESP32 VP
#define InputSCT_2 39 // ESP32 VN
#define InputSCT_3 34 // ESP32 D34
#define InputSCT_4 35 // ESP32 D35

// Define pinos Corrente
#define InputSVT_1 32 // ESP32  D32
#define InputSVT_2 33 // ESP32  D33
#define InputSVT_3 25 // ESP32  D25


//Parâmetros Thingsboard
char thingsboardServer[] = "demo.thingsboard.io";

//Parâmetros Medicao

//Corrente
double Irms_value_1 = 0 ;
double Irms_value_2 = 0 ;
double Irms_value_3 = 0 ;
double Irms_value_4 = 0 ;
double Irms_1 = 0 ;
double Irms_2 = 0 ;
double Irms_3 = 0 ;
double Irms_4 = 0 ;

//Tensão
double Vrms_value_1 = 0;
double Vrms_value_2 = 0;
double Vrms_value_3 = 0;
double Vrms_1 = 0;
double Vrms_2 = 0;
double Vrms_3 = 0;

//Potência
double Preal_value_1 = 0;
double Preal_value_2 = 0;
double Preal_value_3 = 0;
double Preal_1 = 0;
double Preal_2 = 0;
double Preal_3 = 0;
double Saparente_1 = 0;
double Saparente_2 = 0;
double Saparente_3 = 0;
double Fpotencia_1 = 0;
double Fpotencia_2 = 0;
double Fpotencia_3 = 0;

float Vrms_config = 0;
float Irms_config = 0;
float Vrms_config_2 = 0;
float Irms_config_2 = 0;
float Vrms_config_3 = 0;
float IRms_config_3 = 0;

float Tempo = 0;
EnergyMonitor EnerMonitor;       //Cria uma instancia da classe EnergyMonitor

WiFiClient wifiClient;
PubSubClient client(wifiClient);

int status = WL_IDLE_STATUS;
unsigned long lastSend;

void setup()

{
  Serial.begin(115200);
  delay(10);
  InitWiFi();
  client.setServer( thingsboardServer, 1883 );
  lastSend = 0;

  analogReadResolution(ADC_BITS);
  Serial.print("ADC BITS:");
  Serial.println(ADC_BITS);
  Serial.println("Inicia SW");
  delay(1000);
  EnerMonitor.current(InputSCT_1, I_calibration);// configura pino SCT e fator de calibração  
  //Pino, calibracao - Cur Const= Ratio/BurdenR. 2000/R = 60
  Irms_config = EnerMonitor.current(InputSCT_2, I_calibration);
  Irms_config_2 = EnerMonitor.current(InputSCT_3, I_calibration);
  Irms_config_3 = EnerMonitor.current(InputSCT_4, I_calibration);

  
  Vrms_config = EnerMonitor.voltage(InputSVT_1, V_calibration_1, 0);   // configura pino SCT e fator de calibração
  Vrms_config_2 = EnerMonitor.voltage(InputSVT_2, V_calibration_2, 0);   // configura pino SCT e fator de calibração
  Vrms_config_3 = EnerMonitor.voltage(InputSVT_3, V_calibration_3, 0);   // configura pino SCT e fator de calibração
}

void loop()
{
  if ( !client.connected() ) {
    reconnect();
  }

  if ( millis() - lastSend > 1000 ) { // Update and send only after 1 seconds
    getAndSendTemperatureAndHumidityData();
    lastSend = millis();
  }

  client.loop();
}

void getAndSendTemperatureAndHumidityData()
  {
  Serial.println("Collecting tensao e corrente");
  //EnerMonitor.calcIrms(1000);
  EnerMonitor.calcVI(50,1000); // ciclos e milisegundos de amostra
  Vrms_value_1 = EnerMonitor.Vrms_config;
  Irms_value_1 =  EnerMonitor.Irms_config;
  //Preal_value_1 = EnerMonitor.apparentPower;
  
  Vrms_value_2 = EnerMonitor.Vrms_config_2;
  Irms_value_2 =  EnerMonitor.Irms_config_2;
//  Preal_value_2 = EnerMonitor.apparentPower;
  
  Vrms_value_3 = EnerMonitor.Vrms_config_3;
  Irms_value_3 =  EnerMonitor.Irms_config_3;
  //Preal_value_3 = EnerMonitor.apparentPower;
  
  //Irms_value_4 =  EnerMonitor.Irms;
  
  Serial.println(Irms_value_1);
  Serial.println(Vrms_value_1);
  
  Serial.println(Irms_value_2);
  Serial.println(Vrms_value_2);
  
  Serial.println(Irms_value_3);
  Serial.println(Vrms_value_3);
  
  Serial.println(Irms_value_4);
  
  float Irms_1 = Irms_value_1;
  float Vrms_1 = Vrms_value_1; 
  float Preal_1 = Preal_value_1;
  
  float Ikrms_2 = Irms_value_2;
  float Vkrms_2 = Vrms_value_2; 
  float Preal_2 = Preal_value_2;
  
  float Irms_3 = Irms_value_3;
  float Vrms_3 = Vrms_value_3; 
  float Preal_3 = Preal_value_3;
  
  
  float Irms_4 = Irms_value_4;
  
    unsigned long tempo = millis();
  
    // Check if any reads failed and exit early (to try again).
    if (isnan(Irms_1) || isnan(Vrms_1)) {
      Serial.println("Failed to read from sensor!");
      return;
    }

  Serial.print("Corrente_1: ");
  Serial.print(Irms_1);
  Serial.print("Tensao_1: ");
  Serial.print(Vrms_1);
  Serial.print("Potência_1: ");
  Serial.print(Preal_1);

  Serial.print("Corrente_2: ");
  Serial.print(Irms_2);
  Serial.print("Tensao_2: ");
  Serial.print(Vrms_2);
  Serial.print("Potência_2: ");
  Serial.print(Preal_2);

  Serial.print("Corrente_3: ");
  Serial.print(Irms_3);
  Serial.print("Tensao_3: ");
  Serial.print(Vrms_3);
  Serial.print("Potência_3: ");
  Serial.print(Preal_3);

  Serial.print("Corrente_4: ");
  Serial.print(Irms_4);



  
  String corrente_1 = String(Irms_1);
  String tensao_1 = String(Vrms_1);
  String potencia_1 = String(Preal_1);
  
  
  String corrente_2 = String(Ikrms_2);
  String tensao_2 = String(Vrms_2);
  String potencia_2 = String(Preal_2);
  
  String corrente_3 = String(Irms_3);
  String tensao_3 = String(Vrms_3);
  String potencia_3 = String(Preal_3);
  
  String corrente_4 = String(Irms_4);

  // Just debug messages
  Serial.print( "Sending Corrente e tensão : [" );
  Serial.print( corrente_1 ); Serial.print( "," );
  Serial.print( tensao_1 ); Serial.print( "," );
  Serial.print( potencia_1 ); Serial.print( "," );
  Serial.print( tempo );

  Serial.print( corrente_2 ); Serial.print( "," );
  Serial.print( tensao_2 ); Serial.print( "," );
  Serial.print( potencia_2 ); Serial.print( "," );

  Serial.print( corrente_3 ); Serial.print( "," );
  Serial.print( tensao_3 ); Serial.print( "," );
  Serial.print( potencia_3 ); Serial.print( "," );

  Serial.print( corrente_4 ); Serial.print( "," );
  
  Serial.print( "]   -> " );

  // Prepare a JSON payload string
  
    String payload = "{";
    payload += "\"corrente_1\":"; payload += corrente_1; payload += ",";
    payload += "\"tensao_1\":"; payload += tensao_1; payload += ",";
    payload += "\"potencia_1\":"; payload += potencia_1; payload += ",";
    payload += "\"tempo\":"; payload += tempo;
  
  // Send payload
  char attributes[100];
  payload.toCharArray( attributes, 100 );
  client.publish( "v1/devices/me/telemetry", attributes );
  Serial.println( attributes );

}

void InitWiFi()
{
  Serial.println("Connecting to AP ...");
  // attempt to connect to WiFi network

  WiFi.begin(WIFI_AP, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to AP");
}


void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    status = WiFi.status();
    if ( status != WL_CONNECTED) {
      WiFi.begin(WIFI_AP, WIFI_PASSWORD);
      while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
      }
      Serial.println("Connected to AP");
    }
    Serial.print("Connecting to ThingsBoard node ...");
    // Attempt to connect (clientId, username, password)
    if ( client.connect("ESP32", TOKEN, NULL) ) {
      Serial.println( "[DONE]" );
    } else {
      Serial.print( "[FAILED] [ rc = " );
      Serial.print( client.state() );
      Serial.println( " : retrying in 5 seconds]" );
      // Wait 5 seconds before retrying
      delay( 5000 );
    }
  }
}
