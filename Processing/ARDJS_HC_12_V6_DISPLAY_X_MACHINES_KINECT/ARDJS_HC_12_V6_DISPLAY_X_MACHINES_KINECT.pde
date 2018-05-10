import processing.serial.*;

import KinectPV2.KJoint;
import KinectPV2.*;

Serial myPort;
KinectPV2 kinect;

// Machine ID 0 - X/Y

//int MAC0Y = 20;
//int MAC0X = 20;

int areaX = 180;
int areaY = 180;

PVector[] machines = new PVector[7];

public int kinectyPos;
public int kinectxPos;


void setup(){
        //size(1024,600);
        fullScreen();
        frameRate(20);

        ellipseMode(CENTER);

        kinect = new KinectPV2(this);

        kinect.enableColorImg(true);

        //enable 3d  with (x,y,z) position
        kinect.enableSkeleton3DMap(true);

        kinect.init();

        //noLoop();
        String portName = Serial.list()[0];

        machines[0] = new PVector(0,0);
        machines[1] = new PVector(30,-20);
        machines[2] = new PVector(60,-30);
        machines[3] = new PVector(90,-40);
        machines[4] = new PVector(120,-30);
        machines[5] = new PVector(150,-20);
        machines[6] = new PVector(180,0);

        for(int i = 0; i<Serial.list().length; i++) {
                println(Serial.list()[i]);
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
                        //println(xPos,yPos,mouseX,mouseY);
                        fill(255,0,0);
                        //noStroke();
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
                        //X and Z are only relevant properties.
                        //minX = min(joints[KinectPV2.JointType_SpineBase].getX(),minX);
                        //maxX = max(joints[KinectPV2.JointType_SpineBase].getX(),maxX);
                        //minZ = min(joints[KinectPV2.JointType_SpineBase].getZ(),minZ);
                        //maxZ = max(joints[KinectPV2.JointType_SpineBase].getZ(),maxZ);
                        //println(minX,minZ,maxX,maxZ);
                }
        }

        //if(mouseX>5 && mouseX <170){
        //  myPort.write(nf(int(map(mouseX,0,width,0,180)),3)+"-"+nf(int(map(mouseY,0,height,0,180)),3)+"," +".");
        //  println(nf(int(map(mouseX,0,width,0,180)),3)+"-"+nf(int(map(mouseY,0,height,0,180)),3)+"," +".");
        //}

        //if(mouseX>5 && mouseX<170 && mouseY>0 && mouseY<height){

        //  for(int i = 0; i<machines.length;i++){
        //    strokeWeight(80);
        //    stroke(255);
        //    pushMatrix();
        //      translate(machines[i].x,machines[i].y);
        //      //scale(0.1);
        //      rotate(atan2(map(mouseY,0,180,0,180) - map(machines[i].y,0,180,0,180), map(mouseX,0,180,0,180) - map(machines[i].x,0,180,0,180)));
        //      textSize(32);
        //      text(i,-10,10);
        //      //triangle(-10,-10,20,-10,0,10);
        //    popMatrix();
        //  }
        //}
}
