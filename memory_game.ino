#define PLAYER_WAIT_TIME 3000
#include <LiquidCrystal.h>
#include <TimerOne.h>

/* Display */
LiquidCrystal lcd( 7, 6, 5, 4, 3, 2);

byte sequence[100];
byte currentLength = 0;
byte inputCount = 0;
byte lastInput = 0;
byte expRead = 0;
bool btnPressed = false;
bool wait = false;
bool reset = false;
byte nrPins = 3;
byte pins[] = {10, 9 ,8};

long inputTime = 0;

void setup() {
  // put your setup code here, to run once:
  setPinDirection(OUTPUT);
  turnAllLeds(1000);
  delay(1000);
  Serial.begin(9600);
}

//Set pins as Input or Output
void setPinDirection (byte dir){
  for( byte i = 0; i < nrPins; i++){
    pinMode(pins[i], dir);  
  }
}


void blink(byte val){
  for (byte i = 0; i < nrPins; i++){
    digitalWrite(pins[i], val);
  }
}


void playSequence(){
  for(int i = 0; i< currentLength; i++){
    Serial.print("Seq->");
    Serial.print(i + ":");
    Serial.print("Pin: ");
    Serial.println(sequence[i]);
    digitalWrite(sequence[i],HIGH);
    delay(500);
    digitalWrite(sequence[i],LOW);
    delay(500);
    }

 }

void displayScore(){
  //lcd.clear();
  lcd.begin(16,2);
  lcd.setCursor(0,0);
  lcd.print("Score: ");
  lcd.println(currentLength - 1);
  Serial.print("Score:");
  Serial.print(currentLength - 1);
}

void turnAllLeds(int freq){
  for(int i = 0; i < nrPins; i++){
    blink(HIGH);
    delay(freq);
    blink(LOW);
    delay(freq);  
  }
}


void lose(){
    setPinDirection(OUTPUT);
    turnAllLeds(200);
    turnAllLeds(200);
    turnAllLeds(200);
    Serial.println("LOSE GAME !!");
}


void resetFunction(){
  turnAllLeds(1000);
  currentLength = 0;
  inputCount = 0;
  lastInput = 0;
  expRead = 0;
  btnPressed = false;
  wait = false;
  reset = false;
}


void doLoseProcess(){
  lose();
  Serial.println("Last sequence");
  lcd.begin(16,2);
  lcd.setCursor(0,0);
  lcd.print("Game over!");
  lcd.setCursor(0,2);
  lcd.print("Last score: ");
  lcd.print(currentLength - 1);
  setPinDirection(OUTPUT);
  playSequence();
  delay(2000);
  resetFunction();  
}

void loop() {
  // put your main code here, to run repeatedly:
  if(!wait){
   
    setPinDirection(OUTPUT);
    randomSeed(analogRead(A0));
    sequence[currentLength] = pins[random(0, nrPins)];
    currentLength++;
    playSequence();
    wait = true;
    inputTime = millis();
    displayScore();
  }else{
    setPinDirection(INPUT); 
    if (millis() - inputTime > PLAYER_WAIT_TIME){
      doLoseProcess();
      return;
    }

    if(!btnPressed){
      expRead = sequence[inputCount];
      Serial.print("Expected: ");                 
      Serial.println(expRead);                     
      
      for(int i = 0; i < nrPins; i++){           
        if(pins[i] == expRead)                        
          continue;                               
        if(digitalRead(pins[i]) == HIGH){         
          lastInput = pins[i];
          reset = true;                       
          btnPressed = true;                          
          Serial.print("Read: ");                 
          Serial.println(lastInput);
                                        }
      }
    }
  if(digitalRead(expRead) == 1 && !btnPressed){
    inputTime = millis();
    lastInput = expRead;
    inputCount++;
    btnPressed = true;
  }else{
     if(btnPressed && digitalRead(lastInput) == LOW){
      btnPressed = false;
      delay(100);
      if(reset){
        doLoseProcess();
      }
     }else{
      if (inputCount == currentLength){
        wait = false;
        inputCount = 0;
        delay(1000);}
        }
  }
}
}
