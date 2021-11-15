import numpy as np
import cv2 as cv
import imagezmq
import argparse
import socket
import time
import zmq
from time import sleep

def main():
    #print("Scanning")
    #sender = imagezmq.ImageSender(connect_to='tcp://DESKTOP-QRV1JJ8:50489')
    sender = imagezmq.ImageSender(connect_to='tcp://192.168.20.35:5555')
    #imagehub = imagezmq.ImageHub()
    counting=0
    label="placeholder"
    #Connecting to Camera
    Camera = cv.VideoCapture(0)
    Camera.release()
    Camera = cv.VideoCapture(0)
    recordnow = 0
    #Check Connection
    if not Camera.isOpened():
        #print("Cannot open camera")
        exit()
    datafrompc = ''
    while counting==0:
        # Capture frame-by-frame
        ret, VideoFeed = Camera.read()
        VideoFeed = cv.rotate(VideoFeed,cv.ROTATE_180)
        VideoFeed2 = cv.resize(VideoFeed,(400,225))
        #cv.imshow("test",VideoFeed)
        cv.imwrite("test.jpg",VideoFeed2)
        counting+=1
        sender.send_image("test",VideoFeed2)
        #ssleep(2)
        outputfile = open("/home/pi/MyFile.txt","r")
        datafrompc = outputfile.readlines()
        outputfile.close() 
        templabel = str(datafrompc).split(",")
        labelname = str(templabel[0]).replace("'","")
        labelname = labelname.replace("[","")
        
        if labelname == "WhiteArrow":
            label = "1"
        if labelname == "RedArrow":
            label = "2"
        if labelname == "GreenArrow":
            label = "3"
        if labelname == "BlueArrow":
            label = "4"
        if labelname == "YellowCircle":
            label = "5"
        if labelname == "BlueOne":
            label = "6"
        if labelname == "GreenTwo":
            label = "7"
        if labelname == "RedThree":
            label = "8"
        if labelname == "WhiteFour":
            label = "9"
        if labelname == "YellowFive":
            label = "10"
        if labelname == "BlueSix":
            label = "11"
        if labelname == "GreenSeven":
            label = "12"
        if labelname == "RedEight":
            label = "13"
        if labelname == "WhiteNine":
            label = "14"
        if labelname == "RedA":
            label = "15"
        if labelname == "GreenB":
            label = "16"
        if labelname == "WhiteC":
            label = "17"
        if labelname == "BlueD":
            label = "18"
        if labelname == "YellowE":
            label = "19"
        if labelname == "RedF":
            label = "20"
        if labelname == "GreenG":
            label = "21"
        if labelname == "WhiteH":
            label = "22"
        if labelname == "BlueS":
            label = "23"
        if labelname == "YellowT":
            label = "24"
        if labelname == "RedU":
            label = "25"
        if labelname == "GreenV":
            label = "26"
        if labelname == "WhiteW":
            label = "27"
        if labelname == "BlueX":
            label = "28"
        if labelname == "YellowY":
            label = "29"
        if labelname == "RedZ":
            label = "30"
        if labelname == "Target":
            label = "31"       
        
        #print("Label is "+labelname +"Which is "+label)
        
        
        if recordnow == 1:
            Recorder.write(VideoFeed)
        #actions for User's inpute
        Pressedkey =cv.waitKey(1)&0xFF
        if  Pressedkey== ord('q'):
            print("quiting")
            break
        if Pressedkey==ord('r'):
            #cv.putText(VideoFeed,text='Recording Now',org=(200,100),fontFace=cv.FONT_HERSHEY_SIMPLEX,fontScale=2,color=(200,200,0),thickness=2,lineType=cv.LINE_AA)	
            print("Recording Now")
            Recorder = cv.VideoWriter("/home/pi/MDPImageRecognition/OutputVideo/testing.avi",cv.VideoWriter_fourcc(*'MJPG'),30,(int(Camera.get(3)),int(Camera.get(4))))
            recordnow = 1
        if Pressedkey== ord('s'):
            print("Recording Stopped")
            recordnow = 0
            Recorder.release()

    # When everything done, release the capture
    Camera.release()
    cv.destroyAllWindows()
    return label
if __name__ == '__main__':
    label = main()
