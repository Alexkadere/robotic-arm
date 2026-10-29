#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <Preferences.h>

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40);
Preferences preferences;

// =====================================================
// CANAUX PCA9685
// =====================================================

const int BASE = 0;
const int EPAULE = 1;
const int COUDE = 2;
const int POIGNET = 3;
const int ROTATION_POIGNET = 4;

// =====================================================
// JOYSTICK
// =====================================================

const int JOY_X = 34;
const int JOY_Y = 35;
const int BOUTON = 32;

// =====================================================
// POSITIONS
// =====================================================

float positionBase;
float positionEpaule;
float positionCoude;
float positionPoignet;
float positionRotationPoignet;

// =====================================================
// LIMITES
// =====================================================

const int BASE_MIN = 500;
const int BASE_MAX = 2500;

const int EPAULE_MIN = 1000;
const int EPAULE_MAX = 2000;

const int COUDE_MIN = 500;
const int COUDE_MAX = 2500;

const int POIGNET_MIN = 700;
const int POIGNET_MAX = 2300;

const int ROTATION_POIGNET_MIN = 500;
const int ROTATION_POIGNET_MAX = 2500;

// =====================================================
// VITESSES
// =====================================================

const float VITESSE_BASE = 15.0;
const float VITESSE_EPAULE = 8.0;
const float VITESSE_COUDE = 8.0;
const float VITESSE_POIGNET = 8.0;
const float VITESSE_ROTATION_POIGNET = 15;

// =====================================================
// DEADZONES
// =====================================================

const int DEADZONE = 300;

// Plus grande pour éviter que le MF90 tourne tout seul
const int DEADZONE_ROTATION = 500;

// =====================================================
// MODES
// =====================================================

int mode = 1;
bool ancienBouton = HIGH;

// =====================================================
// SAUVEGARDE
// =====================================================

unsigned long dernierMouvement = 0;
bool positionASauvegarder = false;

const unsigned long DELAI_SAUVEGARDE = 500;

// =====================================================
// LECTURE JOYSTICK
// =====================================================

float lireJoystick(int pin, int deadzone) {

  int valeur = analogRead(pin);
  int ecart = valeur - 2048;

  if (abs(ecart) < deadzone) {
    return 0;
  }

  return (float)ecart / 2048.0;
}

// =====================================================
// SAUVEGARDE DES POSITIONS
// =====================================================

void sauvegarderPositions() {

  preferences.putInt(
    "base",
    (int)positionBase
  );

  preferences.putInt(
    "epaule",
    (int)positionEpaule
  );

  preferences.putInt(
    "coude",
    (int)positionCoude
  );

  preferences.putInt(
    "poignet",
    (int)positionPoignet
  );

  preferences.putInt(
    "rotPoignet",
    (int)positionRotationPoignet
  );

  positionASauvegarder = false;
}

// =====================================================
// SETUP
// =====================================================

void setup() {

  Wire.begin(21, 22);

  pwm.begin();
  pwm.setPWMFreq(50);

  pinMode(JOY_X, INPUT);
  pinMode(JOY_Y, INPUT);
  pinMode(BOUTON, INPUT_PULLUP);

  preferences.begin("robot", false);

  // ===================================================
  // CHARGEMENT DES DERNIÈRES POSITIONS
  // ===================================================

  positionBase =
    preferences.getInt("base", 1500);

  positionEpaule =
    preferences.getInt("epaule", 1500);

  positionCoude =
    preferences.getInt("coude", 1500);

  positionPoignet =
    preferences.getInt("poignet", 1500);

  positionRotationPoignet =
    preferences.getInt("rotPoignet", 1500);

  // ===================================================
  // SÉCURITÉ LIMITES
  // ===================================================

  positionBase =
    constrain(
      positionBase,
      BASE_MIN,
      BASE_MAX
    );

  positionEpaule =
    constrain(
      positionEpaule,
      EPAULE_MIN,
      EPAULE_MAX
    );

  positionCoude =
    constrain(
      positionCoude,
      COUDE_MIN,
      COUDE_MAX
    );

  positionPoignet =
    constrain(
      positionPoignet,
      POIGNET_MIN,
      POIGNET_MAX
    );

  positionRotationPoignet =
    constrain(
      positionRotationPoignet,
      ROTATION_POIGNET_MIN,
      ROTATION_POIGNET_MAX
    );

  // ===================================================
  // ENVOI POSITIONS AUX SERVOS
  // ===================================================

  pwm.writeMicroseconds(
    BASE,
    (int)positionBase
  );

  pwm.writeMicroseconds(
    EPAULE,
    (int)positionEpaule
  );

  pwm.writeMicroseconds(
    COUDE,
    (int)positionCoude
  );

  pwm.writeMicroseconds(
    POIGNET,
    (int)positionPoignet
  );

  pwm.writeMicroseconds(
    ROTATION_POIGNET,
    (int)positionRotationPoignet
  );

  delay(1000);
}

