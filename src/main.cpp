#include <Arduino.h>
#include <Servo.h>

Servo svAnsw;  // create servo object to control a servo
Servo svOpen;  // create servo object to control a servo

const int D0 = 10, D1 = 11, D2 = 12, D3 = 13; // Variables correspondientes a las entradas del receptor
const int nmaxOpen = 65;
const int nmaxAnsw = 70;

void pushButton(Servo ser, int nmax){
    for (int pos = 0; pos <= nmax; pos += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    ser.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }
  delay(200);  
  for (int pos2 = nmax; pos2 >= 0; pos2 -= 1) { // goes from 180 degrees to 0 degrees
    ser.write(pos2);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }
}

int arr_screenoff[10];
int nignore = 0;
bool bdebug = true;

bool judge_lights(){
  nignore++;
  if(nignore<300){
    delay(10);
    return false;
  }
  nignore=0;
  int len_off = sizeof(arr_screenoff)/sizeof(int);
  int sum = 0;
  for(int i=0; i<len_off; i++){
    sum += arr_screenoff[i];
  }
  int avg = sum/len_off;
  int currVal = analogRead(A0);
  if(currVal-avg<6){
    for(int i=0; i<len_off-1; i++){
      arr_screenoff[i] = arr_screenoff[i+1];
    }
    arr_screenoff[len_off-1] = currVal;
    Serial.print("add:value:");
    Serial.println(currVal);
  } else if(currVal-avg>10) {
    return true; // only screen off value put to array.
  }
  return false;
}

void setup() {
  svAnsw.attach(5);
  svOpen.attach(6);
  pinMode(D0, INPUT); // Entrada digital correspondiente al boton B
  pinMode(D1, INPUT); // Entrada digital correspondiente al boton D
  pinMode(D2, INPUT); // Entrada digital correspondiente al boton A
  pinMode(D3, INPUT); // Entrada digital correspondiente al boton C  
  Serial.begin(9600); // Velocidad de comunicacion de 9600 bps
  //init screen off value with current value.
  int len_off = sizeof(arr_screenoff)/sizeof(int);
  int startValue = analogRead(A0);
  Serial.print("init sreen off array with current: ");
  Serial.println(startValue);
  for(int i = 0; i <len_off; i++){
    arr_screenoff[i] = startValue;;
  }
}

void loop() {
    svOpen.write(0);
    svAnsw.write(0);
  if (digitalRead(D3) == HIGH){ // Button 1
    Serial.println("Boton: C");
    pushButton(svAnsw, nmaxAnsw); //answer screen
    delay(500);
    pushButton(svOpen, nmaxOpen); // unlock
    delay(500);
    pushButton(svAnsw, nmaxAnsw); // close screen
  }

  if(judge_lights()==true){
    Serial.print("screen:on\n");
  }
  if (Serial.available() > 0) {
    char x = Serial.read();
    if(x=='a'){
      Serial.println("oncmd:a:answer_unlock");
      pushButton(svAnsw, nmaxAnsw); //answer screen
      delay(500);
      pushButton(svOpen, nmaxOpen); // unlock
      delay(500);
      pushButton(svAnsw, nmaxAnsw); // close screen
    } else {
      Serial.print("unkown cmd byte:");
      Serial.println(x);
    }
  }
}