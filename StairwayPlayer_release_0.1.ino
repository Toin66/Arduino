#include "SoftwareSerial.h"
#include "IRremote.h"
# define Start_Byte 0x7E
# define Version_Byte 0xFF
# define Command_Length 0x06
# define End_Byte 0xEF
# define Acknowledge 0x00 //Returns info with command 0x41 [0x01: info, 0x00: no info]
# define ACTIVATED LOW
SoftwareSerial mySerial(9, 8); // Rx, Tx

int button_play1 = 10; //кнопка замыкатель 1
int button_play2 = 11; //кнопка замыкатель 2
int enable_play_led = 13; //светодиод активации воспроизведения
int volume; //громкость
int track_num; //номер трэка

boolean enable_play; //активация проигрывания при срабатывании
boolean enable_test_play; //флаг тестового воспроизведения
boolean playing_now; //флаг воспроизведения

unsigned long last_time; //время для задежки
unsigned long last_time_test_play;

IRrecv irrecv(5);  //5 - стандарт,

decode_results results;

void execute_CMD(byte CMD, byte Par1, byte Par2)
{
  word checksum = -(Version_Byte + Command_Length + CMD + Acknowledge + Par1 + Par2);
  byte Command_line[10] = { Start_Byte, Version_Byte, Command_Length, CMD, Acknowledge,
                            Par1, Par2, highByte(checksum), lowByte(checksum), End_Byte
                          };
  for (byte k = 0; k < 10; k++)
  {
    mySerial.write( Command_line[k]);
  }
}

void play ()
{
  if (millis() - last_time > 1000)
  {
    last_time = millis();

    if (enable_play) {
      digitalWrite(enable_play_led, HIGH);

      if ((digitalRead(button_play1) == ACTIVATED && playing_now == false) || (digitalRead(button_play2) == ACTIVATED && playing_now == false))
      {
        execute_CMD(0x0F, 01 , track_num);
        playing_now = true;
        //Serial.println( "play music");
      }

      else if ((enable_test_play) && (millis() - last_time_test_play > 5000)) {
        execute_CMD(0x16, 0, 0);
        enable_test_play = false;
        //Serial.println( "stop test play");
      }

      else if ((digitalRead(button_play1) == ACTIVATED && playing_now == true) || (digitalRead(button_play2) == ACTIVATED && playing_now == true) || (playing_now == false))
      {
        //Serial.println( "nothing");
      }


      else
      {
        if (enable_test_play == false) {
          execute_CMD(0x16, 0, 0); //выключаем музыку если подсветка не горит
          playing_now = false;
        //  Serial.println( "stop play");
        }
      }


//      Serial.println("track num:");
//      Serial.println(track_num);
//      Serial.println("volume is:");
//      Serial.println(volume);
//      Serial.println("enable_play:");
//      Serial.println(enable_play);
//      Serial.println("enable_test_play");
//      Serial.println(enable_test_play);
//      Serial.println("playing_now");
//      Serial.println(playing_now);
    }


    else  {
      digitalWrite(enable_play_led, LOW);
      execute_CMD(0x16, 0, 0);
    }
  }
}

void test_play ()
{
  enable_test_play = true;
  execute_CMD(0x06, 00, volume);
  delay(100);
  execute_CMD(0x0F, 01 , track_num);
 // Serial.println( "start test play");
  last_time_test_play = millis();
}



void setup ()
{
  pinMode(button_play1, INPUT);
  pinMode(button_play2, INPUT);
  pinMode(enable_play_led, OUTPUT);

  mySerial.begin (9600);
  Serial.begin(9600);
  irrecv.enableIRIn();

  enable_play = true;
  enable_test_play = false;
  playing_now = false;
  volume = 25;
  track_num = 1;
  execute_CMD(0x16, 0, 0);
  delay(100);
  execute_CMD(0x06, 00, volume); //задаем громкость
}

void loop() {

  play();

  if ( irrecv.decode( &results )) {

   // Serial.println( results.value, HEX );

    switch ( results.value ) {
      case 0xFFA25D: //1
        track_num = 1;
        test_play();
        break;

      case 0xFF629D: //2
        track_num = 2;
        test_play();
        break;

      case 0xFFE21D: //3
        track_num = 3;
        test_play();
        break;

      case 0xFF22DD: //4
        track_num = 4;
        test_play();
        break;

      case 0xFF02FD: //5
        track_num = 5;
        test_play();
        break;

      case 0xFFC23D: //6
        track_num = 6;
        test_play();
        break;

      case 0xFFE01F: //7
        track_num = 7;
        test_play();
        break;

      case 0xFFA857: //8
        track_num = 8;
        test_play();
        break;

      case 0xFF906F: //9
        track_num = 9;
        test_play();
        break;

      case 0xFF9867: //0
        track_num = 10;
        test_play();
        break;

      case 0xFF6897: //*
        enable_play = true;
        break;

      case 0xFFB04F: //#
        enable_test_play = false;
        enable_play = false;
        playing_now = false;
        break;

      case 0xFF38C7: //OK

        break;

      case 0xFF18E7: //^
        if (volume < 30)
          volume++;
        test_play();   
        break;

      case 0xFF4AB5: //V
        if (volume > 1)
          volume--;
        test_play();
        break;

      case 0xFF10EF: //<
        if (track_num == 1)
          track_num = 10;
        else track_num--;
        test_play();
        break;

      case 0xFF5AA5: //>
        if (track_num == 10)
          track_num = 1;
        else track_num++;
        test_play();
        break;
    }
    irrecv.resume();
  }

}
