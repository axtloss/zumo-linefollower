inline void animation(double lapTime)
{
  display.gotoXY(10, 4);
  display.print((char)255);
  delay(200);

  display.gotoXY(9, 4);
  display.print((char)255);

  display.gotoXY(11, 4);
  display.print((char)255);
  delay(200);

  display.gotoXY(12, 4);
  display.print((char)255);

  display.gotoXY(8, 4);
  display.print((char)255);
  delay(200);

  for (int l = 0; l < 2; l++)
    {
      display.clear();
      display.gotoXY(7, 4);
      display.print((char)255);
      display.gotoXY(13, 4);
      display.print((char)255);
      display.gotoXY(13, 3);
      display.print((char)255);
      display.gotoXY(13, 5);
      display.print((char)255);
      display.gotoXY(7, 3);
      display.print((char)255);
      display.gotoXY(7, 5);
      display.print((char)255);
      display.gotoXY(12, 3);
      display.print((char)255);
      display.gotoXY(12, 5);
      display.print((char)255);
      display.gotoXY(8, 3);
      display.print((char)255);
      display.gotoXY(8, 5);
      display.print((char)255);
      display.gotoXY(11, 3);
      display.print((char)255);
      display.gotoXY(11, 5);
      display.print((char)255);
      display.gotoXY(9, 3);
      display.print((char)255);
      display.gotoXY(9, 5);
      display.print((char)255);
      display.gotoXY(10, 3);
      display.print((char)255);
      display.gotoXY(10, 5);
      display.print((char)255);

      display.gotoXY(8, 4);
      display.print(lapTime);

      delay(500);

      display.clear();
      display.gotoXY(8, 4);
      display.print(lapTime);

      delay(500);
    }
}
