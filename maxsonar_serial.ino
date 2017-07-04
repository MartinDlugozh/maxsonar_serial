/**
 * ПО преобразователя интерфейсов для ультразвуковых дальномеров MB1043 HRLV-MaxSonar-EZ4 и HC-SR04
 * Необходимо в том случае, если АП неадекватно работает или не работает вообще с датчиком напрямую;
 *
 * С помощью директив препроцессора можно выбрать необходимый вариант компиляции прошивки
 * (путем раскомментирования строки с необходимым значением параметра и подавлением ненужного):
 *
 * SENSOR_TYPE может принимать значения SENSOR_MAXSONAR или SENSOR_HC_SR04 в соответстви с типом подключаемого сенсора.
 *
 * SENSOR_INPUT_TYPE можно установить в один из доступных типов:
 * 	Для сенсоров Maxbotix:
 * 		IN_MX_SERIAL - читать данные с цифрового последовательного интерфейса датчика;
 * 		IN_MX_ANALOG - читать с аналогового выхода;
 * 		IN_MX_PWM - читать с выхода ШИМ;
 * 	Для HC-SR04:
 * 		HC_SR04_TE - использовать порт HC_SR04_TE (+5V, TRIGGER, ECHO, GND).
 *
 * Так же можно задавать следующие параметры:
 * _DEBUG - активирует передачу результатов замеров на последовательный порт Serial (USB);
 * I2C_SENSOR_ADDRESS - адрес МК-преобразователя для подключения к АП (может меняться в зависимости от параметров АП.
 */

/**
 * Sensor type settings
 */
#define SENSOR_MAXSONAR			0
#define SENSOR_HC_SR04			1

#define SENSOR_TYPE 			SENSOR_MAXSONAR
//#define SENSOR_TYPE 			SENSOR_HC_SR04

/**
 * Sensor output type settings
 */
#define IN_MX_SERIAL			1
#define IN_MX_ANALOG			2
#define IN_MX_PWM				3
#define HC_SR04_TE				4

#define SENSOR_INPUT_TYPE		IN_MX_SERIAL
//#define SENSOR_INPUT_TYPE		IN_MX_ANALOG
//#define SENSOR_INPUT_TYPE		IN_MX_PWM
//#define SENSOR_INPUT_TYPE		HC_SR04_TE

/**
 * I2C Slave address
 * Note: адрес 0x62 совместим с настройкой ArduPilot PulsedLightI2C (RNGFND_TYPE = 3)
 */
#define I2C_SENSOR_ADDRESS  	0x62
//#define I2C_SENSOR_ADDRESS  	0x30
//#define I2C_SENSOR_ADDRESS  	0x70

/**
 * MC pins definitions
 */
#define REGISTER_MAP_SIZE 		2
#define MX_RX_SERIAL_PIN 		6
#define MX_PWM_PIN				8
#define MX_ANALOG_PIN			A0
#define HC_SR04_ECHO_PIN 		11
#define HC_SR04_TRIG_PIN 		12

#define _DEBUG 					0

/**
 * Includes
 */
#include "Arduino.h"
#if(_DEBUG == 1)
#include "HardwareSerial.h"
#endif
#if(SENSOR_TYPE == SENSOR_MAXSONAR)
#include "Maxbotix.h"
#elif(SENSOR_TYPE == SENSOR_HC_SR04)
#include "NewPing.h"
#endif
#include <Wire.h>

/**
 * Global variables
 */
byte 				register_map[REGISTER_MAP_SIZE];
volatile int32_t 	dist_cm 							= 0;
volatile float 		dist_cm_f 							= 0;

/**
 * Sensor object
 * Note: если используется датчик из линейки Maxbotix, модель которого не HRLV, ниже следует внести правку,
 * заменив Maxbotix::HRLV на Maxbotix::LV или Maxbotix::XL
 */
#if(SENSOR_INPUT_TYPE == IN_MX_SERIAL)
Maxbotix rangeSensorTX(MX_RX_SERIAL_PIN, Maxbotix::TX, Maxbotix::HRLV, Maxbotix::BEST, 8);
#elif(SENSOR_INPUT_TYPE == IN_MX_ANALOG)
Maxbotix rangeSensorAD(MX_ANALOG_PIN, Maxbotix::AN, Maxbotix::HRLV, Maxbotix::BEST, 8);
#elif(SENSOR_INPUT_TYPE == IN_MX_PWM)
Maxbotix rangeSensorPW(MX_PWM_PIN, Maxbotix::PW, Maxbotix::HRLV, Maxbotix::BEST, 8);
#elif(SENSOR_INPUT_TYPE == HC_SR04_TE)
NewPing hc_sr04(HC_SR04_TRIG_PIN, HC_SR04_ECHO_PIN, 200);
#endif

/**
 * I2C onRequest handler
 */
void requestData()
{
    register_map[0] = dist_cm >> 8; // MSB
    register_map[1] = dist_cm & 0xFF; //LSB

    Wire.write(register_map, REGISTER_MAP_SIZE);
}

void setup()
{
    Wire.begin(I2C_SENSOR_ADDRESS);
    Wire.onRequest(requestData);

#if(_DEBUG == 1)
	Serial.begin(38400);
#endif
}

void loop()
{
	// читаем расстояние, по-умолчанию получаем его в сантиметрах
	// duration_f нужен по большому счету только для отладки
#if(SENSOR_INPUT_TYPE == IN_MX_SERIAL)
	dist_cm_f = rangeSensorTX.getRange(); // / 100.0f
#elif(SENSOR_INPUT_TYPE == IN_MX_ANALOG)
	dist_cm_f = rangeSensorAD.getRange();
#elif(SENSOR_INPUT_TYPE == IN_MX_PWM)
	dist_cm_f = rangeSensorPW.getRange();
#elif(SENSOR_INPUT_TYPE == HC_SR04_TE)
	dist_cm_f = hc_sr04.ping() / US_ROUNDTRIP_CM;
#endif

	// переводим в целочисельный тип для отправки по I2C
	dist_cm = (int16_t)dist_cm_f;

#if(_DEBUG == 1)
	Serial.println(dist_cm_f);
#endif
}
