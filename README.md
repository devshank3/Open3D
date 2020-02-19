# Open3D Library with CUDA support amended for Jetson platform. (extented for JetScan : RGBD 3D scanner )

Link for JetScan overall setup :

https://github.com/devshank3/JetScan


                                                             
                                                       JetScan
<p align="center">
<img src="https://raw.githubusercontent.com/devshank3/JETSCAN/master/Scanned_result/IMG_20200117_132752.jpg" width="280" /><img src="https://raw.githubusercontent.com/devshank3/JETSCAN/master/Scanned_result/IMG_20200117_133003.jpg" width="280" /><img src="https://raw.githubusercontent.com/devshank3/JETSCAN/master/Scanned_result/IMG_20200117_133019.jpg" width="280" />
</p>

<p align="center">
<img src="https://raw.githubusercontent.com/devshank3/JETSCAN/master/Scanned_result/IMG_20200117_132915.jpg" width="280" /><img src="https://raw.githubusercontent.com/devshank3/JETSCAN/master/Scanned_result/IMG_20200117_132835.jpg" width="280" /><img src="https://raw.githubusercontent.com/devshank3/JETSCAN/master/Scanned_result/IMG_20200117_132846.jpg" width="280" />
</p>




**Credits** :

All credits to 

[Wei Dong](https://github.com/theNded/Open3D) and his efforts for making initial CUDA support  

https://github.com/theNded/Open3D

Team [Open3D](https://github.com/intel-isl/Open3D)  

**This repo is only for software stack**

## Usage 

After following **https://github.com/devshank3/JetScan/blob/master/Software_O3D/Readme.md** for Software Setup 

* Step 1 : Open **Open3D-for-Jetson/examples/Python/ReconstructionSystem/**

* Step 2 : Copy or Cut GUI2.py to the build **Open3D-for-Jetson/build/bin/examples/**

* Step 3 :   Open **Open3D-for-Jetson/examples/Cuda/ReconstructionSystem/config/intel/test.json**

**Assign the preset path for **

* Path data
* Camera intrinsic data

**"path_dataset": " Python dataset path of D435i capture",**  
This directry with your system path "Open3D-for-Jetson/examples/Python/ReconstructionSystem/dataset/realsense/"  

**"path_intrinsic": " intel D435i intrinsics either from CUDA config file or Captured dateset intrinsic file",**
 Usually "Open3D-for-Jetson/examples/Python/ReconstructionSystem/dataset/realsense/camera_intrinsic.json"
 
 * Step 4: Run the GUI
 
**In the respective directive path run the GUI.py files**

Open3D-for-Jetson/examples/Python/ReconstructionSystem/

> $ python3 GUI.py

Open3D-for-Jetson/build/bin/examples/

> $ python3 GUI2.py

Two Graphical user interfaces pops up:

* Step 5

**Recording**
In **GUI**
click on **Recorder** 

recording of **RGB** and it alligned **Depth** starts 
you can see the no of frames being captured in the terminal 

**Insruction while capturing **

* For Small environment captiure less than 1000 frames
* For detailed capture make sure less blackish area in the capture frame 
* make sure of backlits (lights / CFL / LED etc ) while capturing scenes
* Do not shake the module much 
* Can capture both Indoor and Outdoor as D435i supports both 

-----

* Max distance of capture of depth can be adjusted by 
This snippet in 

Open3D-for-Jetson/examples/Python/ReconstructionSystem/realsense_recorder.py

```
# We will not display the background of objects more than
#  clipping_distance_in_meters meters away
clipping_distance_in_meters = 3 # 3 meter
clipping_distance = clipping_distance_in_meters / depth_scale
```
mode setting snippet

```
class Preset(IntEnum):
Custom = 0
Default = 1
Hand = 2
HighAccuracy = 3
HighDensity = 4
MediumDensity = 5
.
.
.
.
.
.
.
.
.
# Using preset HighAccuracy for recording
if args.record_rosbag or args.record_imgs:
depth_sensor.set_option(rs.option.visual_preset, Preset.HighAccuracy)
```

* Step  6

After capturing the RGBD sequence 

**IN GUI2**
Click on **Fragment Construction**

In the terminal you can look for completion and time taken 

* Step 7

after Fragment / submap generation 

**IN GUI**
Click on **3D construct**

In the terminal you can look for completion and time taken 

* Step 8

after 3D scene final integration 

**IN GUI**
Click on **View**

**Your high resolution 3D model is ready in  point cloud format for view !!!!**



### For changes made look into 

my_changes.md





