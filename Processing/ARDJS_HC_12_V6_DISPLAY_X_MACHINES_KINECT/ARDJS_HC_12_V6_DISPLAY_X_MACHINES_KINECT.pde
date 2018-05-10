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
        myPort = new Serial(this, portName, 115200);
}

void draw(){

        background(0);
        image(kinect.getColorImage(), 0, 0, 640, 368);

        ArrayList<KSkeleton> skeletonArray =  kinect.getSkeleton3d();

        //individual JOINTS
        for (int i = 0; i < skeletonArray.size(); i++) {
                KSkeleton skeleton = (KSkeleton) skeletonArray.get(skeletonArray.size()-1);
                if (skeleton.isTracked()) {
                        KJoint[] joints = skeleton.getJoints();
                        kinectxPos = constrain(int(map(joints[KinectPV2.JointType_SpineBase].getX(),-1.55,1.55,0,180)),0,180)-20;
                        kinectyPos = constrain(int(map(joints[KinectPV2.JointType_SpineBase].getZ(),1.85,3,0,180)),0,180);
                        fill(255,0,0);
                        if(map(kinectyPos,0,areaX,0,180)<180) {

                                myPort.write(nf(int(map(kinectxPos,0,180,0,180)),3)+"-"+nf(int(map(kinectyPos,0,180,0,180)),3)+"," +".");
                                println(nf(int(map(kinectxPos,0,180,0,180)),3)+"-"+nf(int(map(kinectyPos,0,180,0,180)),3)+"," +".");
                                pushMatrix();
                                translate(700,50);
                                ellipse(kinectxPos,kinectyPos,5,5);

                                for(int j = 0; j<machines.length; j++) {
                                        strokeWeight(80);
                                        stroke(255);
                                        pushMatrix();
                                        translate(machines[j].x,machines[j].y);
                                        rotate(atan2(map(kinectyPos,0,180,0,180) - map(machines[j].y,0,180,0,180), map(kinectxPos,0,180,0,180) - map(machines[j].x,0,180,0,180)));
                                        textSize(32);
                                        text(j,-10,10);
                                        popMatrix();
                                }
                                popMatrix();
                        }
                }
        }
}
