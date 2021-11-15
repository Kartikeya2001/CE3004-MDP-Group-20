import serial
import os
from time import sleep
from ImageRegconitionConnection import *
import VideoReader
import VideoReader_latest_Colour_Cleaned_LowLighting_ROI
#import LiveFeed
import signal
#import subprocess
import sys
import threading
import time
import multiprocessing
import cv2 as cv
port = serial.Serial("/dev/ttyUSB0",baudrate=115200,timeout=0.1)
sys.settrace
#port = serial.Serial("/dev/ttyrfcomm0",baudrate=115200,timeout=0.1)
Android = serial.Serial("/dev/rfcomm0",baudrate=115200,timeout=0.1)

allObCount = []
allX= []
allY = []
allDir = []
index = 0
label = "placeholder"
RobotPlaced = False
Android.write(bytes(str("TARGET,"+str(1)+","+str(1)),'utf-8'))
Android.write(bytes(str("TARGET,"+str(2)+","+str(2)),'utf-8'))
Android.write(bytes(str("TARGET,"+str(3)+","+str(3)),'utf-8'))
Android.write(bytes(str("TARGET,"+str(4)+","+str(4)),'utf-8'))
Android.write(bytes(str("TARGET,"+str(5)+","+str(5)),'utf-8'))
#Camera = cv.VideoCapture(0)
#Camera.release()
#Camera = cv.VideoCapture(0)
#if not Camera.isOpened():
#        print("Cannot open camera")
#recordnow = 1
#LiveFeed.main()
def videorecord():
    #ret, VideoFeed = Camera.read()
    #VideoFeed = cv.rotate(VideoFeed,cv.ROTATE_180)
    VideoReader_latest_Colour_Cleaned_LowLighting_ROI.main()
    #main()
    #Pressedkey =cv.waitKey(1)&0xFF
    #if  Pressedkey== ord('q'):
    #    print("quiting")
    #    Camera.release()
    #    cv.destroyAllWindows()
    #if Pressedkey==ord('r'):
        #cv.putText(VideoFeed,text='Recording Now',org=(200,100),fontFace=cv.FONT_HERSHEY_SIMPLEX,fontScale=2,color=(200,200,0),thickness=2,lineType=cv.LINE_AA)	
    #    print("Recording Now")
    #    Recorder = cv.VideoWriter("/home/pi/MDPImageRecognition/OutputVideo/testrun.avi",cv.VideoWriter_fourcc(*'MJPG'),30,(int(Camera.get(3)),int(Camera.get(4))))
    #    recordnow = 1
    #if Pressedkey== ord('s'):
    #    print("Recording Stopped")
    #    recordnow = 0
    #    Recorder.release()
    #Recorder = cv.VideoWriter("/home/pi/MDPImageRecognition/OutputVideo/testrun.avi",cv.VideoWriter_fourcc(*'MJPG'),30,(int(Camera.get(3)),int(Camera.get(4))))
    #cv.imshow("test",VideoFeed)
    #VideoFeed = cv.rotate(VideoFeed,cv.ROTATE_180)
    #cv.imshow("test",VideoFeed)
    #Recorder.write(VideoFeed)
    #if recordnow == 1:
            #Recorder.write(VideoFeed)

