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

int oldPos = 0;
int nowPos;
int targetPos;
int openMarginPos;
int closedMarginPos;


const int MAX_POS = 1023;
const int MARGIN_RANGE = 100;
//int maxRange = 450;
int openPos = 0;
int closedPos = 400;
//int middlePos;



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

  Serial.begin(9600);
  stopMotor();
}

void loop() {
  // put your main code here, to run repeatedly:
  //信号取得
  getKeySign();
  //ポジション取得（更新）
  getPositionMeter();

  //  middlePos = (closedPos + openPos) / 2;


  //前のポジションと比較して10以上ずれてたら、前のポジションを変更
  if (abs(oldPos - nowPos) > 10) {
    //Serial.print("nowPos:");
    //Serial.println(nowPos);
    oldPos = nowPos;
  }
  //オープンポジションとクローズポジションを設定
  /* if (settingSign == LOW) {
     if (openSign == LOW) {
       Serial.println("setting OpenPos");
       openPos = nowPos;
     } else if (closedSign == LOW) {
       Serial.println("setting closedPos");
       closedPos = nowPos;
     }
     //鍵が動ける最大範囲を設定
     if ( abs( openPos - closedPos) < MAX_POS / 2) {
       maxRange = abs( openPos - closedPos) + MARGIN_RANGE;
     } else {
       maxRange = (MAX_POS - abs(openPos - closedPos)) + MARGIN_RANGE;
     }





     delay(1000);
     return;
    }*/
  //目的ポジションを設定
  if (openSign == LOW) {
    targetPos =  openPos;
    } else if (closedSign == LOW) {
    targetPos = closedPos;
    } else {
    return;
    }
    /*Serial.print("nowPos");
    Serial.println(nowPos);
    Serial.print("targetPos:");
    Serial.println(targetPos);*/


  boolean openMarginPosOver = 0;
  boolean closedMarginPosOver = 0;

  //nowPosがエラーポジションにいたらreturn
  if (openPos < closedPos) {
    openMarginPosOver = fillInOpenDifference(openPos, -(MARGIN_RANGE / 2));
    closedMarginPosOver = fillInClosedDifference(closedPos, MARGIN_RANGE / 2);
    Serial.print("closedMarginPos");
        Serial.println(closedMarginPos);
        Serial.print("openMarginPos");
        Serial.println(openMarginPos);
        Serial.print("nowPos");
        Serial.println(nowPos);
    if ( openMarginPosOver || closedMarginPosOver ) {
      if ((nowPos > closedMarginPos) && (nowPos < openMarginPos)) {
        Serial.println("ERROR1");
        Serial.print("closedMarginPos");
        Serial.println(closedMarginPos);
        Serial.print("openMarginPos");
        Serial.println(openMarginPos);
        Serial.print("nowPos");
        Serial.println(nowPos);
        return;
      }
    } else {
      if ((nowPos > closedMarginPos) || (nowPos < openMarginPos)) {
        Serial.println("ERROR2");
        Serial.print("closedMarginPos");
        Serial.println(closedMarginPos);
        Serial.print("openMarginPos");
        Serial.println(openMarginPos);
        Serial.print("nowPos");
        Serial.println(nowPos);
        return;
      }
    }
  } else {
    closedMarginPosOver = fillInClosedDifference(closedPos, -(MARGIN_RANGE / 2));
    openMarginPosOver = fillInOpenDifference(openPos, MARGIN_RANGE / 2);
    if ( openMarginPosOver || closedMarginPosOver ) {
      if ((nowPos > openMarginPos) || (closedMarginPos > nowPos)) {
        Serial.println("ERROR3");
        Serial.print("closedMarginPos");
        Serial.println(closedMarginPos);
        Serial.print("openMarginPos");
        Serial.println(openMarginPos);
        Serial.print("nowPos");
        Serial.println(nowPos);
        return;
      }
    } else {
      if ((nowPos < openMarginPos) && (closedMarginPos < nowPos)) {
        Serial.println("ERROR4");
        Serial.print("closedMarginPos");
        Serial.println(closedMarginPos);
        Serial.print("openMarginPos");
        Serial.println(openMarginPos);
        Serial.print("nowPos");
        Serial.println(nowPos);
        return;
      }
    }
  }
  

  if (abs(targetPos - nowPos) < 10) {
    stopMotor();
    Serial.println("1");
  }

if (nowPos < targetPos) {
  startSolenoid();
  digitalWrite(MOTOR_NORMAL, HIGH);
  while (abs(targetPos - nowPos) >= 10) {
    getPositionMeter();
    getMotorFault();
  }
  stopMotor();
  
} else {
  startSolenoid();
  digitalWrite(MOTOR_REVERSE, HIGH);
  while (abs(targetPos - nowPos) >= 10) {
    getPositionMeter();
    getMotorFault();
  }
  stopMotor();
}
  
}



