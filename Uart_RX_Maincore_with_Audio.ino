/*
 *  Main.ino - MP Example to communicate message data
 *  Copyright 2019 Sony Semiconductor Solutions Corporation
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifdef SUBCORE
#error "Core selection is wrong!!"
#endif

#include <MP.h>

#include <Arduino.h>
#include <Arduino_JSON.h>

int subcore = 1; /* Communication with SubCore1 */

// for Audio  //
#include <SDHCI.h>
#include <Audio.h>

SDClass theSD;
AudioClass *theAudio;

File myFile;

bool ErrEnd = false;

/**
 * @brief Audio attention callback
 *
 * When audio internal error occurs, this function will be called back.
 */

static void audio_attention_cb(const ErrorAttentionParam *atprm)
{
  puts("Attention!");
  
  if (atprm->error_code >= AS_ATTENTION_CODE_WARNING)
    {
      ErrEnd = true;
   }
}


void setup()
{
  int ret = 0;

  Serial.begin(115200);
  while (!Serial);

  // Serial2ポートを初期化
  Serial2.begin(115200);

  /* Initialize SD */
  while (!theSD.begin())
    {
      /* wait until SD card is mounted. */
      Serial.println("Insert SD card.");
    }


  /* Launch SubCore1 */
  ret = MP.begin(subcore);
  if (ret < 0) {
    printf("MP.begin error = %d\n", ret);
  }

  //randomSeed(100);
}

void loop()
{
  int      ret;
  uint32_t snddata;
  uint32_t rcvdata;
  int8_t   sndid = 100; /* user-defined msgid */
  int8_t   rcvid;

  if (Serial2.available() > 0) {
    // 受信したデータをバッファに読み込む
    String receivedString = Serial2.readStringUntil('\n');
    
    // JSONオブジェクトを解析
    JSONVar jsonObj = JSON.parse(receivedString);
    
    // 解析エラーのチェック
    if (JSON.typeof(jsonObj) == "undefined") {
      Serial.println("Parsing input failed!");
      return;
    }
    
    // JSONデータを取得
    const char* message = (const char*) jsonObj["message"];
    int value = (int) jsonObj["point"];

    // 受信したメッセージをシリアルモニターに出力
    Serial.println("Received JSON: ");
    Serial.println(receivedString);

    snddata = value;
  /* Echo back from SubCore */

  printf("Send: id=%d data=0x%08lx\n", sndid, snddata);

  ret = MP.Send(sndid, snddata, subcore);
  if (ret < 0) {
    printf("MP.Send error = %d\n", ret);
  }

    soundplay(value);
 
  /* Timeout 1000 msec */
  //MP.RecvTimeout(1000);
  //
  //ret = MP.Recv(&rcvid, &rcvdata, subcore);
  //if (ret < 0) {
  //  printf("MP.Recv error = %d\n", ret);
  //}
  //
  //printf("Recv: id=%d data=0x%08lx : %s\n", rcvid, rcvdata,
  //       (snddata == rcvdata) ? "Success" : "Fail");
  //
  //delay(1000);
  }
}

void soundplay(int pattern){
  // start audio system
  theAudio = AudioClass::getInstance();

  theAudio->begin(audio_attention_cb);

  puts("initialization Audio Library");

  /* Set clock mode to normal */
  theAudio->setRenderingClockMode(AS_CLKMODE_NORMAL);

  /* Set output device to speaker with first argument.
   * If you want to change the output device to I2S,
   * specify "AS_SETPLAYER_OUTPUTDEVICE_I2SOUTPUT" as an argument.
   * Set speaker driver mode to LineOut with second argument.
   * If you want to change the speaker driver mode to other,
   * specify "AS_SP_DRV_MODE_1DRIVER" or "AS_SP_DRV_MODE_2DRIVER" or "AS_SP_DRV_MODE_4DRIVER"
   * as an argument.
   */
  theAudio->setPlayerMode(AS_SETPLAYER_OUTPUTDEVICE_SPHP, AS_SP_DRV_MODE_LINEOUT);

  /*
   * Set main player to decode stereo mp3. Stream sample rate is set to "auto detect"
   * Search for MP3 decoder in "/mnt/sd0/BIN" directory
   */
  err_t err = theAudio->initPlayer(AudioClass::Player0, AS_CODECTYPE_MP3, "/mnt/sd0/BIN", AS_SAMPLINGRATE_AUTO, AS_CHANNEL_STEREO);

  /* Verify player initialize */
  if (err != AUDIOLIB_ECODE_OK)
    {
      printf("Player0 initialize error\n");
      exit(1);
    }

  /* Open file placed on SD card */
  switch (pattern){
    case 1:
      //Serial.println("happy");  
      myFile = theSD.open("Applause01.mp3");
      break;
    case 2:
      //Serial.println("sad");  
      myFile = theSD.open("Applause02.mp3");
      break;
    case 3:
      //Serial.println("anger");  
      myFile = theSD.open("Applause03.mp3");
      break;
    case 4:
      //Serial.println("anger");  
      myFile = theSD.open("Applause04.mp3");
      break;

  }

  /* Verify file open */
  if (!myFile)
    {
      printf("File open error\n");
      exit(1);
    }
  printf("Open! 0x%08lx\n", (uint32_t)myFile);
\
  /* Send first frames to be decoded */
  err = theAudio->writeFrames(AudioClass::Player0, myFile);

  if ((err != AUDIOLIB_ECODE_OK) && (err != AUDIOLIB_ECODE_FILEEND))
    {
      printf("File Read Error! =%d\n",err);
      myFile.close();
      exit(1);
    }

  puts("Play!");

  /* Main volume set to -16.0 dB */
  theAudio->setVolume(-80);
  theAudio->startPlayer(AudioClass::Player0);
/**
 * @brief Play stream
 *
 * Send new frames to decode in a loop until file ends
 */


  while(1){
   puts("loop!!");

  /* Send new frames to decode in a loop until file ends */
  int err_0 = theAudio->writeFrames(AudioClass::Player0, myFile);

  /*  Tell when player file ends */
  if (err_0 == AUDIOLIB_ECODE_FILEEND)
    {
      printf("Main player File End!\n");
    }

  /* Show error code from player and stop */
  if (err_0)
    {
      printf("Main player error code: %d\n", err_0);
      stop_player();
      break;
//      goto stop_player;
    }

  if (ErrEnd)
    {
      printf("Error End\n");
      stop_player();
      break;
//      goto stop_player;
    }

  /* This sleep is adjusted by the time to read the audio stream file.
   * Please adjust in according with the processing contents
   * being processed at the same time by Application.
   *
   * The usleep() function suspends execution of the calling thread for usec
   * microseconds. But the timer resolution depends on the OS system tick time
   * which is 10 milliseconds (10,000 microseconds) by default. Therefore,
   * it will sleep for a longer time than the time requested here.
   */

  usleep(40000);


  /* Don't go further and continue play */
  return;

   
  }

  Serial.println("Loopend");  

}


void stop_player(){
  theAudio->stopPlayer(AudioClass::Player0);
  myFile.close();
  theAudio->setReadyMode();
  theAudio->end();
}

