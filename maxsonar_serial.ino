/**
 * ПО преобразователя интерфейсов для ультразвукового дальномера HRLV-MaxSonar-EZ4
 * Необходимо в том случае, если АП неадекватно работает с датчиком напрямую (например, не может оцифровать аналоговый выход);
 *
 * С помощью директив препроцессора можно выбрать необходимый вариант компиляции прошивки,
 * установив SENSOR_INPUT_TYPE в один из доступных типов:
 * IN_SERIAL - читать данные с цифрового последовательного интерфейса датчика;
 * IN_ANALOG - читать с аналогового выхода;
 * IN_PWM - читать с выхода ШИМ;
 * Так же можно задавать следующие параметры:
 * _DEBUG - активирует передачу результатов замеров на последовательный порт Serial (USB);
 * I2C_SENSOR_ADDRESS - адрес МК-преобразователя для подключения к АП (может меняться в зависимости от параметров АП,
 * выбирается путем раскомментирования строки с необходимым адресом и подавлением ненужного;
 */

#define IN_SERIAL			1
#define IN_ANALOG			2
#define IN_PWM				3

#define SENSOR_INPUT_TYPE	IN_SERIAL

/**
 * I2C Slave address
 * Note: адрес 0x62 совместим с настройкой ArduPilot PulsedLightI2C (RNGFND_TYPE = 3)
 */
#define I2C_SENSOR_ADDRESS  0x62
//#define I2C_SENSOR_ADDRESS  0x30
//#define I2C_SENSOR_ADDRESS  0x70

#define REGISTER_MAP_SIZE 	2
#define RX_SERIAL_PIN 		6
#define PWM_PIN				8
#define ANALOG_PIN			A0

#define _DEBUG 				0

#include "Arduino.h"
#if(_DEBUG == 1)
#include "HardwareSerial.h"
#endif
#include "Maxbotix.h"
#include <Wire.h>

byte register_map[REGISTER_MAP_SIZE];
volatile int32_t dist_cm = 0; // duration of the ping
volatile float dist_cm_f = 0;

void requestData() {

    register_map[0] = dist_cm >> 8; // msb
    register_map[1] = dist_cm & 0xFF; //LSB

    Wire.write(register_map, REGISTER_MAP_SIZE);
}

#if(SENSOR_INPUT_TYPE == IN_SERIAL)
Maxbotix rangeSensorTX(RX_SERIAL_PIN, Maxbotix::TX, Maxbotix::HRLV, Maxbotix::BEST, 8);
#elif(SENSOR_INPUT_TYPE == IN_ANALOG)
Maxbotix rangeSensorAD(ANALOG_PIN, Maxbotix::AN, Maxbotix::HRLV, Maxbotix::BEST, 8);
#elif(SENSOR_INPUT_TYPE == IN_PWM)
Maxbotix rangeSensorPW(PWM_PIN, Maxbotix::PW, Maxbotix::HRLV, Maxbotix::BEST, 8);
#endif

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
#if(SENSOR_INPUT_TYPE == IN_SERIAL)
	dist_cm_f = rangeSensorTX.getRange(); // / 100.0f
#elif(SENSOR_INPUT_TYPE == IN_ANALOG)
	dist_cm_f = rangeSensorAD.getRange();
#elif(SENSOR_INPUT_TYPE == IN_PWM)
	dist_cm_f = rangeSensorPW.getRange();
#endif

	// переводим в целочисельный тип для отправки по I2C
	dist_cm = (int16_t)dist_cm_f;

#if(_DEBUG == 1)
	Serial.println(dist_cm_f);
#endif
}
