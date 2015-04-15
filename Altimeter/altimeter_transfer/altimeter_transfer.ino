bool displaying = false;
bool displayed = false;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

}

void loop() {
  String ser = Serial.readStringUntil(',');
  
  if (ser.equals("a"))
  {
    if (displaying)
     {
       displaying = false;
       displayed = true;
     }else{
       displaying = true;
     }
  }else{
    if (displaying && !displayed)
    {
      Serial.println(ser);
    }
  }
   
 
}
