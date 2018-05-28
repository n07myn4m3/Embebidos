boolean       up = true;
float         ref  = 0;
//-----------------------------------
//         Temporizador
//-----------------------------------
unsigned long currentMicros = 0;

void setup() {
Serial.begin(115200);
}

void loop() {
//--------------------------------------
     if (up == true && ref <= 100){
         ref += 1;
           if (ref == 1023){
           up = false; 
           }
     }else{
         up = false;
         ref -= 1;    
           if (ref == 0){
            up = true;
           }
     } 

//  Serial.print(currentMicros);
//  Serial.print(" , ");
//  Serial.print(ref);
//  Serial.print(" , ");
//  Serial.println(-ref);
  
  Serial.print(0);
  Serial.print(" , ");
  Serial.print(1);
  Serial.print(" , ");
  Serial.println(0);
  // wait 
  delay(1000);  
//--------------------------------------
currentMicros = micros();
}
