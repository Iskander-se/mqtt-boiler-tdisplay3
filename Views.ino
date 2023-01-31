
void View2Temp() {
  ViewBG();
  tft.setTextSize(7);
  tft.setTextColor(TFT_GOLD, TFT_BLACK);
  tft.drawString(String(xSensor1.charBuf), 65, 36);
  tft.drawXBitmap(5, 34, thermometer48, 48, 48, TFT_GOLD);
  tft.setTextSize(5);
  tft.setTextColor(TFT_ORANGE, TFT_BLACK);
  tft.drawXBitmap(2, 98, sun36, 36, 36, TFT_ORANGE, TFT_BLACK);
  tft.drawString(String(xSensor2.charBuf), 38, 98);
}


void ViewCase2(int timer1) {
  ViewBG();
  ViewSmallTemp();
  tft.setTextSize(8);
  tft.setTextColor(TFT_RED, TFT_BLACK);
  if (timer1 > 9) tft.drawString( String(timer1) + "min", 18, 36);
  else tft.drawString(" " + String(timer1) + "min ", 8, 36);
}

void ViewCaseOTA() {
  ViewBG();
  ViewSmallTemp();
  tft.setTextSize(5);
  tft.setTextColor(TFT_RED, TFT_BLACK);
  tft.drawString("OTA ON", 35, 41);
}

void ViewSmallTemp() {
  tft.setTextSize(3);
  tft.setTextColor(TFT_GOLD, TFT_BLACK);
  tft.drawString(String(xSensor1.charBuf), 1, 114);
  tft.setTextColor(TFT_ORANGE, TFT_BLACK);
  tft.drawString(String(xSensor2.charBuf), 92, 114);
  tft.drawXBitmap(74, 116, sun18, 18, 18, TFT_ORANGE);
}

void ViewBG() {
  tft.setTextSize(2);
  tft.setTextColor(TFT_NAVY, TFT_BLACK);
  tft.drawString(String(cStatus), 1, 2);
  tft.setTextColor(TFT_SILVER, TFT_BLACK);
  tft.drawString("ON (+10min)", 105, 2);

  tft.setTextSize(3);
  tft.setTextColor(TFT_RED, TFT_BLACK);
  tft.drawString("STOP", 168, 114);

}
