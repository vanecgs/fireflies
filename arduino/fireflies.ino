/*
    Fades a line down the channels, with max value and duration based on
    the voltage of analog pin 0.
    Try grounding analog 0: everything should turn off.
    Try putting +5V into analog 0: everything should turn on.

    See the BasicUse example for hardware setup.

    Alex Leone <acleone ~AT~ gmail.com>, 2009-02-03 */

#include "Tlc5940.h"
#include "tlc_fades.h"

TLC_CHANNEL_TYPE channel;
int totalM = NUM_TLCS * 16 / 2;
int totalF = NUM_TLCS * 16 / 2;
int femaleIndex = totalM;
int maleInfo[8][3];
int femaleInfo[8][4];
int sensors[NUM_TLCS + 1];
boolean likesOther;

void setup()
{
  //Initializing the random number generator
  randomSeed(analogRead(1));
  Tlc.init();
  for(int i = 0; i < totalM; i++) {
    //Asignamos la intensidad del destello de las luciernagas macho
    maleInfo[i][0] = random(1024, 4096);
    //Asignamos la duracion del destello
    maleInfo[i][1] = random(700, 1300);
    //Tiempo entre destello
    maleInfo[i][2] = random(700, 7000);
  }
  
  for(int i = 0; i < totalF; i++) {
    //Asignamos la intensidad de destello que le gusta a cada hembras
    femaleInfo[i][0] = random(3024, 4096);
    //Asignamos la duracion del destello
    femaleInfo[i][1] = random(700, 1300);
    //Tiempo entre destello
    femaleInfo[i][2] = random(700, 7000);
  }
}

void loop()
{
  if (tlc_fadeBufferSize < TLC_FADE_BUFFER_LENGTH - 2) {
    
    //Enviamos la informacion de los destellos a los LEDs machos
    for(int i = 0; i < totalM; i++) {
      if (!tlc_isFading(i)) {
        uint32_t startMillis = millis() + maleInfo[i][2];
        uint32_t endMillis = startMillis + maleInfo[i][1];
        tlc_addFade(i, 0, maleInfo[i][0], startMillis, endMillis);
        tlc_addFade(i, maleInfo[i][0], 0, endMillis, endMillis + maleInfo[i][1]);
      }
      
      //Asignar un valos diferente para la intensidad de los destellos del macho
      //maleInfo[i][0] = random(1024, 4096);
    }
  
    for(int i = 0; i < totalF; i++) {
      likesOther = false;
      
      sensors[0] = analogRead(0);
      
      //Iteramos sobre los machos a ver si alguno le gusta a nuestra amiga
      for (int j = 0; j < totalM; j++) {
        //Restamos el nivel de luz en el ambiente a la potencia del flash del macho
        int maleFlash = maleInfo[j][0] - sensors[0];
        
        if(maleFlash > femaleInfo[i][0]) {
          likesOther = true;
          //Guardamos cual de los machos le gusto, siempre sera reemplazado por el de mayor intensidad
          femaleInfo[i][4] = maleInfo[j][0];
          
        }
      } 
      
      //Si encontramos un macho que le guste y no esta en el ciclo del fade, enviamos el destello
      if(likesOther == true && !tlc_isFading(i + femaleIndex)) {
        uint32_t startMillis = millis() + femaleInfo[i][2];
        uint32_t endMillis = startMillis + femaleInfo[i][1];
        tlc_addFade(i + femaleIndex, 0, femaleInfo[i][0], startMillis, endMillis);
        tlc_addFade(i + femaleIndex, femaleInfo[i][0], 0, endMillis, endMillis + femaleInfo[i][1]);
      }
    }  
  }
  
  
  
  tlc_updateFades();
}

