// Program: Arduino Graph
// Author: Michael Rouse
// Date: 4/2015
// Contact: michael@michaelrouse.net

import processing.serial.*;


Serial arduino;
String x = "";
String y = "";
int xPos = 60;
float oldX = 56;
float oldY = height;
boolean graphing = true;

void setup()
{
   size(700, 600);
   frame.setTitle("Rocket Flight Data");
   
   println(Serial.list());
   
   while (Serial.list().length - 1 < 0)
   {
     println("No COM Devices");
     delay(200);
   }
   println("COM device found!");
   arduino = new Serial(this, Serial.list()[0], 9600);
   
   arduino.bufferUntil('\n');
   
   background(255);
   textSize(32);
}

void draw()
{
    //background(255);
   line(50, (height-50), width, (height-50));
   line(50, 0, 50, (height-50)); 
  stroke(126);
   //if (x!= null && y!=null){
    //text(x, 10, 130);
    //fill(0, 102, 153);
    //text(y, 10, 80);
   // fill(0, 102, 153);
   
}

void serialEvent(Serial arduino)
{
  y = arduino.readStringUntil('\n');
 
  
  if (y != null && graphing)
  {
      y = trim(y);
      
      float inY = float(y);

      inY = map(inY, 0, 20, 00, height - 50);
      
      stroke(127,34,255);
      line(oldX, (height - oldY) - 60, xPos, (height - inY)-60);
       
      oldX = xPos;
      oldY = inY;
      xPos+= 5;
      
      if(xPos > width)
      {
        graphing = false;
      }
     
  }
}


