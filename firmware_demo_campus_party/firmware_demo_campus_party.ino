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
	if(digitalRead(Encoder_1.getPortB()) == 0) {
		Encoder_1.pulsePosMinus();
	} else {
		Encoder_1.pulsePosPlus();
	}
}

void isr_process_encoder2(void)
{
	if(digitalRead(Encoder_2.getPortB()) == 0) {
		Encoder_2.pulsePosMinus();
	} else {
		Encoder_2.pulsePosPlus();
	}
}

void isr_process_encoder3(void)
{
	if(digitalRead(Encoder_3.getPortB()) == 0) {
		Encoder_3.pulsePosMinus();
	} else {
		Encoder_3.pulsePosPlus();
	}
}

void isr_process_encoder4(void)
{
	if(digitalRead(Encoder_4.getPortB()) == 0) {
		Encoder_4.pulsePosMinus();
	} else {
		Encoder_4.pulsePosPlus();
	}
}

void move(int direction, int speed)
{
	int leftSpeed = 0;
	int rightSpeed = 0;
	if(direction == 1) {
		leftSpeed = -speed;
		rightSpeed = speed;
	} else if(direction == 2) {
		leftSpeed = speed;
		rightSpeed = -speed;
	} else if(direction == 3) {
		leftSpeed = speed;
		rightSpeed = speed;
	} else if(direction == 4) {
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

/* PROTOTIPOS DE FUNCIONES NUESTRAS */
void setRotationSpeed(double speed);
void setSpeed(double speed);
void rotateLeft(double degrees);
void avanzar(double tiempo, double rapidez);
void abrirPinza();
void cerrarPinza();
void subirBrazo();
void bajarBrazo();
void girar(double tiempo, double sentido);
/* FIN PROTOTIPOS DE FUNCIONES NUESTRAS */

/* VARIABLES NUESTRAS */
MeMegaPiDCMotor motor_12(12);
MeGyro gyro_1;
MeUltrasonicSensor ultrasonic_7(7);
double turningSpeed;
double normalSpeed;
double umbral;
/* FIN VARIABLES NUESTRAS */

/* FUNCIONES NUESTRAS */
void setRotationSpeed(double speed)
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
	setRotationSpeed(turningSpeed);
	if((gyro_1.getAngle(3)) > (endValue)) {
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

void avanzar(int tiempo, double rapidez)
{
	Encoder_1.runSpeed(rapidez);
	Encoder_2.runSpeed(-1 * rapidez);
	_delay(tiempo);
	Encoder_1.runSpeed(0);
	Encoder_2.runSpeed(0);
}

void abrirPinza()
{
	motor_12.run(-255);
	_delay(2);
	motor_12.run(0);
	_delay(0.1);
}

void cerrarPinza()
{
	motor_12.run(255);
	_delay(2);
	motor_12.run(0);
	_delay(0.1);
}

void subirBrazo()
{
	Encoder_3.setTarPWM(-100);
	_delay(7);
	Encoder_3.setTarPWM(0);
}

void bajarBrazo()
{
	Encoder_3.setTarPWM(100);
	_delay(3);
	Encoder_3.setTarPWM(0);
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
		Serial.print("lectura = ");
		Serial.print(lectura);
		Serial.print("\tlecturaAnterior = ");
		Serial.println(lecturaAnterior);
	} while (abs(lectura-lecturaAnterior) > 5 || lectura >= 400);
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
	subirBrazo();
	bajarBrazo();

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

/*
 * Avanza medio segundo y para.
 */
void programa2() {
	setSpeed(normalSpeed);
	_delay(0.5);
	setSpeed(0);
	_delay(1);
}

/* REACOMODAR
	Serial.println("elija la cantidad de lados del polígono");
	_delay(1);
	while (lectura > 55) {
		lectura = selector();
	}
	lados = lectura / 10 + 2;
*/
void recorridoPoligono(int lados) {
	int lectura = 400;

	Serial.print("polígono de ");
	Serial.print(lados);
	Serial.println(" lados");
	for (int i=0; i<lados; ++i) {
		setSpeed(normalSpeed);
		_delay(1);
		setSpeed(0);
		_delay(1);
		rotateLeft(360 / lados * 0.8333);
		_delay(1);
	}
	_loop();
}

void agarraGiraYSuelta() {
	/* Llevar brazo a posición inicial y abrir pinza */
	{
		motor_12.run(-255);
		Encoder_3.setTarPWM(-100);
		_delay(2);
		motor_12.run(0);
		_delay(5);
		Encoder_3.setTarPWM(0);
	}

	for (char i = 0; i < 2; i++) {
		avanzar(2, normalSpeed);
		bajarBrazo();
		if (i % 2)
			abrirPinza();
		else
			cerrarPinza();
		subirBrazo();
		avanzar(2,-normalSpeed);
		girar(0.69, turningSpeed);
	}
}

/* 
 * Detiene todos los motores y realiza acciones útiles para el tiempo
 * entre distintos programas.
 */
void reset()
{
	Encoder_1.setTarPWM(0);
	Encoder_2.setTarPWM(0);
	Encoder_3.setTarPWM(0);
	Encoder_4.setTarPWM(0);
	_delay(1);
}
/* FIN FUNCIONES NUESTRAS */

void loop() {

	/* PROGRAMA NUESTRO */
	for (char i = 1; i <= 4; i++) {
		switch (i) {
		case 1:
			Serial.println("Robot que se para cuando tiene algo adelante");
			robotQueSeParaCuandoTieneAlgoAdelante();
			girar(0.69, 255);
			break;
		case 2:
			Serial.println("Agarra, gira y suelta");
			agarraGiraYSuelta();
			break;
		case 3:
			Serial.println("Sube y baja por rampa hasta cima");
			break;
		case 4:
			Serial.println("Avanza hasta obstáculo y se esconde detrás de el");
			break;
		case 5:
			Serial.println("Gira alrededor de objeto");
			break;
		default:
			break;
		}
		Serial.println("RESET");
		reset();
	}
fin:
	Serial.println("FIN DEMO");
	for(;;)
		_loop();
	/* FIN PROGRAMA NUESTRO */
}

void _delay(float seconds) {
	long endTime = millis() + seconds * 1000;
	while(millis() < endTime)_loop();
}

void _loop() {
	Encoder_1.loop();
	Encoder_2.loop();
	Encoder_3.loop();
	gyro_1.update();
}

void setup() {
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
	attachInterrupt(Encoder_3.getIntNum(), isr_process_encoder3, RISING);
	Encoder_3.setPulse(8);
	Encoder_3.setRatio(46.67);
	Encoder_3.setPosPid(1.8,0,1.2);
	Encoder_3.setSpeedPid(0.18,0,0);
	gyro_1.begin();
	turningSpeed = 255;      // turningSpeed = 100;
	normalSpeed = 255;
	umbral = 25;
	/* FIN INICIALIZACIÓN VARIABLES NUESTRAS */
}
