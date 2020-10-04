# Open3D with CUDA support amended for Jetson platform (extented for JetScan)

**forked from:**

https://github.com/theNded/Open3D

Link for JetScan overall setup :

https://github.com/devshank3/JetScan

***
### **Credits** :


[Wei Dong](https://github.com/theNded/Open3D) and his efforts for making initial CUDA support  

[Repo Link](https://github.com/theNded/Open3D) & [Paper](http://dongwei.info/publications/open3d-gpu.pdf)

Team [Open3D](https://github.com/intel-isl/Open3D)  
***
### **This repo constitutes the software stack for JetScan**

Follow **https://github.com/devshank3/JetScan/blob/master/Software_O3D/Readme.md** for Software Setup

### Usage 

* Step 1 : Go to **Open3D-for-Jetson/Reconstruction_Jetscan_Hybrid/**

* Step 2 : Edit **Main_gui.py**

    set your system executable path to Run_MakeFragments,ViewPoseGraph,Run_IntegrateScene

    eg.

       os.system('/home/shank/Projects/3D_processing/jetscan/jetscan-final-edit/Open3D-for-Jetson/build/bin/examples/./Run_MakeFragments')

                                                              ⇩⇩⇩ to ⇩⇩⇩                       

       os.system('your_system _path/Open3D-for-Jetson/build/bin/examples/./Run_MakeFragments')

 * Step 3: Run the UI (in  virtual env if setup)

   > $ python3 Main_gui.py

* Step 4 : Start the Reconstruction

     **Capture RGB-D**

     Captures the RGB-D sequence

     **Reconstruct Scene**

     The hybrid reconstruction system starts with the RGB-D sequence recorded

     **View**

     View the latest reconstructed 3D Model




