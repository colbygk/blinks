#include "blinklib.h"

void setup() {
  // No setup needed for this simple example!  
}

unsigned long t = 0;
byte hue = 255;
byte brightness = 50;
byte bidx = 0;
int bmod[6] = {1,1,1,1,1,1};
byte maxb = 250;
byte minb = 30;
int upb = 2;
int downb = -1;
int speed = 3;
byte itv = 30;
byte b[6] = {minb,minb+itv,minb+(itv*2),minb+(itv*3),minb+(itv*5),minb+(itv*6)};
byte mode = 0;

void loop() {
    t = millis();

    if (buttonDoubleClicked()) {
      mode = 1;
    }
    if (buttonSingleClicked()) {
      mode = 0;
    }
    
    switch (mode) {

      case 0:
        if (t%speed==0) 
        {
          for (bidx=0; bidx<6; bidx++) {
            if (b[bidx] > maxb)
            {
              bmod[bidx] = downb;
              b[bidx] = maxb;
            } else if (b[bidx] < minb) {
              bmod[bidx] = upb;
              b[bidx] = minb;   
            }
            b[bidx] += bmod[bidx];
            setFaceColor( bidx, makeColorHSB( 150, 255, b[bidx] ));   
          }    
        }
        break;
      case 1:
        FOREACH_FACE(bidx) {
          setFaceColor( bidx, makeColorHSB( 25, 100, 200 ));   
        }
        break;
    }
}


