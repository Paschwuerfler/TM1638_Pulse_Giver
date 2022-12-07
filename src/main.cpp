#include "Arduino.h"
#include "TM1638.h"

bool debug = false; 

const int OUTPUT_PIN = 6; 
const int LED_PIN = LED_BUILTIN; 
const int EXT_Trigger = 7;

const int STB = 8;
const int CLK = 9;
const int DIO = 10;
TM1638 modul(DIO, CLK, STB);

unsigned long off_duration_ms = 2000; 
int dec_pos = 0; 
unsigned long on_duration_ms = 100; 
unsigned long count = 20;




unsigned long loop_dur_ms = 0; 


void smart_decrease(unsigned long * value) { //value is given in ms
  unsigned long val = *value; 
  
  if(val < 20) {
    val = 10; 
    dec_pos = 0; 
  } else if(val < 1000) { //smaller 1 second 
    val -= 10; 
    dec_pos = 0; 
  } else if(val < 10000) { //smaller 10 seconds 
    val -= 100;
    dec_pos = 1; 
  } else if(val <= 100000) { //smaller 100 seconds
    val -= 1000;
    dec_pos = 2; 
  }

  *value  = val; 

  if(debug) {
    Serial.print("Dec: ");
    Serial.println(*value); 
  }
}



void smart_increase(unsigned long * value) { //value is given in ms
  unsigned long val = *value; 
  
  if(val < 1000) { //smaller 1 second 
    val += 10; 
    dec_pos = 0; 
  } else if(val < 10000) { //smaller 10 seconds 
    val += 100;
    dec_pos = 1; 
  } else if(val < 100000) { //smaller 100 seconds
    val += 1000;
    dec_pos = 2; 
  } 

  if(val > 100000) {
    val = 100000; 
  }

  *value  = val; 

  if(debug) {
    Serial.print("inc: ");
    Serial.println(*value); 
  }

  
}



bool getBit(byte a, byte n) {
  return ((a >> n)  & 0x01); 
}

void setDispHelper(unsigned long *value, byte pos1, byte pos2) {
  unsigned long val = *value; 


  byte pos1d = 0; 
  byte pos2d = 0; 


  if(val < 1000) {

    if(debug) Serial.print("c1 "); 
    pos1d = (val / 100) % 10; 
    pos2d = (val / 10) % 10; 
    dec_pos = 0; 

  }
  else if (val < 10000) { //This can be simplified but a bodge is faster 

  if(debug) Serial.print("c2 "); 
   pos1d = (val / 1000) % 10; 
   pos2d = (val / 100) % 10; 
   dec_pos = 1; 
  } 
  else if (val < 100000) {

  if(debug) Serial.print("c3 "); 
   pos1d = (val / 10000) % 10; 
   pos2d = (val / 1000) % 10;
   dec_pos = 2;  
  }




  if(dec_pos == 0) {

    //modul.setDisplayDigit(0, pos1-1, 1); 
    modul.clearDisplayDigit(pos1-1,1); 
    modul.setDisplayDigit(pos1d, pos1, 0);
    modul.setDisplayDigit(pos2d, pos2, 0);
  } 
  else if(dec_pos == 1) {
    //modul.setDisplayDigit(0, pos1-1, 0); 
    modul.clearDisplayDigit(pos1-1,0); 
    modul.setDisplayDigit(pos1d, pos1, 1);
    modul.setDisplayDigit(pos2d, pos2, 0);
  }
  else if(dec_pos == 2) {
    //modul.setDisplayDigit(0, pos1-1, 0); 
    modul.clearDisplayDigit(pos1-1,0); 
    modul.setDisplayDigit(pos1d, pos1, 0);
    modul.setDisplayDigit(pos2d, pos2, 1);
  }



  if(debug) { 
    Serial.print(val); 
    Serial.print(" "); 
    Serial.print(dec_pos); 
    Serial.print(" "); 
    Serial.print(pos1d);
    Serial.print(" "); 
    Serial.print(pos2d); 
    Serial.print(" "); 
  }
  
}

