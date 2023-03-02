import processing.serial.*;
import g4p_controls.*;

Serial connexion;;


public void setup(){
   String portName = Serial.list()[0];
  connexion = new Serial(this, portName, 9600);
  size(1800, 900, JAVA2D);
  createGUI();
  customGUI();
  // Place your setup code here
  
}

public void draw(){
   while (connexion.available() > 0)
  {
  String msg = connexion.readString();
  if (msg != null)
  {
  textarea3.appendText("\n"+ msg);
  }
  }
  background(230);
  
}

// Use this method to add additional statements
// to customise the GUI controls
public void customGUI(){

}
