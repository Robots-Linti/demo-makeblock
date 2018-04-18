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
void setRotation(double speed);
void setSpeed(double speed);
void rotateLeft(double degrees);
MeGyro gyro_1;
MeUltrasonicSensor ultrasonic_7(7);
double turningSpeed;
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
    gyro_1.begin();
    turningSpeed = 50;
    normalSpeed = 255;
    umbral = 25;
/* FIN INICIALIZACIÓN VARIABLES NUESTRAS */
}

/* FUNCIONES NUESTRAS */
void setRotation(double speed)
{
    Encoder_1.setTarPWM(speed);
    Encoder_2.setTarPWM(speed);
}

void setSpeed(double speed)
{
    Encoder_1.setTarPWM((0) - (speed));
    Encoder_2.setTarPWM(speed);
}

void rotateLeft(double degrees)
{
    double endValue = (fmod(((gyro_1.getAngle(3)) + (180)) + (degrees),360)) - (180);
    setRotation(turningSpeed);
    if((gyro_1.getAngle(3)) > (endValue)){
        while(!(( 0 ) > (gyro_1.getAngle(3))))
        {
            _loop();
        }
    }
    while(!((gyro_1.getAngle(3)) > (endValue)))
    {
        _loop();
    }
    setSpeed(0);
}

int leerOpcion() {
/* la lectura de la elección se realiza mediante el sensor de distancia;
apoyando la mano a cierta distancia (en intervalos de 10cm) se elige una opción */
    int eligio = false;
    int lectura;
    while (!eligio) {
      lectura = ultrasonic_7.distanceCm();
      if (lectura < 30) {
        eligio = true;
        if (lectura < 20) {
          if (lectura < 10) {
            return 1;
          }
          return 2;
        }
        return 3;
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

void recorridoPoligono(){
/* cantidad de lados del polígono = distancia al sensor ultrasónico a la que
se pone la mano dividido 10cm */
    int lectura = 400;
    int lados;
    while (lectura > 55) {
      lectura = ultrasonic_7.distanceCm();
    }
    lados = lectura / 10;
    setSpeed(normalSpeed);
    _delay(1);
    setSpeed(0);
    _delay(1);
    rotateLeft(((360) / (lados)) * (0.8333));
    _delay(1);
    _loop();
}
/* FIN FUNCIONES NUESTRAS */

void loop(){
/* PROGRAMA NUESTRO */
  Serial.println("inicio loop ppal");
  switch (leerOpcion()) {
    case 1:
      Serial.println("entro a programa 1: robot que se para cuando tiene algo adelante");
      robotQueSeParaCuandoTieneAlgoAdelante();
      break;
    case 2:
      Serial.println("entro a programa 2: giro un poquito las ruedas");
      programa2();
      break;
    case 3:
      Serial.println("entro a programa 3: recorrido poligono");
      recorridoPoligono();
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
    gyro_1.update();
}