//なぜmiddlePosを使ったのだろうか、、、
/*  if (nowPos < middlePos) {
    if (targetPos == closedPos) {

      startSolenoid();
      digitalWrite(MOTOR_REVERSE, HIGH);
      while (abs(targetPos - nowPos) >= 10) {
        getPositionMeter();
        getMotorFault();
      }
      stopMotor();
      Serial.println("2");
    } else if (targetPos == openPos) {
      startSolenoid();
      digitalWrite(MOTOR_NORMAL, HIGH);
      while (abs(targetPos - nowPos) >= 10) {
        getPositionMeter();
        getMotorFault();
      }
      stopMotor();
      Serial.println("3");
    }
  } else if (nowPos >= middlePos) {
    if (targetPos == closedPos) {

      startSolenoid();
      digitalWrite(MOTOR_NORMAL, HIGH);
      while (abs(targetPos - nowPos) >= 10) {
        getPositionMeter();
        getMotorFault();
      }
      stopMotor();
      Serial.println("4");
    } else if (targetPos == openPos) {
      startSolenoid();
      digitalWrite(MOTOR_REVERSE, HIGH);
      while (abs(targetPos - nowPos) >= 10) {
        getPositionMeter();
        getMotorFault();
      }
      stopMotor();
      Serial.println("5");
    }
  }
  }*/

/*  if (openPos < closedPos) {
    openMarginPos = fillInDifference(openPos, -(MARGIN_POS / 2));
    closedMarginPos = fillInDifference(closedPos, MAGIN_POS / 2);
    if (closedMarginPos > openMarginPos) {
      if ((closedMarginPos < nowPos) || (nowPos < openMarginPos)) {
        Serial.println("ERROR 1");
        return;
      }
    } else {
      if ((closedPos < nowPos) && (nowPos < openMarginPos)) {
        Serial.println("ERROR 2");
        return;
      }
    }
  } else {
    openMarginPos = fillInDifference(openPos, MARGIN_POS / 2);
    closedMarginPos = fillInDifference(closedPos, -(MAGIN_POS / 2));
    if (openMarginPos > closedMarginPos) {
      if ((openMarginPos < nowPos) || (nowPos < closedMarginPos)) {
          Serial.println("ERROR 3");
          return;
      }
    } else {
      if (((openMarginPos - MAX_POS) < nowPos) && (nowPos < closedMarginPos)) {
        Serial.println("ERROR 4");
        return;
      }
    }
  }*/
/*
  //動く距離が10より小さかったらモーターを止める
  if (abs(targetPos - nowPos) < 10) {
    stopMotor();

    //目的ポジションより現在ポジションのほうが小さく、かつ移動距離が開閉距離より小さかった場合
  } else if ((targetPos > nowPos) && (abs(targetPos - nowPos) < maxRange)) {
    Serial.print("nowPos:");
    Serial.println(nowPos);
    Serial.print("maxRange");
    Serial.println(maxRange);
    startSolenoid();
    digitalWrite(MOTOR_REVERSE, HIGH);
    while (abs(targetPos - nowPos) >= 10) {
      getPositionMeter();
      getMotorFault();
    }
    stopMotor();
    Serial.print("nowPos:");
    Serial.println(nowPos);
    Serial.println();
    //目的ポジションより現在ポジションのほうが大きく、かつ移動距離が開閉距離より小さかった場合
  } else if ((targetPos < nowPos) && (abs(targetPos - nowPos) < maxRange)) {
    Serial.println("targetPos < nowPos");
    Serial.print("targetPos:");
    Serial.println(targetPos);
    Serial.print("nowPos:");
    Serial.println(nowPos);
    Serial.print("targetPos - nowPos");
    Serial.print("maxRange");
    Serial.println(maxRange);
    startSolenoid();
    digitalWrite(MOTOR_NORMAL, HIGH);
    while (abs(targetPos - nowPos) >= 10) {
      getPositionMeter();
      getMotorFault();
    }
    stopMotor();
    Serial.print("nowPos:");
    Serial.println(nowPos);
    Serial.println();
  } else {
    Serial.println("Max Range Error");
  }
  }*/


void getKeySign() {
  openSign = digitalRead(OPEN_SIGN);
  closedSign = digitalRead(CLOSED_SIGN);
  //settingSign = digitalRead(SETTING_BTN);
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

int fillInOpenDifference(int openPos, int margin) {
  if ((openPos + margin) < 0) {
    openMarginPos = MAX_POS + (openPos + margin);
    return 1;
  } else if ((openPos + margin) > MAX_POS) {
    openMarginPos = (openPos + margin) - MAX_POS;
    return 1;
  } else {
    openMarginPos = openPos + margin;
    return 0;
  }
}

int fillInClosedDifference(int closedPos, int margin) {
  if ((closedPos + margin) < 0) {
    closedMarginPos = MAX_POS + (closedPos + margin);
    return 1;
  } else if ((closedPos + margin) > MAX_POS) {
    closedMarginPos = (closedPos + margin) - MAX_POS;
    return 1;
  } else {
    closedMarginPos = closedPos + margin;
    return 0;
  }
}
