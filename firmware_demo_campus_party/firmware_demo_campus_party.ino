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
void avanzar(double tiempo, double rapidez);
void abrirPinza();
void cerrarPinza();
void girar(double tiempo, double sentido);
MeMegaPiDCMotor motor_4(4);
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
    Encoder_1.setPulse(8);
    Encoder_1.setRatio(46.67);
    Encoder_1.setPosPid(1.8,0,1.2);
    Encoder_1.setSpeedPid(0.18,0,0);
    attachInterrupt(Encoder_2.getIntNum(), isr_process_encoder2, RISING);
    Encoder_2.setPulse(8);
    Encoder_2.setRatio(46.67);
    Encoder_2.setPosPid(1.8,0,1.2);
    Encoder_2.setSpeedPid(0.18,0,0);
    gyro_1.begin();
    turningSpeed = 255;      // turningSpeed = 100;
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
    Encoder_1.setTarPWM(speed);
    Encoder_2.setTarPWM((0) - (speed));
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

void avanzar(double tiempo, double rapidez)
{
    Encoder_1.runSpeed(rapidez);
    Encoder_2.runSpeed((-1) * (rapidez));
    _delay(tiempo);
    Encoder_1.runSpeed(0);
    Encoder_2.runSpeed(0);
}

void abrirPinza()
{
    motor_4.run(-255);
    _delay(2);
    motor_4.run(0);
    _delay(0.1);
}

void cerrarPinza()
{
    motor_4.run(255);
    _delay(2);
    motor_4.run(0);
    _delay(0.1);
}

void girar(double tiempo, double sentido)
{
    Encoder_1.runSpeed(sentido);
    Encoder_2.runSpeed(sentido);
    _delay(tiempo);
    Encoder_1.runSpeed(0);
    Encoder_2.runSpeed(0);
}

int selector() {
  int lectura = 400;
  int lecturaAnterior;
  do {
    lecturaAnterior = lectura;
    _delay(1);
    lectura = ultrasonic_7.distanceCm();
  } while ((fabs(lectura-lecturaAnterior) > 5) || (lectura >= 400));
  Serial.print("lectura = ");
  Serial.print(lectura);
  Serial.print("\tlecturaAnterior = ");
  Serial.println(lecturaAnterior);
  return lectura;
}

int leerOpcion() {
/* la lectura de la elección se realiza mediante el sensor de distancia;
apoyando la mano a cierta distancia (en intervalos de 10cm) se elige una opción */
    int eligio = false;
    int lectura;
    while (!eligio) {
      lectura = selector();
      if (lectura < 40) {
        eligio = true;
        if (lectura < 30) {
          if (lectura < 20) {
            if (lectura < 10) {
              return 1;
            }
            return 2;
          }
          return 3;
        }
        return 4;
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
}

void recorridoPoligono(){
  int lectura = 400;
  int lados;

  Serial.println("elija la cantidad de lados del polígono");
  _delay(1);
  while (lectura > 55) {
    lectura = selector();
  }
  /* cantidad de lados del polígono = (distancia al sensor ultrasónico a la que
  se pone la mano) / 10cm + 2 */
  lados = lectura / 10 + 2;
  Serial.print("polígono de ");
  Serial.print(lados);
  Serial.println(" lados");
  for(int __i__=0;__i__<lados;++__i__) {
    setSpeed(normalSpeed);
    _delay(1);
    setSpeed(0);
    _delay(1);
    rotateLeft(((360) / (lados)) * (0.8333));
    _delay(1);
  }
  _loop();
}

void agarraGiraYSuelta() {
  int obj = 0;
  abrirPinza();
  for(int __i__=0;__i__<2;++__i__) {
    avanzar(2,normalSpeed);
    if(((fmod(obj,2))==(1))){
      abrirPinza();
    } else {
      cerrarPinza();
    }
    obj += 1;
    avanzar(2,-normalSpeed);
    girar(0.69,turningSpeed);
  }
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
    case 4:
      Serial.println("entro a programa 4: agarra, gira y suelta");
      agarraGiraYSuelta();
      break;
    default:
      break;
  }
  Serial.println("fin del switch case");
  _delay(1);
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
