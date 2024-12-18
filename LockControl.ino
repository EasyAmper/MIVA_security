void LockControl() {  // управление центральным замком(актуатором)
  if (LockState == 0) { // снято с охраны   lock off
    digitalWrite(LED_PIN, 0);
    digitalWrite(R3, 0);
    digitalWrite(R1, 0);
    delay(LockDel);
    digitalWrite(R3, 1);
    digitalWrite(R1, 1);
    power.sleep(SLEEP_2048MS);
   // Serial.println("Снято с охраны");
  }
  if (LockState == 1) { // на охране  lock on
    digitalWrite(R4, 0);
    digitalWrite(R1, 0);
    delay(LockDel);
    digitalWrite(R4, 1);
    digitalWrite(R1, 1);
    delay(500);
    digitalWrite(R1, 0);
    delay(500);
    digitalWrite(R1, 1);
    power.sleep(SLEEP_8192MS);  // обход проблемы с АПС и концевиком на 2110 
   // Serial.println("Поставлено на охрану");
  }
}
