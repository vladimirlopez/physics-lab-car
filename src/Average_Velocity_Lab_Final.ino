/* Instructions to use:
 * 1. Press reset, green LED at the top left will blink very bright 3 times
 *  2. Green LED will be on dimmer for 2 seconds waiting for the selection
 *  3. Press the selection on the remote and the car will start moving. 
 *  
 *  Description of Scenarios:
 *  Trips 1,3: slow forward, fast forward 
 *  Trip 2: fast back, slow back 
 *  Trip 4: slow back, fast back 
 *  Trip 5: slow forward, fast back 
 *  Trip 6: fast forward, slow back
 *  Trip 7: fast back, slow forward
 *  Trip 8: fast forward, slow back
 */


#include <IRremote.h>

#define UP 16736925
#define RIGHT 16761405
#define DOWN 16754775
#define LEFT 16720605
#define OK 1839493672
#define N1 16738455    //number 1
#define N2 16750695
#define N3 16756815
#define N4 16724175
#define N5 16718055
#define N6 16743045
#define N7 16716015
#define N8 16726215
#define N9 16734885
#define N0 16730805
#define S* 16728765   //symbol *
#define S# 16732845

#define RECV_PIN 12

//Line Tracking IO define
#define LT_R !digitalRead(10)
#define LT_M !digitalRead(4)
#define LT_L !digitalRead(2)
//motor definitions
#define ENB 5 // Left wheel speed
#define IN1 7 // Left wheel forward
#define IN2 8 // Left wheel reverse
#define IN3 9 // Right wheel reverse
#define IN4 11 // Right wheel forward
#define ENA 6 // Right wheel speed
//speed definitions
#define slow 120 // initial speed of car >=0 to <=255
#define fast 250


IRrecv irrecv(RECV_PIN);
decode_results results;
unsigned long val;

void forward(int selectedSpeed) {
    analogWrite(ENA, selectedSpeed);
    analogWrite(ENB, selectedSpeed);
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
    Serial.println("go forward!");
   }
  
void back(int selectedSpeed){
    analogWrite(ENA, selectedSpeed);
    analogWrite(ENB, selectedSpeed);
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    Serial.println("Back");
    }
  
void stop(){
  digitalWrite(ENA, LOW);
  digitalWrite(ENB, LOW);
  Serial.println("STOP!");
}

//Trip 1: slow forward, fast forward
void Trip1(){
  bool dark = true;
  while(!LT_M){// move with vi until the white line
  forward(slow);
  Serial.print("Middle ");
  Serial.println(LT_M);
  dark = LT_M;
  }
  if(dark){
  forward(fast);
  Serial.print("Middle ");
  Serial.println(LT_M);
  delay(4000);
  stop();
  }
}
  //Trip 2: fast back, slow back
void Trip2(){
  bool dark = true;
  while(!LT_M){// move with vi until the white line
  back(fast);
  Serial.print("Middle ");
  Serial.println(LT_M);
  dark = LT_M;
  }
  if(dark){
  back(slow);
  Serial.print("Middle ");
  Serial.println(LT_M);
  delay(4000);
  stop();
  }
}
//Trip 3: slow forward, fast forward
void Trip3(){
  bool dark = true;
  while(!LT_M){// move with vi until the white line
  forward(slow);
  Serial.print("Middle ");
  Serial.println(LT_M);
  dark = LT_M;
  }
  if(dark){
  forward(fast);
  Serial.print("Middle ");
  Serial.println(LT_M);
  delay(4000);
  stop();
  }
}
//Trip4: slow back, fast back
void Trip4(){
  bool dark = true;
  while(!LT_M){// move with vi until the white line
  back(slow);
  Serial.print("Middle ");
  Serial.println(LT_M);
  dark = LT_M;
  }
  if(dark){
  back(fast);
  Serial.print("Middle ");
  Serial.println(LT_M);
  delay(4000);
  stop();
  }
}
//Trip 5: slow forward, fast back
void Trip5(){
  bool dark = true;
  while(!LT_M){// move with vi until the white line
  forward(slow);
  Serial.print("Middle ");
  Serial.println(LT_M);
  dark = LT_M;
  }
  if(dark){
  back(fast);
  Serial.print("Middle ");
  Serial.println(LT_M);
  delay(4000);
  stop();
  }
}
//Trip 6: Fast forward, slow back
 void Trip6(){
  bool dark = true;
  while(!LT_M){// move with vi until the white line
  forward(fast);
  Serial.print("Middle ");
  Serial.println(LT_M);
  dark = LT_M;
  }
  if(dark){
  back(slow);
  Serial.print("Middle ");
  Serial.println(LT_M);
  delay(4000);
  stop();
  }
 }
//Trip 7: Fast back, slow forward
 void Trip7(){
  bool dark = true;
  while(!LT_M){// move with vi until the white line
  back(fast);
  Serial.print("Middle ");
  Serial.println(LT_M);
  dark = LT_M;
  }
  if(dark){
  forward(slow);
  Serial.print("Middle ");
  Serial.println(LT_M);
  delay(4000);
  stop();
  }
 }
//Trip 8: Fast forward, slow back
 void Trip8(){
  bool dark = true;
  while(!LT_M){// move with vi until the white line
  forward(fast);
  Serial.print("Middle ");
  Serial.println(LT_M);
  dark = LT_M;
  }
  if(dark){
  back(slow);
  Serial.print("Middle ");
  Serial.println(LT_M);
  delay(4000);
  stop();
  }
} 
  
//delay to tell user when to press selection
void signal(){
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on 
  delay(2000);                       // wait for half a second
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off 
  
}

void setup() {
  Serial.begin(9600);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  
  //stop();
  irrecv.enableIRIn();
  Serial.println("Select speed");
  signal();
    if (irrecv.decode(&results)){
      val = results.value;
      Serial.println(val);
      irrecv.resume();
      switch(val){
      case N1: Trip1(); break;
      case N2: Trip2(); break;
      case N3: Trip3(); break;
      case N4: Trip4(); break;
      case N5: Trip5(); break;
      case N6: Trip6(); break;
      case N7: Trip7(); break;
      case N8: Trip8(); break;
      default: break;
      }
     }
    else{
      stop();
      }
  }
void loop(){
}
