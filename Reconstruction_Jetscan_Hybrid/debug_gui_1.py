#
# Jetscan Debug GUI 1
#

import sys
import os
import tkinter
import tkinter.messagebox
root=tkinter.Tk()
root.geometry('800x480')
root.title('Jetscan Debug 1')
root.configure(background='#660000')

def AA():
    os.system('python3 realsense_D435i_recorder.py')

def BB():
    os.system('/home/shank/Projects/3D_processing/jetscan/jetscan-final-edit/Open3D-for-Jetson/build/bin/examples/./Run_MakeFragments')

def CC():
    os.system('python3 run_regref.py config/intel/config_debug1.json --register --refine')

def DD():
    os.system('/home/shank/Projects/3D_processing/jetscan/jetscan-final-edit/Open3D-for-Jetson/build/bin/examples/./Run_IntegrateScene /home/shank/Projects/3D_processing/jetscan/jetscan-final-edit/Open3D-for-Jetson/examples/Cuda/ReconstructionSystem/config/stanford/lounge.json')

def EE():
    os.system('/home/shank/Projects/3D_processing/jetscan/jetscan-final-edit/Open3D-for-Jetson/build/bin/examples/./RunSystem /home/shank/Projects/3D_processing/jetscan/jetscan-final-edit/Open3D-for-Jetson/examples/Cuda/ReconstructionSystem/config/stanford/lounge.json')

def FF():
    os.system('/home/shank/Projects/3D_processing/jetscan/jetscan-final-edit/Open3D-for-Jetson/build/bin/examples/./ViewPoseGraph /home/shank/Projects/3D_processing/jetscan/jetscan-final-edit/Open3D-for-Jetson/examples/Cuda/ReconstructionSystem/config/stanford/lounge.json')


A=tkinter.Button(root,text="Capture RGBD",font=('Glacial Indifference',20),fg='black',width=30,bg="#ff8533",relief='flat',command= AA)
A.place(x = 30, y = 1*70, width=600, height=50)

B=tkinter.Button(root,text="Make Fragments CUDA",font=('Glacial Indifference',20),fg='black',width=30,bg="#ff8533",relief='flat',command= BB)
B.place(x = 30, y = 2*70, width=600, height=50)

C=tkinter.Button(root,text="Registration Refine (ColorICP, RANSAC)",font=('Glacial Indifference',20),fg='black',width=30,bg="#ff8533",relief='flat',command= CC)
C.place(x = 30, y = 3*70, width=600, height=50)

D=tkinter.Button(root,text="Integrate CUDA",font=('Glacial Indifference',20),fg='black',width=30,bg="#ff8533",relief='flat',command= DD)
D.place(x = 30, y = 4*70, width=600, height=50)

E=tkinter.Button(root,text="RunSystem CUDA All",font=('Glacial Indifference',20),fg='black',width=30,bg="#ff8533",relief='flat',command= EE)
E.place(x = 30, y = 5*70, width=600, height=50)

F=tkinter.Button(root,text="View  PG",font=('Glacial Indifference',18),fg='black',width=30,bg="#ff8533",relief='flat',command= FF)
F.place(x = 640, y = 1*70, width=150, height=150)

G=tkinter.Button(root,text="View ",font=('Glacial Indifference',18),fg='black',width=30,bg="#ff8533",relief='flat',command= FF)
G.place(x = 640, y = 3.5*70, width=150, height=150)

root.mainloop()