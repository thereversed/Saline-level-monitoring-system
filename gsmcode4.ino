
#define USE_ARDUINO_INTERRUPTS true 
#include <PulseSensorPlayground.h> 
const int PulseWire = 0;       // PulseSensor PURPLE WIRE connected to ANALOG PIN 0
PulseSensorPlayground pulseSensor;
#include<SoftwareSerial.h>
SoftwareSerial gsm(7,8);
        
int Threshold = 550; 
int Instructions_view = 500;
//dallas
#include <OneWire.h>
#include <DallasTemperature.h>
const int SENSOR_PIN = 2; // Arduino pin connected to DS18B20 sensor's DQ pin
OneWire oneWire(SENSOR_PIN);         // setup a oneWire instance
DallasTemperature tempSensor(&oneWire); // pass oneWire to DallasTemperature library
float tempCelsius;    // temperature in Celsius

#include <HX711_ADC.h>  
#include <Wire.h>
#include <LiquidCrystal_I2C.h> // LiquidCrystal_I2C library
HX711_ADC LoadCell(4, 5); // dt pin, sck pin
LiquidCrystal_I2C lcd(0x27, 20, 4); // LCD HEX address 0x27
int taree = 6;
int a = 0;
float b = 0;
const int rled=11;
const int yled=12;
const int bled=13;
  int buzzer=9;

  // gsm send message subroutine
  
 
void setup() {
  
  Serial.begin(115200);
  gsm.begin(115200);
    delay(10000);
  //heart bpm
   pulseSensor.analogInput(PulseWire);   
  //pulseSensor.blinkOnPulse(LED_3); //--> auto-magically blink Arduino's LED with heartbeat.
  pulseSensor.setThreshold(Threshold); 

  if (pulseSensor.begin())
  {
    Serial.println("  pulseSensor Object Created !");  
  }
  //hx711
  pinMode (taree, INPUT_PULLUP);
  LoadCell.begin(); // start c  HX711
  LoadCell.start(1000);  
   LoadCell.setReverseOutput(); 
   pinMode(13, OUTPUT); 
   pinMode(12, OUTPUT); 
   pinMode(11, OUTPUT); 
   pinMode(9,OUTPUT);

 LoadCell.setCalFactor(380.62); // Calibarate your LOAD CELL with 100g weight, and change the value according to readings
   
  lcd.begin(); // begins connection to the LCD module
  lcd.backlight(); // turns on the backlight
  lcd.setCursor(1, 0); // set cursor to first row
  lcd.print("Saline scale : "); // print out to LCD
 
delay(2000);
lcd.clear();
}
void loop()
{ 
  //dallas temp
  int myBPM = pulseSensor.getBeatsPerMinute(); //--> Calls function on our pulseSensor object that returns BPM as an "int". "myBPM" hold this BPM value now.
  
  if (Instructions_view < 500) {
    Instructions_view++;
  }
  
  if (Instructions_view > 499) {
    lcd.setCursor(0,1);
    lcd.print("NO Heart Beat");  
    delay(1000);
    lcd.clear();
    delay(500);
  }
  if (pulseSensor.sawStartOfBeat()) 
  { //--> If test is "true", then the following conditions will be executed.
    Serial.println("♥  A HeartBeat Happened ! "); //--> Print a message "a heartbeat happened".
    Serial.print("BPM: "); //--> Print phrase "BPM: " 
    Serial.println(myBPM); //--> Print the value inside of myBPM.
      lcd.setCursor(0,3);
    lcd.print("Heart Rate");
    lcd.setCursor(11,3);
    lcd.print(": ");
    lcd.print(myBPM);
//    lcd.print(" ");
    lcd.setCursor(17,3);
    lcd.print("BPM");      
    Instructions_view = 0; 
  } 

  // dallas temperature sensor

   tempSensor.requestTemperatures();            
  tempCelsius = tempSensor.getTempCByIndex(0);   
   Serial.print("Temperature: ");
  Serial.print(tempCelsius);    // print the temperature in Celsius
  Serial.println("°C");
         // separator between Celsius and Fahrenheit
  
 
  lcd.setCursor(0,2);
  lcd.print("temperatre :");
  lcd.setCursor(12,2);
  lcd.print(tempCelsius);
   lcd.setCursor(18,2);
  lcd.print("C");

  //taring and Hx711
  digitalWrite(rled,LOW);
  digitalWrite(yled,LOW);
  digitalWrite(bled,LOW);
  lcd.setCursor(1, 0); // set cursor to first row
  lcd.print("Digital Scale "); // print out to LCD 
  LoadCell.update(); // retrieves data from the load cell
  float i = LoadCell.getData()-39; // get output value
 
  
  lcd.setCursor(1, 1); // set cursor to secon row
  lcd.print(i, 1); // print out the retrieved value to the second row
  lcd.print("g ");
  float z = (i*1025/1000);
  lcd.setCursor(9, 1);
  lcd.print(z, 2);
  lcd.print("mL");
  Serial.print("Load_cell output val: ");
  Serial.println(i);


 if(i < 300 && i>200)
  {
    Serial.println("saline had gone through 300g");
    //sendx1(tempCelsius, myBPM);
    digitalWrite(yled,HIGH);
   }
   
  if(i < 200 && i> 70  )
  {
    Serial.println("saline had gone through 200g");
    //sendx2(tempCelsius, myBPM);
    digitalWrite(yled,LOW);
    digitalWrite(bled,HIGH);
    
  }
     
  if(i < 70)
       
  {
    Serial.println("saline had almost end");
    sendx3(tempCelsius, myBPM);
    digitalWrite(bled,LOW);
    digitalWrite(rled,HIGH);
    digitalWrite(buzzer,HIGH);
    delay(100 );
    digitalWrite(buzzer,LOW);
    delay(100 );
   }

}


void sendx3(float t, float y)
{
 
  gsm.println("AT+CMGF=1");
  delay(1000);
  gsm.println("AT+CMGS=\"+919407939984\"\r"); //replace x by your number
  delay(1000);
  gsm.println("saline level almost empty, need for re fill");
  delay(100);
   gsm.println("Temperature and heart beat :");
  delay(100);
    gsm.println(t);
  delay(100);
  gsm.println(y);
  delay(100);
  gsm.println((char)26);
  delay(30000);

}
