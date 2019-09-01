const int OPEN_SIGN = 2;
const int CLOSED_SIGN = 3;
const int SOLENOID_NORMAL = 4;
const int SOLENOID_REVERSE = 5;
const int MOTOR_NORMAL = 6;
const int MOTOR_REVERSE = 7;
const int SETTING_BTN = 8;
const int SOLENOID_FAULT = 9;
const int MOTOR_FAULT = 10;
const int POSITIONMETER = A0;

boolean openSign;
boolean closedSign;
boolean settingSign;

int nowPos_old = 0;
int nowPos;
int targetPos;

const int MAX_POS = 800;
int OPEN_POS = 0;
int CLOSED_POS = 350;
int MAX_RANGE = abs(OPEN_POS - CLOSED_POS) + 100;

void setup() {
  // put your setup code here, to run once:
  pinMode(OPEN_SIGN, INPUT_PULLUP);
  pinMode(CLOSED_SIGN, INPUT_PULLUP);
  pinMode(MOTOR_NORMAL, OUTPUT);
  pinMode(MOTOR_REVERSE, OUTPUT);
  pinMode(SOLENOID_NORMAL, OUTPUT);
  pinMode(SOLENOID_REVERSE, OUTPUT);
  pinMode(SETTING_BTN, INPUT_PULLUP);
  pinMode(POSITIONMETER, INPUT);
  pinMode(SOLENOID_FAULT, INPUT_PULLUP);
  pinMode(MOTOR_FAULT, INPUT_PULLUP);

  digitalWrite(SOLENOID_NORMAL, LOW);
  digitalWrite(SOLENOID_REVERSE, HIGH);
  delay(200);
  digitalWrite(SOLENOID_NORMAL, LOW);
  digitalWrite(SOLENOID_REVERSE, HIGH);
  delay(200);
  digitalWrite(SOLENOID_NORMAL, LOW);
  digitalWrite(SOLENOID_REVERSE, LOW);

  Serial.begin(9600);
  stopMotor();
}

void loop() {
  // put your main code here, to run repeatedly:
  getKeySign();

  getPositionMeter();

  if (abs(nowPos_old - nowPos) > 10) {
    Serial.print("nowPos:");
    Serial.println(nowPos);
    nowPos_old = nowPos;
  }
  if (settingSign == LOW) {
    if (openSign == LOW) {
      Serial.println("setting OpenPos");
      OPEN_POS = nowPos;
    } else if (closedSign == LOW) {
      Serial.println("setting closedPos");
      CLOSED_POS = nowPos;
    }
    MAX_RANGE = abs(OPEN_POS - CLOSED_POS) + 100;
    delay(1000);
    return;
  }

  if (openSign == LOW) {
    targetPos = OPEN_POS;
  } else if (closedSign == LOW) {
    targetPos = CLOSED_POS;
  } else {
    return;
  }

  Serial.print("targetPos:");
  Serial.println(targetPos);

  int targetRange = abs(targetPos - nowPos);

  if (targetRange > MAX_POS / 2) {
    targetRange = MAX_POS - targetRange;
  }

  Serial.print("targetRange:");
  Serial.println(targetRange);

  if (targetRange < 10) {
    stopMotor();
  } else if ((targetPos > nowPos) && (targetRange < MAX_RANGE)) {
    Serial.println("targetPos > nowPos");
    startSolenoid();
    digitalWrite(MOTOR_NORMAL, HIGH);
    while (abs(targetPos - nowPos) >= 10) {
      getPositionMeter();
      getMotorFault();
    }
    stopMotor();
  } else if ((targetPos < nowPos) && (targetRange < MAX_RANGE)) {
    Serial.println("targetPos < nowPos");
    startSolenoid();
    digitalWrite(MOTOR_REVERSE, HIGH);
    while (abs(targetPos - nowPos) >= 10) {
      getPositionMeter();
      getMotorFault();
    }
    stopMotor();
  } else {
    Serial.println("Max Range Error");
  }
}

void getKeySign() {
  openSign = digitalRead(OPEN_SIGN);
  closedSign = digitalRead(CLOSED_SIGN);
  settingSign = digitalRead(SETTING_BTN);
}

void getPositionMeter() {
  nowPos = analogRead(POSITIONMETER);
}

void stopMotor() {
  digitalWrite(MOTOR_NORMAL, LOW);
  digitalWrite(MOTOR_REVERSE, LOW);
  digitalWrite(SOLENOID_NORMAL, LOW);
  digitalWrite(SOLENOID_REVERSE, LOW);
  endSolenoid();
}

void startSolenoid() {
  digitalWrite(SOLENOID_NORMAL, HIGH);
  digitalWrite(SOLENOID_REVERSE, LOW);
  delay(200);
  digitalWrite(SOLENOID_NORMAL, LOW);
  digitalWrite(SOLENOID_REVERSE, LOW);
}

void endSolenoid() {
  startSolenoid();
  delay(200);
  digitalWrite(SOLENOID_NORMAL, LOW);
  digitalWrite(SOLENOID_REVERSE, HIGH);
  delay(200);
  digitalWrite(SOLENOID_NORMAL, LOW);
  digitalWrite(SOLENOID_REVERSE, LOW);
}

void getMotorFault() {
  if (digitalRead(SOLENOID_FAULT) == LOW) {
    Serial.println("Solenoid Error");
  }
  if (digitalRead(MOTOR_FAULT) == LOW) {
    Serial.println("motor Error");
  }
}
