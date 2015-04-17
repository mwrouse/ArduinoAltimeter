import processing.serial.*;


Serial arduino;
String x = "";
String y = "";
int xPos = 100;
float oldX = 100;
float oldY = height;
boolean graphing = true;

int MODIFIER = 100;

int MAX_ALT = 1524; // Max altitde in meters of the graph

void setup()
{
   size(1000, 900);
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
    textSize(36);
    //background(255);
   line(MODIFIER, (height-MODIFIER), width, (height-MODIFIER));
   line(MODIFIER, 0, MODIFIER, (height-MODIFIER)); 
    stroke(126);

    text("Time (s)", (width/2)-50, height-10);
    fill(0, 102, 153);
    textSize(36);
    
    text ("A", 10, (height/2) - 50);
    text ("L", 10, (height/2) - 20);
    text ("T", 10, (height/2) + 10);
    fill(0, 102, 153);
   
   
   textSize(15); 
   
   // Label the altitude axis
   float value = MAX_ALT / 8.0;
   float y;
   int mody;
   for (int i = 0; i < 8; i++)
   {
     
     y = map(value * i, 0, MAX_ALT, 0, height - MODIFIER );
     line(MODIFIER - 10, y, MODIFIER, y);
     

     if (i ==0)
     {
       text(MAX_ALT, 45, y + 20);
     }else{
       
       if ((value * (8-i)) < 1000.0)
         mody = 55;
        else
          mody = 45;
          
       text(int(value * (8-i)), mody, y + 7);
     }
   }
    
   
   // Label the time axis
   
   for (int i = 10; i <= 100; i+=10)
   {
     
     float xPos2 = map(i, 0, 100, 0, width - MODIFIER );
     line (MODIFIER + xPos2, (height - MODIFIER), MODIFIER + xPos2, (height - MODIFIER) + 10);
     
     text(i, xPos2 + MODIFIER - 7, (height - MODIFIER) + 25);
   }
   
   // Label 0
   line(MODIFIER - 10, (height - MODIFIER), MODIFIER, (height - MODIFIER));
   line(MODIFIER, (height - MODIFIER), MODIFIER, (height - MODIFIER) + 10);
   text("0", MODIFIER - 15, (height - MODIFIER) + 15);
   
   
   // Fill in all lines and text
   fill(0, 102, 153);
   stroke(126);
   
   
}

void serialEvent(Serial arduino)
{
  y = arduino.readStringUntil('\n');
 
  float value = map(0.25, 0, 100, 0, width - MODIFIER);
  
  if (y != null && graphing)
  {
      y = trim(y);
      
      float inY = float(y);
      
      inY = map(inY, 0, MAX_ALT, 0, height - MODIFIER);
      
      stroke(127,34,255);
      line(oldX, (height - oldY) - MODIFIER - 10, xPos, (height - inY)-MODIFIER - 10);
       
      oldX = xPos;
      oldY = inY;
      xPos+= 5;
      
      if(xPos > width)
      {
        graphing = false;
      }
     
  }
}


