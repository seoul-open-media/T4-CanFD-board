/*

  https://seoul-open-media.slack.com/archives/C0747PEVCHJ/p1724459026978029

*/

int pushButton1 = 23; // L
int pushButton2 = 29; //R

void setup() {
 
  Serial.begin(9600);

  pinMode(pushButton1, INPUT);
  pinMode(pushButton2, INPUT);
}


void loop() {
  // read the input pin:
  int buttonState1 = digitalRead(pushButton1);
  int buttonState2 = digitalRead(pushButton2);
  // print out the state of the button:
  Serial.print(buttonState1);  Serial.print(", "); Serial.println(buttonState2);
  delay(100);   
}
