import processing.serial.*;                           // import libraries
import KinectPV2.KJoint;
import KinectPV2.*;

Serial myPort;                                        // create instance of Serial with which we will talk to the HC-12 Passthrough board.
KinectPV2 kinect;                                     // create instance of KinectPV2 to handle the Kinect tracking

int areaX = 180;                                      // define the area of the trackable field
int areaY = 180;

PVector[] machines = new PVector[7];                  // create 7 machines as an array of PVectors

public int kinectyPos;                                // create public variables for the tracking data we get from the kinect
public int kinectxPos;


void setup(){
        size(1024,600);                               // set size to 1024x600 as this is the resolution of the handheld screen used to operate this sketch
        //fullScreen();                               // sketch is set to fullScreen when running on handheld display
        frameRate(20);                                // limit framerate to 20 to limit the frequency of draw() calls in tern limiting frequency of Serial calls to prevent overloading of the Arduino

        ellipseMode(CENTER);                          // draw ellipses from the center to reduce confusion down the line

        kinect = new KinectPV2(this);                 // initialise instance of KinectPV2
        kinect.enableColorImg(true);                  // enable the full spectrum camera in the Kinect
        kinect.enableSkeleton3DMap(true);             // enable Kinect skeleton tracking
        kinect.init();                                // initialise the Kinect with these parameters

        String portName = Serial.list()[0];           // open a serial port to the Arduino using the first open COM channel (only one connected)

        machines[0] = new PVector(0,0);               // set positions of each machine in the trackable field
        machines[1] = new PVector(30,-20);
        machines[2] = new PVector(60,-30);
        machines[3] = new PVector(90,-40);
        machines[4] = new PVector(120,-30);
        machines[5] = new PVector(150,-20);
        machines[6] = new PVector(180,0);

        for(int i = 0; i<Serial.list().length; i++) {           // loop through available COM ports
                println(Serial.list()[i]);                      // print each to the console (for debugging)
        }
        myPort = new Serial(this, portName, 115200);            // initialise the Serial port with a baud(data) rate of 115200 bits per second
}

void draw(){

        background(0);                                          // set background to black to clear screen each frame
        image(kinect.getColorImage(), 0, 0, 640, 368);          // display color image from kinec tot the left of the screen

        ArrayList<KSkeleton> skeletonArray =  kinect.getSkeleton3d();       // get the array of skeletons for this frame

        for (int i = 0; i < skeletonArray.size(); i++) {                                          // iterate through every skeleton in the skeleton array
                KSkeleton skeleton = (KSkeleton) skeletonArray.get(skeletonArray.size()-1);       // get the skeleton of the most recent person to enter the visual field

                if (skeleton.isTracked()) {                                                       // while the skeleton is tracked
                        KJoint[] joints = skeleton.getJoints();                                   // get array of joints from the skeleton
                        kinectxPos = constrain(int(map(joints[KinectPV2.JointType_SpineBase].getX(),-1.55,1.55,0,180)),0,180)-20;             // calculate a position for the base of the user's spine in the 2D trackable field
                        kinectyPos = constrain(int(map(joints[KinectPV2.JointType_SpineBase].getZ(),1.85,3,0,180)),0,180);

                        if(map(kinectyPos,0,areaX,0,180)<180) {              // if the y position of the user is less than 180, then they are being tracked

                                // the write command here sends two values to the Arduino in a manner that allows for error correction later
                                // the command sends the position in the form "xxx-yyy,." this means that even if the number is 1 (001) or 125 (125) three digits are always sent
                                // the Arduino checks for three digits when it recieves the values, if there are not 3 values the message is discarded
                                myPort.write(nf(int(map(kinectxPos,0,180,0,180)),3)+"-"+nf(int(map(kinectyPos,0,180,0,180)),3)+"," +".");
                                println(nf(int(map(kinectxPos,0,180,0,180)),3)+"-"+nf(int(map(kinectyPos,0,180,0,180)),3)+"," +".");          // the sent data is also printed to the console for debugging
                                pushMatrix();                                       // push the matrix so that we can draw a visual representation of the tracked space to the right of the live image
                                translate(700,50);                                  // the start point for drawing is moved 700 pixels right and 50 down, so it is beside the getColorImage
                                fill(255,0,0);                                      // set the fill colour of drawn elements to red
                                ellipse(kinectxPos,kinectyPos,5,5);                 // draw the position of the trakced user in the space

                                for(int j = 0; j<machines.length; j++) {            // loop through the array of machines so we can display them
                                        strokeWeight(80);                           // set some parameters for the drawn elements
                                        stroke(255);
                                        pushMatrix();                               // push the matrix again so we can translate to each position individually
                                        translate(machines[j].x,machines[j].y);     // translate to the position of the machine selected on this iteration
                                        // rotate the cursor so that the number of the machine appears to rotate as the head of the real machine should , to face the user
                                        rotate(atan2(map(kinectyPos,0,180,0,180) - map(machines[j].y,0,180,0,180), map(kinectxPos,0,180,0,180) - map(machines[j].x,0,180,0,180)));
                                        textSize(32);                               // set the size the text of the machine number will be drawing
                                        text(j,-10,10);                             // draw the text
                                        popMatrix();                                // pop the matrix for this individual machine
                                }
                                popMatrix();                                        // pop the matrix for the broader representation
                        }
                }
        }
}
