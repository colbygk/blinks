#include "blinklib.h"

#define I_AM_NOTHING -1
#define I_AM_A_PUCK 0
int whatami = I_AM_A_PUCK;
int whatareyou = I_AM_NOTHING;

#define I_AM_GOAL_ONE 1
#define I_AM_GOAL_TWO 2

void setup() {
  whatami = I_AM_A_PUCK;
  setColor( RED );
  delay(200);
  setColor( GREEN );
  delay(200);
  setColor( RED );
  delay(200);
  setColor( WHITE );
}

void loop() {

  if (buttonSingleClicked()) {
    setup();
  }
  if (buttonDoubleClicked()) {
    if (whatami == I_AM_GOAL_ONE) {
      whatami = I_AM_GOAL_TWO;
    } else {
      whatami = I_AM_GOAL_ONE;
    }
  }

  switch (whatami) {
    case I_AM_A_PUCK:
        setColor( WHITE );
      break;
    case I_AM_GOAL_ONE:
      setColor( YELLOW );
      break;
    case I_AM_GOAL_TWO:
      setColor( RED );
      break;          
  }

  // Talk to other tiles
  FOREACH_FACE(face) {
    if (whatami == I_AM_A_PUCK) {
      if (irIsReadyOnFace(face)) {
        whatareyou = irGetData(face);
        if (whatareyou > I_AM_A_PUCK) {
          whatami = whatareyou;
        }
        while (irIsReadyOnFace(face)) {
          irGetData(face);
        }
      }
    } else {
      if (irIsReadyOnFace(face)) {
        
      }
      irBroadcastData( whatami );
        while (irIsReadyOnFace(face)) {
          irGetData(face);
        }
    }
    
  }

}
