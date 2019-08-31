const int OPEN_SIGN = 2;
const int CLOSED_SIGN = 3;
const int SOLENOID_NORMAL = 4;
const int SOLENOID_REVERSE = 5;
const int MOTOR_NORMAL = 6;
const int MOTOR_REVERSE = 7;
const int POSITIONMETER = 0;

boolean openSign;
boolean closedSign;

int nowPos;
int targetPos;

const int MAX_POS = 800;
const int OPEN_POS = 0;
const int CLOSED_POS = 350;
const int MAX_RANGE = abs(OPEN_POS - CLOSED_POS) + 100;

void setup() {
  // put your setup code here, to run once:
  pinMode(OPEN_SIGN, INPUT_PULLUP);
  pinMode(CLOSED_SIGN, INPUT_PULLUP);
  pinMode(MOTOR_NORMAL, OUTPUT);
  pinMode(MOTOR_REVERSE, OUTPUT);
  pinMode(SOLENOID_NORMAL, OUTPUT);
  pinMode(SOLENOID_REVERSE, OUTPUT);
  pinMode(POSITIONMETER, INPUT);
  Serial.begin(9600);

}

void loop() {
  // put your main code here, to run repeatedly:
  getKeySign();
  
  getPositionMeter();
  Serial.print("nowPos:");
  Serial.println(nowPos);

  
  
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
  
  digitalWrite(MOTOR_NORMAL, LOW);    
  digitalWrite(MOTOR_REVERSE, LOW);

  if (targetRange < 10) {
    stopMotor();
  } else if ((targetPos > nowPos) && (targetRange < MAX_RANGE)) {
    digitalWrite(MOTOR_NORMAL, HIGH);
    while (abs(targetPos - nowPos) >= 10) {
      getPositionMeter();
    }
    stopMotor();
    Serial.println("targetPos > nowPos");
  } else if ((targetPos < nowPos) && (targetRange < MAX_RANGE)) {
    digitalWrite(MOTOR_REVERSE, HIGH);
    while (abs(targetPos - nowPos) >= 10) {
      getPositionMeter();
    }
    stopMotor();
    
    
      Serial.println("targetPos < nowPos");
  } else {
    Serial.println("ERROR");
  }

  
}

void getKeySign() {
  openSign = digitalRead(OPEN_SIGN);
  closedSign = digitalRead(CLOSED_SIGN);
}

void getPositionMeter() {
  nowPos = analogRead(POSITIONMETER);
}

void stopMotor() {
  digitalWrite(MOTOR_NORMAL, LOW);
  digitalWrite(MOTOR_REVERSE, LOW);
  Serial.println("target < 10");
}
