import sys
import os
import socket

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect(('127.0.0.1', 1234))

""" NOT WORKING WITH VS2017 and Maya 2019 in H470
# ask Maya for its PID :) 
s.send(b'getpid\n')
answer = s.recv(1024)
answer = answer[0:answer.find(b"\n")]

# attach maya process (dialog pops up)
os.system("vsjitdebugger -p " + str(answer))
print("K")
# new file
s.send(b"file -f -new\n")
answer = s.recv(1024)
"""

# C:/Users/liyey/Desktop/mayaPlugin/MayaMLL

# load plugin
s.send(b'loadPlugin("C:/Users/liyey/Desktop/mayaPlugin/MayaMLL/x64/Debug/MayaAPI.mll")\n')

# close socket with Maya
s.close()