#include "Arduino.h"
#include "blinklib.h"

#define PLANET 1
#define SHIP -PLANET

#define MAX_MINERALS 6

int whatami;

byte mineraltype[6] = { 25, 45, 75, 150, 200, 250 };

// bug in library?  It's in the library, but no header declares it?
long random(long);
void randomSeed(unsigned long);
void setup() {
  whatami = PLANET;
  resetMyself();
}

byte currentMineralType[6];
byte currentMineralAmount[6] = { 255, 255, 255, 255, 255, 255 };
byte oldMineralAmount[6];

void randomizeMinerals() {
  randomSeed(millis());
  FOREACH_FACE(rm) {
    currentMineralType[rm] = mineraltype[random(MAX_MINERALS)];
    oldMineralAmount[rm] = currentMineralType[rm];
    currentMineralAmount[rm] = 255;
  }
}

void showMinerals () {
  FOREACH_FACE(sm) {
    setFaceColor( sm, makeColorHSB( currentMineralType[sm], 255, currentMineralAmount[sm] ) );
  }
}

void randomizePickup() {
  randomSeed(millis());
  byte newtype = mineraltype[random(MAX_MINERALS)]; //random does not seem to be 'exclusive'
  FOREACH_FACE(m) {
    currentMineralAmount[m] = 0;
    oldMineralAmount[m] = 0;
    currentMineralType[m] = newtype;
  }
  currentMineralAmount[0] = 255;
  oldMineralAmount[0] = 255;
}

void showPickup () {
  FOREACH_FACE(m) {
    setFaceColor( m, makeColorHSB( currentMineralType[m], 255, currentMineralAmount[m] ));
  }

}

void resetMyself() {
  if (whatami == PLANET) {
    randomizeMinerals();
    showMinerals();
  } else if (whatami == SHIP) {
    randomizePickup();
    showPickup();
  }
}

void moveShip () {
  for (int dim = 0; dim < 255; dim++) {
    delay(10);
    FOREACH_FACE(m) {
      setFaceColor( m, makeColorHSB( currentMineralType[m], 255, min(currentMineralAmount[m], dim) ));
    }
  }
  for (int blink = 0; blink < 3; blink++) {
    FOREACH_FACE(m) {
      setFaceColor( m, makeColorHSB( currentMineralType[m], 255, 0 ));
    }
    delay(150);
    FOREACH_FACE(m) {
      setFaceColor( m, makeColorHSB( currentMineralType[m], 255, currentMineralAmount[m] ));
    }
    delay(150);
  }
}

void drainPorts () {
  FOREACH_FACE(d) {
    while (irIsReadyOnFace(d)) {
      irGetData(d);
    }
  }
}


void takeMinerals (byte m) {
  if (irIsReadyOnFace(m)) {
    byte gotsome = irGetData(m);
    FOREACH_FACE(a) {
      if (currentMineralAmount[a] == 0 && gotsome > 0) {
        oldMineralAmount[a] = currentMineralAmount[a];
        currentMineralAmount[a] = 255;
        gotsome--;
      }
    }
  }
  //drainPorts();
  addMinerals();
}

void shipWins() {
  FOREACH_FACE(a) {
    setColor(WHITE);
    delay(200);
    setColor(makeColorHSB(currentMineralType[a], 255, 255));
    delay(200);
  }
}


void addMinerals() {
  int totalMinerals = 0;

  FOREACH_FACE(a) {
    totalMinerals += currentMineralAmount[a];
    while (oldMineralAmount[a] != currentMineralAmount[a]) {
      oldMineralAmount[a] += 5;
      setFaceColor(a, makeColorHSB( currentMineralType[a], 255, oldMineralAmount[a]));
      delay(15);
    }
  }

  if (totalMinerals > 255 * 5) {
    shipWins();
  }
}

void removeMinerals() {
  FOREACH_FACE(a) {
    while (oldMineralAmount[a] != currentMineralAmount[a]) {
      oldMineralAmount[a] -= 5;
      setFaceColor(a, makeColorHSB( currentMineralType[a], 255, oldMineralAmount[a]));
      delay(15);
    }
  }
}

void giveMinerals (byte m) {
  if (irIsReadyOnFace(m)) {
    byte typeofmineral = irGetData(m) * 5;
    byte amounttogive = 0;
    FOREACH_FACE(a) {
      oldMineralAmount[a] = currentMineralAmount[a];
      if (currentMineralType[a] == typeofmineral && currentMineralAmount[a] > 0) {
        amounttogive++;
        currentMineralAmount[a] = 0;
      }
    }
    if (amounttogive > 0) {
      irBroadcastData(amounttogive);
    }
  }
  //drainPorts();
  removeMinerals();
}

void rotateFaces() {
  byte first = currentMineralAmount[0];
  for (int f = 5; f > 0; f-- ) {
    currentMineralAmount[f - 1] = currentMineralAmount[f];
  }
  currentMineralAmount[5] = first;
  showMinerals();
}

unsigned long ct = 0;
void loop() {
  ct = millis();

  if (whatami == SHIP ) {
    irBroadcastData(currentMineralType[0] / 5);
  }

  FOREACH_FACE(m) {
    if (irIsReadyOnFace(m)) {
      if (whatami == SHIP) {
        takeMinerals(m);
      } else {
        giveMinerals(m);
      }
    }
  }

  if (buttonSingleClicked()) {
    if (whatami == SHIP ) {
      moveShip();
    }
  }
  if (buttonDoubleClicked()) {
    if (whatami == PLANET ) {
      resetMyself();
    }
  } else if (buttonMultiClicked()) {
    byte count = buttonClickCount();
    if (count == 3) {
      whatami = -whatami;
      resetMyself();
    }
  }
  /*
    if (ct % 250 == 0) {
      if (whatami == PLANET) {
        rotateFaces();
      }
   }*/
}
