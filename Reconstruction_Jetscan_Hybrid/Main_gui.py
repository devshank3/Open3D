#
# Jetscan Main GUI 
#

import sys
import os
import tkinter
import tkinter.messagebox
from tkinter import *

root=tkinter.Tk()
root.geometry('800x480')
root.title('Jetscan')
root.configure(background='#660000')

canvas = Canvas(root, width = 350, height = 140)
canvas.pack()
img = PhotoImage(file="jetscan_logo.ppm")      
canvas.create_image(0,0, anchor=NW, image=img)

def AA():
    os.system('python3 realsense_D435i_recorder.py')

def BB():
    os.system('/home/shank/Projects/3D_processing/jetscan/jetscan-final-edit/Open3D-for-Jetson/build/bin/examples/./Run_MakeFragments')
    os.system('python3 run_regref.py config/intel/config.json --register --refine')
    os.system('/home/shank/Projects/3D_processing/jetscan/jetscan-final-edit/Open3D-for-Jetson/build/bin/examples/./Run_IntegrateScene')

def CC():
    os.system('/home/shank/Projects/3D_processing/jetscan/jetscan-final-edit/Open3D-for-Jetson/build/bin/examples/./ViewPoseGraph')
'''
def DD():
    os.system('/home/shank/Projects/3D_processing/jetscan/jetscan-final-edit/Open3D-for-Jetson/build/bin/examples/./Run_IntegrateScene')
'''
A=tkinter.Button(root,text="Capture RGB-D",font=('Glacial Indifference',20),fg='black',width=30,bg="#ff8533",relief='flat',command= AA)
A.place(x = 30, y = 1.5*105, width=350, height=50)

B=tkinter.Button(root,text=" Reconstruct Scene",font=('Glacial Indifference',20),fg='black',width=30,bg="#ff8533",relief='flat',command= BB)
B.place(x = 30, y = 2.5*94, width=350, height=50)

C=tkinter.Button(root,text=" View with Posegraph",font=('Glacial Indifference',20),fg='black',width=30,bg="#ff8533",relief='flat',command= CC)
C.place(x = 30, y = 3.5*90, width=350, height=50)
'''
D=tkinter.Button(root,text="Hybrid full Run",font=('Glacial Indifference',20),fg='white',width=30,bg="#ff8533",relief='flat',command= DD)
D.place(x = 30, y = 4*80, width=500, height=50)
'''

root.mainloop()