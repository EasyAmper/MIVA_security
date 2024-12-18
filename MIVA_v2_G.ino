// MIVA v2.3 global ed
// система охраны и управления замком

#include <GyverPower.h>  // sleep lib
#include <SPI.h>
#include <MFRC522.h>  // RFID lib

#define LockDel 600   // длительность работы актуатора 
#define RST_period 40   // примерный период сброса модуля в режиме охраны (byte)

#define RST_PIN 9 // RFID reader
#define SS_PIN 10 // RFID reader  
#define DoorSW 4  // концевики
#define LED_PIN 3
#define R1 16  // фары
#define R2 17  // сигнал
#define R3 18  // актуатор
#define R4 19  // 

MFRC522 rfid(SS_PIN, RST_PIN);   // Create MFRC522 instance.

// 3 UID метки
byte trueUID1[] = {0xE3, 0x01, 0x01, 0x01};  //A0 51 ....
byte trueUID2[] = {0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01};
byte trueUID3[] = {0x01, 0x01, 0x01, 0x58};

bool uidMatch;    // соответствие метки 1 - верная
bool LockState = 1; // состояние замка при запуске 1 - под охраной
bool alarmState = 0; // состояние тревоги 1 - активна
bool ledFL;
bool BLfl;
byte RST_TMR; // таймер для сброса модуля (костыль)
byte alarm_TMR;
unsigned long tim;            // для таймеров

void setup() {
  pinMode(DoorSW, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  pinMode(R1, OUTPUT);
  pinMode(R2, OUTPUT);
  pinMode(R3, OUTPUT);
  pinMode(R4, OUTPUT);
  digitalWrite(R1, 1);
  digitalWrite(R2, 1);
  digitalWrite(R3, 1);
  digitalWrite(R4, 1);
  // Serial.begin(9600);
  SPI.begin();        // Init SPI bus
  rfid.PCD_Init(); // Init MFRC522
  power.autoCalibrate();
  power.hardwareDisable(PWR_I2C | PWR_ADC);  // отключаем ненужное
  power.setSleepMode(STANDBY_SLEEP); // если нужен другой режим сна, см константы в GyverPower.h (по умолчанию POWERDOWN_SLEEP)
  power.bodInSleep(false); // рекомендуется выключить bod во сне для сохранения энергии (по умолчанию false - выключен!!)
}

void loop() {

  if (LockState == 1) {   // действия на охране
    if (digitalRead(DoorSW) == 0)alarmState = 1; // контроль концевиков
    if (alarmState == 0) {
      digitalWrite(LED_PIN, 1);
      power.sleep(SLEEP_16MS);
      digitalWrite(LED_PIN, 0);
      power.sleep(SLEEP_1024MS);
      RST_TMR++;
      if (RST_TMR >= RST_period) { // сброс по таймеру на охране
        RFrst();
        RST_TMR = 0;
      }
    } else {
      alarm();
      digitalWrite(LED_PIN, 1);
    }
  }
  if (LockState == 0) {   // когда охрана снята
    static uint32_t rebootTimer = millis(); // Важный костыль против зависания модуля!
    if (millis() - rebootTimer >= 1000) {   // Таймер с периодом 1000 мс
      rebootTimer = millis();               // Обновляем таймер
      RFrst();
    }
    if (digitalRead(DoorSW) == 0) {
      digitalWrite(LED_PIN, 1);   // для проверки сигналов концевиков
    } else {
      digitalWrite(LED_PIN, 0);
    }
  }

  if (rfid.PICC_IsNewCardPresent()) {    // карта поднесена
    if (rfid.PICC_ReadCardSerial()) {
      uidMatch = true;    // заранее определяем метку как верную
      for (byte i = 0; i < rfid.uid.size; i++) {
        if (rfid.uid.uidByte[i] != trueUID1[i] && rfid.uid.uidByte[i] != trueUID2[i] && rfid.uid.uidByte[i] != trueUID3[i]) {
          uidMatch = false;  // если проверка провалена меняем на неверную
          break;
        }
      }
      if (uidMatch) {  // метка верная
        alarmState = 0; // сброс тревоги
        alarm_TMR = 0;
        digitalWrite(LED_PIN, 0);  // гасим, если остался гореть
        LockState = !LockState;  // сменить состояние охраны
        LockControl();  // вызов срабатывания актуатора
      } else { // метка неверная
        //Serial.println("Ошибка/error UID");
        digitalWrite(R2, 0); // бибикнуть долго
        digitalWrite(R1, 0); // моргать фарами
        delay(500);
        digitalWrite(R1, 1);
        delay(500);
        digitalWrite(R2, 1);
        digitalWrite(R1, 0);
        delay(500);
        digitalWrite(R1, 1);
        delay(500);
        digitalWrite(R1, 0);
        delay(500);
        digitalWrite(R1, 1);
      }
      rfid.PICC_HaltA(); // конец общения с меткой
    }
  }
}
void RFrst() { // костыль против зависания модуля
  digitalWrite(RST_PIN, HIGH);          // Сбрасываем модуль
  delayMicroseconds(2);                 // Ждем 2 мкс
  digitalWrite(RST_PIN, LOW);           // Отпускаем сброс
  rfid.PCD_Init();                      // Инициализируем заного
}