// =====================================================
// LOOP
// =====================================================

void loop() {

  // ===================================================
  // BOUTON : MODE 1 → 2 → 3 → 1
  // ===================================================

  bool bouton = digitalRead(BOUTON);

  if (
    ancienBouton == HIGH &&
    bouton == LOW
  ) {

    mode++;

    if (mode > 3) {
      mode = 1;
    }

    delay(200);
  }

  ancienBouton = bouton;

  // ===================================================
  // LECTURE JOYSTICK
  // ===================================================

  float x;
  float y;

  // Mode 3 = deadzone plus grande sur X
  if (mode == 3) {

    x = lireJoystick(
      JOY_X,
      DEADZONE_ROTATION
    );

    y = lireJoystick(
      JOY_Y,
      DEADZONE
    );
  }

  else {

    x = lireJoystick(
      JOY_X,
      DEADZONE
    );

    y = lireJoystick(
      JOY_Y,
      DEADZONE
    );
  }

  bool mouvement = false;

  // ===================================================
  // MODE 1
  //
  // X = BASE
  // Y = EPAULE
  // ===================================================

  if (mode == 1) {

    if (x != 0) {

      positionBase +=
        x * VITESSE_BASE;

      positionBase =
        constrain(
          positionBase,
          BASE_MIN,
          BASE_MAX
        );

      pwm.writeMicroseconds(
        BASE,
        (int)positionBase
      );

      mouvement = true;
    }

    if (y != 0) {

      positionEpaule +=
        y * VITESSE_EPAULE;

      positionEpaule =
        constrain(
          positionEpaule,
          EPAULE_MIN,
          EPAULE_MAX
        );

      pwm.writeMicroseconds(
        EPAULE,
        (int)positionEpaule
      );

      mouvement = true;
    }
  }

  // ===================================================
  // MODE 2
  //
  // X = COUDE
  // Y = INCLINAISON POIGNET
  // ===================================================

  else if (mode == 2) {

    if (x != 0) {

      positionCoude +=
        x * VITESSE_COUDE;

      positionCoude =
        constrain(
          positionCoude,
          COUDE_MIN,
          COUDE_MAX
        );

      pwm.writeMicroseconds(
        COUDE,
        (int)positionCoude
      );

      mouvement = true;
    }

    if (y != 0) {

      positionPoignet +=
        y * VITESSE_POIGNET;

      positionPoignet =
        constrain(
          positionPoignet,
          POIGNET_MIN,
          POIGNET_MAX
        );

      pwm.writeMicroseconds(
        POIGNET,
        (int)positionPoignet
      );

      mouvement = true;
    }
  }

  // ===================================================
  // MODE 3
  //
  // X = ROTATION POIGNET
  // Y = LIBRE POUR LA FUTURE PINCE
  // ===================================================

  else if (mode == 3) {

  // Joystick au repos = STOP
  if (x == 0) {

    pwm.writeMicroseconds(
      ROTATION_POIGNET,
      1500
    );
  }

  // Joystick vers un côté
  else {

    int commandeRotation;

    if (x > 0) {

      commandeRotation = map(
        (int)(x * 1000),
        0,
        1000,
        1500,
        2500
      );
    }

    else {

      commandeRotation = map(
        (int)(x * 1000),
        -1000,
        0,
        500,
        1500
      );
    }

    pwm.writeMicroseconds(
      ROTATION_POIGNET,
      commandeRotation
    );
  }
}

  // ===================================================
  // DÉTECTION MOUVEMENT
  // ===================================================

  if (mouvement) {

    dernierMouvement = millis();
    positionASauvegarder = true;
  }

  // ===================================================
  // SAUVEGARDE APRÈS 0,5 SECONDE SANS MOUVEMENT
  // ===================================================

  if (
    positionASauvegarder &&
    millis() - dernierMouvement >
    DELAI_SAUVEGARDE
  ) {

    sauvegarderPositions();
  }

  delay(20);
}