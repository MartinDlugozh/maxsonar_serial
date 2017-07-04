# Sonar interface converter

ПО преобразователя интерфейсов для ультразвуковых дальномеров MB1043 HRLV-MaxSonar-EZ4 и HC-SR04.
Необходимо в том случае, если АП неадекватно работает или не работает вообще с датчиком напрямую.
 
С помощью директив препроцессора можно выбрать необходимый вариант компиляции прошивки
путем раскомментирования строки с необходимым значением параметра и подавлением ненужного):

**SENSOR_TYPE** может принимать значения **SENSOR_MAXSONAR** или **SENSOR_HC_SR04** в соответстви с типом подключаемого сенсора.

SENSOR_INPUT_TYPE можно установить в один из доступных типов:
1. Для сенсоров **_Maxbotix_**:
  * **IN_MX_SERIAL** - читать данные с цифрового последовательного интерфейса датчика
  * **IN_MX_ANALOG** - читать с аналогового выхода
  * **IN_MX_PWM** - читать с выхода ШИМ
2. Для **_HC-SR04_**:
  * **HC_SR04_TE** - использовать порт HC_SR04_TE (+5V, TRIGGER, ECHO, GND)

Также можно задавать следующие параметры:
_DEBUG - активирует передачу результатов замеров на последовательный порт Serial (USB);
I2C_SENSOR_ADDRESS - адрес МК-преобразователя для подключения к АП (может меняться в зависимости от параметров АП.

# История версий:

## v.1.0.1
Начальная версия без правок. Реализована поддержка только сенсоров линейки Maxbotix.

## v.1.1.0
Реализована поддержка всех сенсоров линейки Maxbotix, а также сенсоров HC-SR04.
Внесены правки в документацию, добавлено множество комментариев к коду.