void setcount(int count) {
    if(count == 100) {
      modul.setDisplayDigit(0,0,1); 
    } else {
      byte pos1 = (count / 10) % 10; 
      byte pos2 = count % 10; 

      modul.setDisplayDigit(pos1, 0, 0);
      modul.setDisplayDigit(pos2, 1, 0); 

}

void setDisp() {
  // Count 
  if(pulses_to_go > 0) {
    setcount(pulses_to_go);
  } else {
      setcount(count);
    }
  }

  //Off



  setDispHelper(&off_duration_ms, 3,4); 



  /*
  if(debug) {
    Serial.print(dec_pos); 
    Serial.print(" "); 
  }
  */

  setDispHelper(&on_duration_ms, 6,7); 

  if(debug) Serial.println(dec_pos); 

  
}

int press_dur[8]; 
unsigned long iterations = 0; 
int threshhold = 40; 
int fast = 10; 


void updatePressDur(byte b) {  //Determines how long a button has been pushed down for 
  for(int i = 0; i < 8; i ++) {
    if(getBit(b, i) == 0) {
      press_dur[i] = 0;
    } else {
      press_dur[i] ++; 
    }
  }
}



void send_pulses() {
  if(off_duration_ms < 100 & on_duration_ms < 100) {
    send_pulses_short(); 
  } else {
    pulses_to_go = count; 
  }
}

void send_pulses_short() {
      
    modul.setLEDs(255); 
    for(int i = 0; i < count; i ++) { 
      delay(off_duration_ms); 
      digitalWrite(OUTPUT_PIN, HIGH); 
      digitalWrite(LED_PIN, HIGH); 
      delay(on_duration_ms); 
      digitalWrite(LED_PIN, LOW);
      digitalWrite(OUTPUT_PIN, LOW); 

    }
    modul.setLEDs(0); 

}

bool is_on = false; 
int pulses_to_go = 0; 
unsigned long target_time = 0; 
int last_loop_time = 0; 

void pulse_handler() {
  if(pulses_to_go == 0) return; 
  
  last_loop_time = last_loop_time - millis(); 

  unsigned long time_to_go = target_time - millis(); 


  if(time_to_go < (last_loop_time + 5)) {
    while(target_time < millis()) {
      delayMicroseconds(10); 
    }
    is_on != is_on; 
    digitalWrite(OUTPUT_PIN, is_on); 
    digitalWrite(LED_PIN, is_on); 

    if(is_on) {
      target_time = millis() + on_duration_ms; 
    } else {
      target_time = millis() + off_duration_ms; 
      pulses_to_go --; 
    }

  }

  last_loop_time = millis(); 

}



void setup(){
  //bleibt leer

  pinMode(LED_PIN, OUTPUT);
  pinMode(OUTPUT_PIN, OUTPUT); 
  pinMode(EXT_Trigger, INPUT); 


  if(debug) Serial.begin(9600); 
}
void loop(){

  iterations ++; 

  loop_dur_ms = millis(); 

  bool trigger_pressed = digitalRead(EXT_Trigger);



  byte button = modul.getButtons(); 

  updatePressDur(button); 

  if(getBit(button, 0)) { // - Count 

    if(press_dur[0] == 1 || ( (press_dur[0] > threshhold) && ((iterations % fast) == 0) ) )  {
      count --; 
      if(count == 0) count = 100; 
    }
  

  }
  if(getBit(button,1)) { //+ Count

    if(press_dur[1] == 1 || ( (press_dur[1] > threshhold) && ((iterations % fast) == 0) ) )  {
      count ++; 
      if(count > 100) count = 1;
    }

  }
  if(getBit(button,2)) { //EEPROM
    //nope
  }
  if(getBit(button,3)) {

    if(press_dur[3] == 1 || ( (press_dur[3] > threshhold) && ((iterations % fast) == 0) ) )  {
      smart_decrease(&off_duration_ms); 
    }

  }
  if(getBit(button,4)) {

    if(press_dur[4] == 1 || ( (press_dur[4] > threshhold) && ((iterations % fast) == 0) ) )  {
      smart_increase(&off_duration_ms);
    }
  }
  if(getBit(button,5) || trigger_pressed) {
      send_pulses();
  } 
  if(getBit(button, 6)) {

    if(press_dur[6] == 1 || ( (press_dur[6] > threshhold) && ((iterations % fast) == 0) ) )  {
      smart_decrease(&on_duration_ms); 
    }

  }
  if(getBit(button,7)){


  if(press_dur[7] == 1 || ( (press_dur[7] > threshhold) && ((iterations % fast) == 0) ) )  {
    smart_increase(&on_duration_ms); 
  }

  }

  setDisp(); 

  pulse_handler();

  delay(1); 



}