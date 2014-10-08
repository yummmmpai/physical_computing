import gab.opencv.*;
import processing.video.*;
import processing.serial.*;
import java.awt.*;

Capture video;
OpenCV opencv;
Serial xbee;  // The serial port
PImage img;
int timeline; 
String content = "";
int number; 

void setup() {
  size(640, 480);
  video = new Capture(this, 640/2, 480/2);
  opencv = new OpenCV(this, 640/2, 480/2);
  opencv.loadCascade(OpenCV.CASCADE_FRONTALFACE);
  println(Serial.list());
  //xbee = new Serial(this, Serial.list()[5], 9600); 
  
  timeline = 1;
  
  video.start();
}

void draw() {
  scale(2);
  //timeline = xbee.read(); 
  opencv.loadImage(video);

  image(video, 0, 0 );
  
  switch (timeline) {
    case 1:  
      img = loadImage("/Users/spai55/Documents/BRANDCENTER_DOCS/Physical_Computing/Dice/darkesttimeline.png");
      break;
        
    case 4: 
      img = loadImage("/Users/spai55/Documents/BRANDCENTER_DOCS/Physical_Computing/Dice/fourtimeline.png");
      break;
    }

  Rectangle[] faces = opencv.detect();
  //println(faces.length);
  println("timeline: " + timeline); 
  
  switch(timeline) {
    case 1: 
      for (int i = 0; i < faces.length; i++) {
        println(faces[i].x + "," + faces[i].y);
        image(img, faces[i].x, faces[i].y + (faces[i].height - 38), faces[i].width, faces[i].height);
      }
      break;
      
    case 4: 
      for (int i = 0; i < faces.length; i++) {
        println(faces[i].x + "," + faces[i].y);
        scale(2);
        image(img, faces[i].x - (.6   * faces[i].width), faces[i].y - (.5 * faces[i].height), faces[i].width, faces[i].height);
      }
      break; 
  }

}

void captureEvent(Capture c) {
  c.read();
}

