import sys
import os
import tkinter
import tkinter.messagebox
root=tkinter.Tk()
root.geometry('550x460')
root.title('3d')
root.configure(background='#993d00')


def AA():
    os.system('python3 realsense_recorder.py --record_imgs')
def BB():
    os.system('python3 run_system.py config/realsense.json --register --refine --integrate')
def CC():
    os.system('python3 view.py ')

A=tkinter.Button(root,text=" Recorder ",font=('Glacial Indifference',20),fg='white',width=30,bg="#ff8533",relief='flat',command= AA)
A.place(x = 25, y = 1*80, width=500, height=50)


B=tkinter.Button(root,text=" 3D construct ",font=('Glacial Indifference',20),fg='white',width=30,bg="#ff8533",relief='flat',command= BB)
B.place(x = 25, y = 2*80, width=500, height=50)

C=tkinter.Button(root,text="View ",font=('Glacial Indifference',20),fg='white',width=30,bg="#ff8533",relief='flat',command= CC)
C.place(x = 25, y = 3*80, width=500, height=50)


root.mainloop()
