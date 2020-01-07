int randNumber;

void setup() {
  Serial.begin(115200);
  randomSeed(analogRead(0));
   
}

void loop() {
  // put your main code here, to run repeatedly:
  randNumber = random(10, 100);// print a random number from 10 to 100

  Serial.println(randNumber);
   
  delay(60000);
  
}
