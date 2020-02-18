import sys
import os
import tkinter
import tkinter.messagebox
sys.path.append("build/bin/examples/")
root=tkinter.Tk()
root.geometry('550x460')
root.title('3D ')
root.configure(background='#660000')



def BB():
    os.system('./RunSystem ')
def CC():
    os.system('python3 gpuGraph.py')




B=tkinter.Button(root,text=" Fragment construction",font=('Glacial Indifference',20),fg='white',width=30,bg="#ff3333",relief='flat',command= BB)
B.place(x = 25, y = 2*80, width=500, height=50)

C=tkinter.Button(root,text="GPU usage",font=('Glacial Indifference',20),fg='white',width=30,bg="#ff3333",relief='flat',command= CC)
C.place(x = 25, y = 3*80, width=500, height=50)

root.mainloop()
