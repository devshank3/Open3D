import sys
import os
import tkinter
import tkinter.messagebox
root=tkinter.Tk()
root.geometry('550x460')
root.title('Jetscan')
root.configure(background='#660000')

def AA():
    os.system('/home/shank/Projects/3D_processing/jetscan/jetscan-final-edit/Open3D-for-Jetson/build/bin/examples/./RunSystem')

def BB():
    os.system('/home/shank/Projects/3D_processing/jetscan/jetscan-final-edit/Open3D-for-Jetson/build/bin/examples/./Run_MakeFragments')

def CC():
    os.system('/home/shank/Projects/3D_processing/jetscan/jetscan-final-edit/Open3D-for-Jetson/build/bin/examples/./Run_IntegrateScene')

A=tkinter.Button(root,text="Run full System ",font=('Glacial Indifference',20),fg='white',width=30,bg="#ff8533",relief='flat',command= AA)
A.place(x = 25, y = 1*80, width=500, height=50)

B=tkinter.Button(root,text=" Only Submap construction",font=('Glacial Indifference',20),fg='white',width=30,bg="#ff8533",relief='flat',command= BB)
B.place(x = 25, y = 2*80, width=500, height=50)

C=tkinter.Button(root,text="Only Integrate the whole scene",font=('Glacial Indifference',20),fg='white',width=30,bg="#ff8533",relief='flat',command= CC)
C.place(x = 25, y = 3*80, width=500, height=50)

root.mainloop()