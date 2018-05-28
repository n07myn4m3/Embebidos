// La velocidad depende del modelo de ESP-01
// siendo habituales 9600 y 115200
const int baudRate = 115200;
 
 
void setup()
{
   Serial.begin(115200);
   Serial1.begin(115200);
}
 
void loop()
// enviar los datos de la consola serial al ESP-01, 
// y mostrar lo enviado por el ESP-01 a nuestra consola
{
   if (Serial1.available())
   {
      Serial.print((char)Serial1.read());
   }
   if (Serial.available())
   {
      Serial1.print((char)Serial.read());
   }
}
