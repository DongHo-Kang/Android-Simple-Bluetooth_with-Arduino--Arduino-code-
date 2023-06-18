#include <BluetoothSerial.h>

BluetoothSerial SerialBT;

const int ledPinA = 5; //Led를 제어하기 위한 핀 번호
const int buttonPin1 = 4; //버튼을 연결한 핀 번호
const int blinkCount = 5; //깜박이는 횟수 설정


bool ledAPowered = false; //LED 상태를 저장하기 위한 변수
unsigned long previousTime = 0; //이전에 난수를 전송한 시간을 저장하는 변수
bool ledBlinking = false; //LED 깜빡임 동작 상태 변수

void setup(){
  SerialBT.begin("DongHo ESP32");//Bluetooth 이름 설정 
  pinMode(ledPinA, OUTPUT);
  pinMode(buttonPin1, INPUT_PULLUP); //풀업 저항을 사용하여 버튼 입력 설정
  Serial.begin(115200); //시리얼 통신 초기화
}

void loop(){
  if(SerialBT.available()){ //수신 데이터 확인
    char command = SerialBT.read(); //명령어 읽기

    switch(command){
      case '1':
        digitalWrite(ledPinA, HIGH); //led 켜기
        Serial.println("LED가 켜짐"); //시리얼 모니터에 출력
        break;

        case '3':
        digitalWrite(ledPinA, LOW); //led 끄기
        Serial.println("LED끄기");
        break;

      case '2':
        if(!ledBlinking){ //LED가 깜빡이는 동작 중이 아닌 경우에만 시작
          ledBlinking = true;
          while(ledBlinking){
            digitalWrite(ledPinA, HIGH);
            delay(1000);
            digitalWrite(ledPinA, LOW);
            delay(1000);
            Serial.println("LED가 깜빡이는 중입니다.");
            if(SerialBT.available()){ //다른 command 도착 시 루프 종료
              char nextCommand = SerialBT.read();
              if(nextCommand != '2'){ //다음 명령어가 2가 아니면 루프 종료
                ledBlinking = false;
                break;
              }
            }            
          }
        }
        break;
      
      case '4':
        if(ledBlinking){
          ledBlinking = false;
          digitalWrite(ledPinA, LOW);
          Serial.println("LED가 꺼졌습니다.");
        }
        break;
      }
    }
    //버튼 상태 확인
    if(digitalRead(buttonPin1) == LOW) {
      //버튼이 눌렸을 때 LEDPinA 상태 변경
      ledAPowered = !ledAPowered;

      //LEDPinA 상태에 따라 LED 켜기/끄기
      digitalWrite(ledPinA, ledAPowered ? HIGH : LOW);

      //버튼 클릭 시 메시지 출력
      if(ledAPowered){
        SerialBT.print(102);
        Serial.println("LED 켜짐");
      }else{
        SerialBT.print(103);
        Serial.println("LED 꺼짐");
      }
      delay(200); // 버튼 피싱을 방지하기 위한 딜레이
    }
    //10초마다 난수 생성 및 전송
    unsigned long currentTime = millis();
    if(currentTime - previousTime >= 10000){
      int randomNumber = random(101); // 0부터 100사이의 정수 난수 생성
      SerialBT.println(randomNumber); //난수를 전송

      Serial.println(randomNumber); //난수가 시리얼 모니터에 표시 
      previousTime = currentTime;
    }
  
}