while True:
    #For Video Recording
    #t = multiprocessing.Process(target = videorecord)
    #t.start()
    
    AndroidPort = Android.read(99)
    AndroidMsg = str(AndroidPort)
    RobotPort = port.read(99)
    RobotMsg =str(RobotPort)
        
    #print(len(AndroidMsg))
    if len(AndroidMsg)==4 and not("begin" in AndroidMsg and RobotPlaced):
        print(str(AndroidMsg))
        if str(AndroidMsg) != "b's'":
            port.write(AndroidPort)
        else:
            print("in")
            templabel = main()
            label = main()
            print(label)
            #sleep(1)
            #label = "20"
            #VideoReader.main()
            #ObID = movement         
            Android.write(bytes(str(label),'utf-8')) 
        
    
    if len(AndroidMsg) > 4:
        if "Coordinates" in AndroidMsg:
            RobotPlaced = True
        
        if "Obstacle" in AndroidMsg and not("Removed" in AndroidMsg):
            MsgSplited = AndroidMsg.split(":")
            ObCount = str(MsgSplited[0][-1])
            Coordinate = MsgSplited[1].split(",")
            if len(Coordinate) == 3:
                x = str(Coordinate[0]).replace("(","")
                y2 = str(Coordinate[1]).replace(")","")
                y = str(y2).replace("'","")
                direction = str(Coordinate[2]).replace("'","")
                allObCount.append(ObCount.strip())
                print("Obj No.: "+ ObCount)
                allX.append(x.strip())
                print("X Co-ordinates: "+ x)
                allY.append(y.strip())
                print("Y Co-ordinates: "+y)
                if direction.strip() == "N":
                    allDir.append("1")
                    print("Direction: "+direction + ", Num: 1")
                    Android.write(bytes("Successfully placed",'utf-8'))
                elif direction.strip() == "E":
                    allDir.append("2")
                    print("Direction: "+direction + ", Num: 2")
                    Android.write(bytes("Successfully placed",'utf-8'))
                elif direction.strip() == "S":
                    allDir.append("3")
                    print("Direction: "+direction + ", Num: 3")
                    Android.write(bytes("Successfully placed",'utf-8'))
                elif direction.strip() == "W":
                    allDir.append("4")
                    print("Direction: "+direction + ", Num: 4")
                    Android.write(bytes("Successfully placed",'utf-8'))
                    
        if "beginExplore" in AndroidMsg and RobotPlaced:
            inputfile = open("/home/pi/Desktop/MDP-Group-20-master/Algorithms/input.txt","w+")
            inputfile.write(str(len(allObCount))+"\n")
            for x in allX:
                if index != len(allX)-1:
                    inputfile.write(allX[index] + " " + allY[index] + " " +allDir[index]+"\n")
                else:
                    inputfile.write(allX[index] + " " + allY[index] + " " +allDir[index])
                index+=1
            inputfile.close()
            print("finished writing input, " + str(len(allObCount))+" Objects")
            index = 0
            os.chdir("/home/pi/Desktop/MDP-Group-20-master/Algorithms")
            print("Executing Algo")
            os.system('./algoOnly <input.txt> output.txt')
            #pid = os.getpid()
            #pid = int(subprocess.check_output(['pgrep','-f','output.txt']).strip())
            #print(pid)
            #os.kill(pid,signal.SIGTERM)
            print("Bestpath found!")
            outputfile = open("/home/pi/Desktop/MDP-Group-20-master/Algorithms/output.txt","r")
            bestpath = outputfile.readlines()
            outputfile.close()
            bestpath = bestpath[0]
            xpos = 1
            ypos = 1
            rDir = "N"
            trace = 0
            command = len(bestpath)
            countforshow = 0
            for movement in bestpath:
                print(movement)
                if movement != "s" and not(movement.isdigit()) and movement.isalnum():
                    #print(movement)
                    RobotPort2=''
                    port.write(bytes(str(movement),'utf-8'))
                    print("Writtn: " + str(movement))
                    while RobotPort2 != bytes(str(movement),'utf-8'):
                        RobotPort2 = port.read(1)
                        #print(RobotPort)
                    #print("A: "+str(RobotPort))
                    #print("Movm is :" + str(movement))
                    #if str(RobotPort) == ("b'"+str(movement)+"'"):    
                    #print("current Movement: " +str(movement))
                    if movement == "r":   
                        if rDir == "N":
                            Android.write(bytes(str("ROBOT,"+str(xpos)+","+str(ypos)+","+"E"),'utf-8'))
                            rDir = "E"
                        elif rDir == "E":
                            Android.write(bytes(str("ROBOT,"+str(xpos)+","+str(ypos)+","+"S"),'utf-8'))
                            rDir = "S"
                        elif rDir == "S":
                            Android.write(bytes(str("ROBOT,"+str(xpos)+","+str(ypos)+","+"W"),'utf-8'))
                            rDir = "W"
                        elif rDir == "W":
                            Android.write(bytes(str("ROBOT,"+str(xpos)+","+str(ypos)+","+"N"),'utf-8'))
                            rDir = "N"
                    if movement == "l":
                        if rDir == "N":
                            Android.write(bytes(str("ROBOT,"+str(xpos)+","+str(ypos)+","+"W"),'utf-8'))
                            rDir = "W"
                        elif rDir == "E":
                            Android.write(bytes(str("ROBOT,"+str(xpos)+","+str(ypos)+","+"N"),'utf-8'))
                            rDir = "N"
                        elif rDir == "S":
                            Android.write(bytes(str("ROBOT,"+str(xpos)+","+str(ypos)+","+"E"),'utf-8'))
                            rDir = "E"
                        elif rDir == "W":
                            Android.write(bytes(str("ROBOT,"+str(xpos)+","+str(ypos)+","+"S"),'utf-8'))
                            rDir = "S"
                    if movement == "f":
                        if rDir == "N":
                            ypos+=1
                            Android.write(bytes(str("ROBOT,"+str(xpos)+","+str(ypos)+","+str(rDir)),'utf-8'))
                        elif rDir == "E":
                            xpos+=1
                            Android.write(bytes(str("ROBOT,"+str(xpos)+","+str(ypos)+","+str(rDir)),'utf-8'))
                        elif rDir == "S":
                            ypos-=1
                            Android.write(bytes(str("ROBOT,"+str(xpos)+","+str(ypos)+","+str(rDir)),'utf-8'))
                        elif rDir == "W":
                            xpos-=1
                            Android.write(bytes(str("ROBOT,"+str(xpos)+","+str(ypos)+","+str(rDir)),'utf-8'))
                    if movement == "b":
                        if rDir == "N":
                            ypos-=1
                            Android.write(bytes(str("ROBOT,"+str(xpos)+","+str(ypos)+","+str(rDir)),'utf-8'))
                        elif rDir == "E":
                            xpos-=1
                            Android.write(bytes(str("ROBOT,"+str(xpos)+","+str(ypos)+","+str(rDir)),'utf-8'))
                        elif rDir == "S":
                            ypos+=1
                            Android.write(bytes(str("ROBOT,"+str(xpos)+","+str(ypos)+","+str(rDir)),'utf-8'))
                        elif rDir == "W":
                            xpos+=1
                            Android.write(bytes(str("ROBOT,"+str(xpos)+","+str(ypos)+","+str(rDir)),'utf-8')) 
                else:
                    #print("in")
                    if movement.isdigit():
                        #LiveFeed.main()
                        templabel = main()
                        label = main()
                        #sleep(3)
                        #label = "20"              
                        ObID = movement         
                        Android.write(bytes(str("TARGET,"+str(ObID)+","+str(label)),'utf-8'))
        print("End of Path")
        if "beginFastest" in AndroidMsg:
            print("Sending q")
            port.write(bytes("q",'utf-8'))
            print("Sent q")
        if "SS|" in AndroidMsg:
            print("Sending Q")
            port.write(bytes("Q",'utf-8'))
            print("Sent Q")
        
        
    #writethis = input("write something: ")
    #ROBOT,3,12,N
    #Target,3,12
    
    #port.write(AndroidRx)
    #rcv = port.read(1)
    #print("Robots Says: ", rcv)
    #port.write(bytes("\r\nReceived: " + repr(rcv),'ascii'))
