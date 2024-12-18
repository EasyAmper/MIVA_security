void alarm() {  // работа сигнализации
  if (millis() - tim > 900) {
   // Serial.println("тревога alarm");
    digitalWrite(R2, BLfl);
    BLfl = !BLfl;
    digitalWrite(R1, BLfl);
    alarm_TMR++;
    tim = millis();
  }
  if (alarm_TMR >= 200 and digitalRead(DoorSW) == 1) {  // если прошло около 3 мин и двери закрыты
    alarmState = 0;  // выключить тревогу
    alarm_TMR=0;  // сброс таймера
    digitalWrite(R1, 1);
    digitalWrite(R2, 1);
  }
}
