# Robotics-Assessment-1
Course work for robotics module.
Contained withtin this repo is namely two versions of my project one as a zip file and one as a un compressed version I wont get in to exactly why i did this but it has to do with my laptop

## GUI
### To run the GUI you need two things
* Processing 4.2
* The G4P libraries

The GUI can then be run by using Processing.

## Ardunio Robot
### To run the Arduino code you need to:
* The Zumo32u4 libaries
* The Correct board selected to get the motors working to get the correct board available the following process must be followed

  *  Open the File menu and select “Preferences”.

  * Find the “Additional Boards Manager URLs” text box

  *  Copy and paste this into that text box https://files.pololu.com/arduino/package_pololu_index.json

See image blow:

![image](https://user-images.githubusercontent.com/38881444/222428585-534ab998-adec-4864-b38b-ec0e05ad86aa.png)

To run the code you must first close the GUI if it is connected to the robot and then upload the code from the ardunio IDE to the robot when this process finishes you can run the GUI and start moving the robot.

Note when the robot starts it will calibrate the line sensor this process can be identified by the Yellow LED being on when the robot is turned on and when it finishes it should turn off you can re calibrate it by typing the key press 'C'

## Acknowledgements
The GUI was built using the G4P GUI builder

A lot of my understandiing of using the Zumo32u4 robot came from code examples from pololus example projects these can be found here https://github.com/pololu/zumo-32u4-arduino-library/tree/master/examples

BlinkLEDs taught me how to use LEDS to give user information on what state the robot is in.

LineFollower & MazeSolver helped me navigate the building the robot would be put in

FaceTowardsOpponent code from this example was used to figure out how to identify objects using the proximity sensors 

