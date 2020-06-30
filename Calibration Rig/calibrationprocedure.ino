// Use with PLX DAQ V2.11. Install file, open PLX spreadsheet and then connect through the DAQ interface. 

#include <Servo.h>

Servo pitch;
int sampleSeconds = 5; // number of seconds for single sample to be averaged
int frequency = 100; // frequency of sample (Hz)
int arraySize = sampleSeconds*frequency;

int x = 0;
int y = 0;
int centre = 56; //degree at which probe is horizontal in pitch
byte directionPin = 9;
byte stepPin = 8;
int numberOfSteps = 29; //number of steps in a 5 degree arc
byte ledPin = 13;
int pulseWidthMicros = 20;  // microseconds
int millisbetweenSteps = 50; //lower number = faster steps 

void setup() {
  analogReference(EXTERNAL); //sets arduino for 3.3V analog logic provided 3.3V is supplied to AREF pin
  pitch.attach(3);
  Serial.begin(9600);
  pinMode(A8, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);
  pinMode(directionPin, OUTPUT);
  pinMode(stepPin, OUTPUT);
  delay(1000);
  pitch.write(56);
  delay(100);

  Serial.println("Initialising:");
    for(int j = 0; j < 50; j++){
      initialise();
    }
  stepCCWsetup(); // sets stepper to first reading angle ************************************************************
  Serial.println("Initialised");
  Serial.println("CLEARSHEET"); // clears data for PLXDAQ
  Serial.println("LABEL,Yaw angle,pich angle,Yellow(middle),Blue(top),Purple(right),Grey(left)"); //Sets labels for PLXDAQ
  //we were losing 2 rows of data in the spreadsheet to the PLXDAQ button so this spoofs 2 rows of data.
  Serial.print("DATA,");
  Serial.println(y - 40);
  Serial.print("DATA,");
  Serial.println(y - 40);
  }

void loop() {
  Serial.println("checkpoint");
    for(int k = 0; k < 19; k++){
      y = (5*k)-45; //y here is only for denoting angle in data labels.
      pitch.write(56-45);
      delay(1000);
      Serial.print("Yaw angle = ");
      Serial.println(y - 90);
      for(int i = 0; i < 19; i++){
        x = (56-45)+5*i; // x here is used for writing servo angle as well as data labels
        Serial.print("Pitch angle = ");
        Serial.println(x+56);
        pitch.write(x);
        Serial.println("sensing");
        sense();
      }
      Serial.println("DATA"); // prints an empty line for ease of viewing
      stepCW();
    }
    stepCCWsetup(); // resets stepper to starting yaw position
    nothing(); // end of the road, does what it says
}

void sense(){ // values to store averages that will be written to spreadsheet
  int val1AVG = 0;
  int val2AVG = 0;
  int val3AVG = 0;
  int val4AVG = 0;
  long sum1 = 0;
  long sum2 = 0;
  long sum3 = 0;
  long sum4 = 0;
  
  int sensVal1[sampleSeconds*frequency]; // arrays to store sample sets before being averaged.
  int sensVal2[sampleSeconds*frequency];
  int sensVal3[sampleSeconds*frequency];
  int sensVal4[sampleSeconds*frequency];
  for(int l = 0; l < (sampleSeconds*frequency); l++){
  sensVal1[l] = analogRead(A8); //taking 10 data points and storing them in arrays
  sensVal2[l] = analogRead(A1);
  sensVal3[l] = analogRead(A2);
  sensVal4[l] = analogRead(A3);
  /*Serial.print("Val = ");
  Serial.println(val1AVG);
  Serial.print("DATA,");
  Serial.print(y);
  Serial.print(",");
  Serial.print(x - 56);
  Serial.print(",");
  Serial.print(sensVal1[l]);
  Serial.print(",");
  Serial.print(sensVal2[l]);
  Serial.print(",");
  Serial.print(sensVal3[l]);
  Serial.print(",");
  Serial.println(sensVal4[l]);*/ //This outputs raw live data for testing
  delay(1000/frequency);
  }

  for(int m = 0; m < (sampleSeconds*frequency); m++){
    sum1 += sensVal1[m];
    sum2 += sensVal2[m];
    sum3 += sensVal3[m];
    sum4 += sensVal4[m];
  }
  val1AVG = sum1/arraySize;
  val2AVG = sum2/arraySize;
  val3AVG = sum3/arraySize;
  val4AVG = sum4/arraySize;
  //serial printing the averaged data to PLXDAQ
  Serial.print("Val = ");
  Serial.println(val1AVG);
  Serial.print("DATA,");
  Serial.print(y);
  Serial.print(",");
  Serial.print(x - 56);
  Serial.print(",");
  Serial.print(val1AVG);
  Serial.print(",");
  Serial.print(val2AVG);
  Serial.print(",");
  Serial.print(val3AVG);
  Serial.print(",");
  Serial.println(val4AVG);
  delay(100);
}

void initialise(){
  pitch.write(35);
  delay(100);
    int val1 = 0;
    int val2 = 0;
    int val3 = 0;
    int val4 = 0;
  val1 = analogRead(A5);
  val2 = analogRead(A1);
  val3 = analogRead(A2);
  val4 = analogRead(A3);
  Serial.print("Val1 = ");
  Serial.print(val1);
  Serial.print("  Val2 = ");
  Serial.print(val2);
  Serial.print("  Val3 = ");
  Serial.print(val3);
  Serial.print("  Val4 = ");
  Serial.println(val4);
  delay(100);
}

void nothing(){
  Serial.println("Finished");
  while(1);
}

void stepCW(){
  digitalWrite(directionPin, HIGH);
  for(int n = 0; n < numberOfSteps; n++) {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(pulseWidthMicros); // this line is probably unnecessary
    digitalWrite(stepPin, LOW);
    
    delay(millisbetweenSteps);
  }
}

void stepCCW(){
  digitalWrite(directionPin, LOW);
  for(int n = 0; n < numberOfSteps; n++) {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(pulseWidthMicros); // this line is probably unnecessary
    digitalWrite(stepPin, LOW);
    
    delay(millisbetweenSteps);
   }
}

void stepCCWsetup(){
  digitalWrite(directionPin, LOW);
  delay(200);
  for(int n = 0; n < numberOfSteps*9; n++) {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(pulseWidthMicros); // this line is probably unnecessary
    digitalWrite(stepPin, LOW);
    delay(millisbetweenSteps);
  }
}
