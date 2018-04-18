#include <Arduino.h>
#include <Wire.h>
#include <SoftwareSerial.h>

#include <MeMegaPi.h>

//Encoder Motor
MeEncoderOnBoard Encoder_1(SLOT1);
MeEncoderOnBoard Encoder_2(SLOT2);
MeEncoderOnBoard Encoder_3(SLOT3);
MeEncoderOnBoard Encoder_4(SLOT4);

void isr_process_encoder1(void)
{
      if(digitalRead(Encoder_1.getPortB()) == 0){
            Encoder_1.pulsePosMinus();
      }else{
            Encoder_1.pulsePosPlus();
      }
}

void isr_process_encoder2(void)
{
      if(digitalRead(Encoder_2.getPortB()) == 0){
            Encoder_2.pulsePosMinus();
      }else{
            Encoder_2.pulsePosPlus();
      }
}

void isr_process_encoder3(void)
{
      if(digitalRead(Encoder_3.getPortB()) == 0){
            Encoder_3.pulsePosMinus();
      }else{
            Encoder_3.pulsePosPlus();
      }
}

void isr_process_encoder4(void)
{
      if(digitalRead(Encoder_4.getPortB()) == 0){
            Encoder_4.pulsePosMinus();
      }else{
            Encoder_4.pulsePosPlus();
      }
}

void move(int direction, int speed)
{
      int leftSpeed = 0;
      int rightSpeed = 0;
      if(direction == 1){
            leftSpeed = -speed;
            rightSpeed = speed;
      }else if(direction == 2){
            leftSpeed = speed;
            rightSpeed = -speed;
      }else if(direction == 3){
            leftSpeed = speed;
            rightSpeed = speed;
      }else if(direction == 4){
            leftSpeed = -speed;
            rightSpeed = -speed;
      }
      Encoder_1.setTarPWM(rightSpeed);
      Encoder_2.setTarPWM(leftSpeed);
}
void moveDegrees(int direction,long degrees, int speed_temp)
{
      speed_temp = abs(speed_temp);
      if(direction == 1)
      {
            Encoder_1.move(degrees,(float)speed_temp);
            Encoder_2.move(-degrees,(float)speed_temp);
      }
      else if(direction == 2)
      {
            Encoder_1.move(-degrees,(float)speed_temp);
            Encoder_2.move(degrees,(float)speed_temp);
      }
      else if(direction == 3)
      {
            Encoder_1.move(degrees,(float)speed_temp);
            Encoder_2.move(degrees,(float)speed_temp);
      }
      else if(direction == 4)
      {
            Encoder_1.move(-degrees,(float)speed_temp);
            Encoder_2.move(-degrees,(float)speed_temp);
      }
}

double angle_rad = PI/180.0;
double angle_deg = 180.0/PI;

/* VARIABLES NUESTRAS */
void setSpeed(double number1);
MeUltrasonicSensor ultrasonic_7(7);
double normalSpeed;
double umbral;
/* FIN VARIABLES NUESTRAS */

void setup(){
    //Set Pwm 8KHz
    TCCR1A = _BV(WGM10);
    TCCR1B = _BV(CS11) | _BV(WGM12);
    TCCR2A = _BV(WGM21) | _BV(WGM20);
    TCCR2B = _BV(CS21);
/* INICIALIZACIÓN VARIABLES NUESTRAS */
    Serial.begin(9600);
    attachInterrupt(Encoder_1.getIntNum(), isr_process_encoder1, RISING);
    attachInterrupt(Encoder_2.getIntNum(), isr_process_encoder2, RISING);
    normalSpeed = 255;
    umbral = 25;
/* FIN INICIALIZACIÓN VARIABLES NUESTRAS */
}

/* FUNCIONES NUESTRAS */
void setSpeed(double number1)
{
    Encoder_1.setTarPWM((0) - (number1));
    Encoder_2.setTarPWM(number1);
}

int leerOpcion() {

    // DEFINIR metodo de entrada (bt, serial, etc.)
    // y tomar la opcion elegida de ahi

    int eligio = false;
    int lectura;
    while (!eligio) {
      lectura = ultrasonic_7.distanceCm();
      if (lectura < 20) {
        eligio = true;
        if (lectura < 10) {
          return 1;
        }
        return 2;
      }
      _loop();
    }
    return 0;
}

void robotQueSeParaCuandoTieneAlgoAdelante() {
  _delay(1);

  setSpeed(normalSpeed);
  while(!((ultrasonic_7.distanceCm()) < (umbral))) {
    _loop();
  }
  setSpeed(0);
  while(!((ultrasonic_7.distanceCm()) > (umbral))) {
    _loop();
  }
}

void programa2() {
  for(int __i__=0;__i__<1;++__i__) {
    setSpeed(normalSpeed);
    _delay(0.5);
    setSpeed(0);
    _delay(1);
  }
  Serial.println("termino ejecucion");
}
/* FIN FUNCIONES NUESTRAS */

void loop(){
/* PROGRAMA NUESTRO */
  Serial.println("inicio loop ppal");
  switch (leerOpcion()) {
    case 1:
      Serial.println("entro a programa 1");
      robotQueSeParaCuandoTieneAlgoAdelante();
      break;
    case 2:
      Serial.println("entro a programa 2");
      programa2();
      break;
    default:
      // do something
      break;
  }
  Serial.println("fin del switch case");
/* FIN PROGRAMA NUESTRO */
    _loop();
}

void _delay(float seconds){
    long endTime = millis() + seconds * 1000;
    while(millis() < endTime)_loop();
}

void _loop(){
    Encoder_1.loop();
    Encoder_2.loop();
}
