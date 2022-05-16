# DeepStream-Yolo - DeepSORT
NVIDIA DeepStream SDK 6.0.1  YOLO models with Tracker Integration. 

This repo focuses on the Tracking itself, for more information on the DeepStream YOLO plugin please refer to [Marcos Luciano DeepStream Yolo Repo](https://github.com/marcoslucianops/DeepStream-Yolo/)

There you can find benchmarks and extra tutorial and Info.

<img src="assets/deepstream-gif.gif" width="600"/>


### Getting started

* [Requirements](#requirements)
* [Tested models](#tested-models)
* [Benchmarks](#benchmarks)
* [dGPU installation](#dgpu-installation)
* [Basic usage](#basic-usage)
* [YOLOv5 usage](#yolov5-usage)
* [YOLOR usage](#yolor-usage)
* [INT8 calibration](#int8-calibration)
* [Using your custom model](docs/customModels.md)
* [Implement Trackers (like DeepSort)](#implement-tracking)

##

### Requirements

#### x86 platform

* [Ubuntu 18.04](https://releases.ubuntu.com/18.04.6/)
* [CUDA 11.4](https://developer.nvidia.com/cuda-toolkit)
* [TensorRT 8.0 GA (8.0.1)](https://developer.nvidia.com/tensorrt)
* [cuDNN >= 8.2](https://developer.nvidia.com/cudnn)
* [NVIDIA Driver >= 470.63.01](https://www.nvidia.com.br/Download/index.aspx)
* [NVIDIA DeepStream SDK 6.0.1 (6.0)](https://developer.nvidia.com/deepstream-sdk)
* [DeepStream-Yolo](https://github.com/marcoslucianops/DeepStream-Yolo)

#### Jetson platform

* [JetPack 4.6.1](https://developer.nvidia.com/embedded/jetpack)
* [NVIDIA DeepStream SDK 6.0.1 (6.0)](https://developer.nvidia.com/deepstream-sdk)
* [DeepStream-Yolo](https://github.com/marcoslucianops/DeepStream-Yolo)

### For YOLOv5 and YOLOR

#### x86 platform

* [PyTorch >= 1.7.0](https://pytorch.org/get-started/locally/)

#### Jetson platform

* [PyTorch >= 1.7.0](https://forums.developer.nvidia.com/t/pytorch-for-jetson-version-1-10-now-available/72048)

##

### Tested models

* [Darknet YOLO](https://github.com/AlexeyAB/darknet)
* [YOLOv5 4.0, 5.0, 6.0 and 6.1](https://github.com/ultralytics/yolov5)
* [YOLOR](https://github.com/WongKinYiu/yolor)
* [MobileNet-YOLO](https://github.com/dog-qiuqiu/MobileNet-Yolo)
* [YOLO-Fastest](https://github.com/dog-qiuqiu/Yolo-Fastest)

##

### Benchmarks

```
nms-iou-threshold = 0.6
pre-cluster-threshold = 0.001 (mAP eval) / 0.25 (FPS measurement)
batch-size = 1
valid = val2017 (COCO) - 1000 random images for INT8 calibration
sample = 1920x1080 video
NOTE: Used maintain-aspect-ratio=1 in config_infer file for YOLOv4 (with letter_box=1), YOLOv5 and YOLOR models.
```

##

### dGPU installation

To install the DeepStream on dGPU (x86 platform), without docker, we need to do some steps to prepare the computer.

<details><summary>Open</summary>

#### 1. Disable Secure Boot in BIOS

<details><summary>If you are using a laptop with newer Intel/AMD processors, please update the kernel to newer version.</summary>

```
wget https://kernel.ubuntu.com/~kernel-ppa/mainline/v5.11/amd64/linux-headers-5.11.0-051100_5.11.0-051100.202102142330_all.deb
wget https://kernel.ubuntu.com/~kernel-ppa/mainline/v5.11/amd64/linux-headers-5.11.0-051100-generic_5.11.0-051100.202102142330_amd64.deb
wget https://kernel.ubuntu.com/~kernel-ppa/mainline/v5.11/amd64/linux-image-unsigned-5.11.0-051100-generic_5.11.0-051100.202102142330_amd64.deb
wget https://kernel.ubuntu.com/~kernel-ppa/mainline/v5.11/amd64/linux-modules-5.11.0-051100-generic_5.11.0-051100.202102142330_amd64.deb
sudo dpkg -i  *.deb
sudo reboot
```

</details>

#### 2. Install dependencies

```
sudo apt-get install gcc make git libtool autoconf autogen pkg-config cmake
sudo apt-get install python3 python3-dev python3-pip
sudo apt install libssl1.0.0 libgstreamer1.0-0 gstreamer1.0-tools gstreamer1.0-plugins-good gstreamer1.0-plugins-bad gstreamer1.0-plugins-ugly gstreamer1.0-libav libgstrtspserver-1.0-0 libjansson4
sudo apt-get install libglvnd-dev
sudo apt-get install linux-headers-$(uname -r)
```

**NOTE**: Install DKMS only if you are using the default Ubuntu kernel

```
sudo apt-get install dkms
```

**NOTE**: Purge all NVIDIA driver, CUDA, etc.

```
sudo apt-get remove --purge '*nvidia*'
sudo apt-get remove --purge '*cuda*'
sudo apt-get remove --purge '*cudnn*'
sudo apt-get remove --purge '*tensorrt*'
sudo apt autoremove --purge && sudo apt autoclean && sudo apt clean
```

#### 3. Disable Nouveau

```
sudo nano /etc/modprobe.d/blacklist-nouveau.conf
```

* Add

```
blacklist nouveau
options nouveau modeset=0
```

* Run

```
sudo update-initramfs -u
```

#### 4. Reboot the computer

```
sudo reboot
```

#### 5. Download and install NVIDIA Driver without xconfig

* TITAN, GeForce RTX / GTX series and RTX / Quadro series

```
wget https://us.download.nvidia.com/XFree86/Linux-x86_64/470.103.01/NVIDIA-Linux-x86_64-470.103.01.run
sudo sh NVIDIA-Linux-x86_64-470.103.01.run
```

* Data center / Tesla series

```
wget https://us.download.nvidia.com/tesla/470.103.01/NVIDIA-Linux-x86_64-470.103.01.run
sudo sh NVIDIA-Linux-x86_64-470.103.01.run
```

**NOTE**: Only if you are using default Ubuntu kernel, enable the DKMS during the installation.

#### 6. Download and install CUDA 11.4.3 without NVIDIA Driver

```
wget https://developer.download.nvidia.com/compute/cuda/11.4.3/local_installers/cuda_11.4.3_470.82.01_linux.run
sudo sh cuda_11.4.3_470.82.01_linux.run
```

* Export environment variables

```
nano ~/.bashrc
```

* Add

```
export PATH=/usr/local/cuda-11.4/bin${PATH:+:${PATH}}
export LD_LIBRARY_PATH=/usr/local/cuda-11.4/lib64\${LD_LIBRARY_PATH:+:${LD_LIBRARY_PATH}}
```

* Run

```
source ~/.bashrc
sudo ldconfig
```

**NOTE**: If you are using a laptop with NVIDIA Optimius, run

```
sudo apt-get install nvidia-prime
sudo prime-select nvidia
```

#### 7. Download from [NVIDIA website](https://developer.nvidia.com/nvidia-tensorrt-8x-download) and install the TensorRT 8.0 GA (8.0.1)

```
echo "deb https://developer.download.nvidia.com/compute/cuda/repos/ubuntu1804/x86_64 /" | sudo tee /etc/apt/sources.list.d/cuda-repo.list
wget https://developer.download.nvidia.com/compute/cuda/repos/ubuntu1804/x86_64/7fa2af80.pub
sudo apt-key add 7fa2af80.pub
sudo apt-get update
sudo dpkg -i nv-tensorrt-repo-ubuntu1804-cuda11.3-trt8.0.1.6-ga-20210626_1-1_amd64.deb
sudo apt-key add /var/nv-tensorrt-repo-ubuntu1804-cuda11.3-trt8.0.1.6-ga-20210626/7fa2af80.pub
sudo apt-get update
sudo apt-get install libnvinfer8=8.0.1-1+cuda11.3 libnvinfer-plugin8=8.0.1-1+cuda11.3 libnvparsers8=8.0.1-1+cuda11.3 libnvonnxparsers8=8.0.1-1+cuda11.3 libnvinfer-bin=8.0.1-1+cuda11.3 libnvinfer-dev=8.0.1-1+cuda11.3 libnvinfer-plugin-dev=8.0.1-1+cuda11.3 libnvparsers-dev=8.0.1-1+cuda11.3 libnvonnxparsers-dev=8.0.1-1+cuda11.3 libnvinfer-samples=8.0.1-1+cuda11.3 libnvinfer-doc=8.0.1-1+cuda11.3
```

#### 8. Download from [NVIDIA website](https://developer.nvidia.com/deepstream-sdk) and install the DeepStream SDK 6.0.1 (6.0)

```
sudo apt-get install ./deepstream-6.0_6.0.1-1_amd64.deb
rm ${HOME}/.cache/gstreamer-1.0/registry.x86_64.bin
sudo ln -snf /usr/local/cuda-11.4 /usr/local/cuda
```

#### 9. Reboot the computer

```
sudo reboot
```

</details>

##

### Basic usage

#### 1. Download the repo

```
git clone https://github.com/marcoslucianops/DeepStream-Yolo.git
cd DeepStream-Yolo
```

#### 2. Download cfg and weights files from your model and move to DeepStream-Yolo folder

#### 3. Compile lib

* x86 platform

```
CUDA_VER=11.4 make -C nvdsinfer_custom_impl_Yolo
```

* Jetson platform

```
CUDA_VER=10.2 make -C nvdsinfer_custom_impl_Yolo
```

#### 4. Edit config_infer_primary.txt for your model (example for YOLOv4)

```
[property]
...
# 0=RGB, 1=BGR, 2=GRAYSCALE
model-color-format=0
# YOLO cfg
custom-network-config=yolov4.cfg
# YOLO weights
model-file=yolov4.weights
# Generated TensorRT model (will be created if it doesn't exist)
model-engine-file=model_b1_gpu0_fp32.engine
# Model labels file
labelfile-path=labels.txt
# Batch size
batch-size=1
# 0=FP32, 1=INT8, 2=FP16 mode
network-mode=0
# Number of classes in label file
num-detected-classes=80
...
[class-attrs-all]
# IOU threshold
nms-iou-threshold=0.45
# Score threshold
pre-cluster-threshold=0.25
```

#### 5. Run

```
deepstream-app -c deepstream_app_config.txt
```

**NOTE**: If you want to use YOLOv2 or YOLOv2-Tiny models, change the deepstream_app_config.txt file before run it

```
...
[primary-gie]
enable=1
gpu-id=0
gie-unique-id=1
nvbuf-memory-type=0
config-file=config_infer_primary_yoloV2.txt
```

##

### YOLOv5 usage

**NOTE**: Make sure to change the YOLOv5 repo version to your model version before conversion.

#### 1. Copy gen_wts_yoloV5.py from DeepStream-Yolo/utils to [ultralytics/yolov5](https://github.com/ultralytics/yolov5) folder

#### 2. Open the ultralytics/yolov5 folder

#### 3. Download pt file from [ultralytics/yolov5](https://github.com/ultralytics/yolov5/releases/) website (example for YOLOv5n 6.1)

```
wget https://github.com/ultralytics/yolov5/releases/download/v6.1/yolov5n.pt
```

#### 4. Generate cfg and wts files (example for YOLOv5n)

```
python3 gen_wts_yoloV5.py -w yolov5n.pt
```

#### 5. Copy generated cfg and wts files to DeepStream-Yolo folder

#### 6. Open DeepStream-Yolo folder

#### 7. Compile lib

* x86 platform

```
CUDA_VER=11.4 make -C nvdsinfer_custom_impl_Yolo
```

* Jetson platform

```
CUDA_VER=10.2 make -C nvdsinfer_custom_impl_Yolo
```

#### 8. Edit config_infer_primary_yoloV5.txt for your model (example for YOLOv5n)

```
[property]
...
# 0=RGB, 1=BGR, 2=GRAYSCALE
model-color-format=0
# CFG
custom-network-config=yolov5n.cfg
# WTS
model-file=yolov5n.wts
# Generated TensorRT model (will be created if it doesn't exist)
model-engine-file=model_b1_gpu0_fp32.engine
# Model labels file
labelfile-path=labels.txt
# Batch size
batch-size=1
# 0=FP32, 1=INT8, 2=FP16 mode
network-mode=0
# Number of classes in label file
num-detected-classes=80
...
[class-attrs-all]
# IOU threshold
nms-iou-threshold=0.45
# Score threshold
pre-cluster-threshold=0.25
```

#### 8. Change the deepstream_app_config.txt file

```
...
[primary-gie]
enable=1
gpu-id=0
gie-unique-id=1
nvbuf-memory-type=0
config-file=config_infer_primary_yoloV5.txt
```

#### 9. Run

```
deepstream-app -c deepstream_app_config.txt
```

**NOTE**: For YOLOv5 P6 or custom models, check the gen_wts_yoloV5.py args and use them according to your model

* Input weights (.pt) file path **(required)**

```
-w or --weights
```

* Input cfg (.yaml) file path

```
-c or --yaml
```

* Model width **(default = 640 / 1280 [P6])**

```
-mw or --width
```

* Model height **(default = 640 / 1280 [P6])**

```
-mh or --height
```

* Model channels **(default = 3)**

```
-mc or --channels
```

* P6 model

```
--p6
```

##

### YOLOR usage

#### 1. Copy gen_wts_yolor.py from DeepStream-Yolo/utils to [yolor](https://github.com/WongKinYiu/yolor) folder

#### 2. Open the yolor folder

#### 3. Download pt file from [yolor](https://github.com/WongKinYiu/yolor) website

#### 4. Generate wts file (example for YOLOR-CSP)

```
python3 gen_wts_yolor.py -w yolor_csp.pt -c cfg/yolor_csp.cfg
```

#### 5. Copy cfg and generated wts files to DeepStream-Yolo folder

#### 6. Open DeepStream-Yolo folder

#### 7. Compile lib

* x86 platform

```
CUDA_VER=11.4 make -C nvdsinfer_custom_impl_Yolo
```

* Jetson platform

```
CUDA_VER=10.2 make -C nvdsinfer_custom_impl_Yolo
```

#### 8. Edit config_infer_primary_yolor.txt for your model (example for YOLOR-CSP)

```
[property]
...
# 0=RGB, 1=BGR, 2=GRAYSCALE
model-color-format=0
# CFG
custom-network-config=yolor_csp.cfg
# WTS
model-file=yolor_csp.wts
# Generated TensorRT model (will be created if it doesn't exist)
model-engine-file=model_b1_gpu0_fp32.engine
# Model labels file
labelfile-path=labels.txt
# Batch size
batch-size=1
# 0=FP32, 1=INT8, 2=FP16 mode
network-mode=0
# Number of classes in label file
num-detected-classes=80
...
[class-attrs-all]
# IOU threshold
nms-iou-threshold=0.5
# Score threshold
pre-cluster-threshold=0.25
```

#### 8. Change the deepstream_app_config.txt file

```
...
[primary-gie]
enable=1
gpu-id=0
gie-unique-id=1
nvbuf-memory-type=0
config-file=config_infer_primary_yolor.txt
```

#### 9. Run

```
deepstream-app -c deepstream_app_config.txt
```

##

### INT8 calibration

#### 1. Install OpenCV

```
sudo apt-get install libopencv-dev
```

#### 2. Compile/recompile the nvdsinfer_custom_impl_Yolo lib with OpenCV support

* x86 platform

```
cd DeepStream-Yolo
CUDA_VER=11.4 OPENCV=1 make -C nvdsinfer_custom_impl_Yolo
```

* Jetson platform

```
cd DeepStream-Yolo
CUDA_VER=10.2 OPENCV=1 make -C nvdsinfer_custom_impl_Yolo
```

#### 3. For COCO dataset, download the [val2017](https://drive.google.com/file/d/1gbvfn7mcsGDRZ_luJwtITL-ru2kK99aK/view?usp=sharing), extract, and move to DeepStream-Yolo folder

##### Select 1000 random images from COCO dataset to run calibration

```
mkdir calibration
```

```
for jpg in $(ls -1 val2017/*.jpg | sort -R | head -1000); do \
    cp ${jpg} calibration/; \
done
```

##### Create the calibration.txt file with all selected images

```
realpath calibration/*jpg > calibration.txt
```

##### Set environment variables

```
export INT8_CALIB_IMG_PATH=calibration.txt
export INT8_CALIB_BATCH_SIZE=1
```

##### Change config_infer_primary.txt file

```
...
model-engine-file=model_b1_gpu0_fp32.engine
#int8-calib-file=calib.table
...
network-mode=0
...
```

* To

```
...
model-engine-file=model_b1_gpu0_int8.engine
int8-calib-file=calib.table
...
network-mode=1
...
```

##### Run

```
deepstream-app -c deepstream_app_config.txt
```

**NOTE**: NVIDIA recommends at least 500 images to get a good accuracy. In this example I used 1000 images to get better accuracy (more images = more accuracy). Higher INT8_CALIB_BATCH_SIZE values will increase the accuracy and calibration speed. Set it according to you GPU memory. This process can take a long time.

##

### Implement Tracking

The Deepstream allows you easy acess to different trackers with different settings. Among them are:
- IoU Tracker (Very simple one, should only be used as a Base case to compare)
- NvDCF (Nvidia's Tracker)
- DeepSORT (Alpha)

You can find more information about each specific tracker and it's settings on the [NVIDIA website](https://docs.nvidia.com/metropolis/deepstream/dev-guide/text/DS_plugin_gst-nvtracker.html#low-level-tracker-comparisons-and-tradeoffs).

For a quickstart you can run the following configs (edit first to choose which yolo you want).
```
deepstream-app -c deepstream_app_config-tracker.txt
```

```
deepstream-app -c deepstream_app_config-tracker-webcam.txt
```

If you want to customize which tracker you are using you have to edit the following lines in the deepstream config file.

You can also change the interval of the detector for faster inference. We don't need to detect every single frame if we have a tracker on, this will boost the FPS. We might loose some detections so consider the interval gap according to the use of your application.


```
[primary-gie]
enable=1
gpu-id=0
interval=2
gie-unique-id=1
nvbuf-memory-type=0
config-file=config_infer_primary_yolor.txt
```


More of these config instructions can be found on  [Using your custom model](docs/customModels.md) 

```
[tracker]
enable=1
tracker-width=640
tracker-height=384
gpu-id=0
ll-lib-file=/opt/nvidia/deepstream/deepstream/lib/libnvds_nvmultiobjecttracker.so
#ll-config-file=/opt/nvidia/deepstream/deepstream/samples/configs/deepstream-app/config_tracker_DeepSORT.yml
#enable-past-frame=1
enable-batch-process=1
```

#### Choosing different Trackers

The NVIDIA tracker plugin unites all trackers under the same common library called nvmultiobjecttracker

Each different tracker can be choosen with a config-file

For example to use the IoU Tracker you would use:
```
ll-config-file=/opt/nvidia/deepstream/deepstream/samples/configs/deepstream-app/config_tracker_IoU.yml
```

To use NvDCF with max performance
```
ll-config-file=/opt/nvidia/deepstream/deepstream/samples/configs/deepstream-app/config_tracker_NvDCF_max_perf.yml
```

To use DeepSORT:
```
ll-config-file=/opt/nvidia/deepstream/deepstream/samples/configs/deepstream-app/config_tracker_DeepSORT.yml
```

Note to make DeepStream work you first need to compile the engine according to the instructions in: [DeepSORT DeepStream Tutorial](docs/DeepSORT-YOLO-DeepStream.md) 

You can try different trackers outisde of DeepStream default library such as Byte. Instructions can be found here.
[ByteTrack DeepStream](https://github.com/chirag4798/Byte-Deepstream/)

#### Using Webcam with DeepStream

To use the USB-Cam (or any type of cam) in DeepStream you must eddit the [source] on the deepstream-config. 

More of these config instructions on different type of sources can be found on  [Using your custom model](docs/customModels.md) 


It's important that the camera fps / width / height match the settings of your camera.
```
#Camera Source
[source0]
enable=1
# Type – 1=CameraV4L2 2=URI 3=MultiURI
type=1
camera-width=640
camera-height=480
camera-fps-n=30
camera-fps-d=1
camera-v4l2-dev-node=1
num-sources=1
```

### Extract metadata

You can get metadata from deepstream in Python and C++. For C++, you need edit deepstream-app or deepstream-test code. For Python your need install and edit [deepstream_python_apps](https://github.com/NVIDIA-AI-IOT/deepstream_python_apps).

You need manipulate NvDsObjectMeta ([Python](https://docs.nvidia.com/metropolis/deepstream/python-api/PYTHON_API/NvDsMeta/NvDsObjectMeta.html)/[C++](https://docs.nvidia.com/metropolis/deepstream/sdk-api/struct__NvDsObjectMeta.html)), NvDsFrameMeta ([Python](https://docs.nvidia.com/metropolis/deepstream/python-api/PYTHON_API/NvDsMeta/NvDsFrameMeta.html)/[C++](https://docs.nvidia.com/metropolis/deepstream/sdk-api/struct__NvDsFrameMeta.html)) and NvOSD_RectParams ([Python](https://docs.nvidia.com/metropolis/deepstream/python-api/PYTHON_API/NvOSD/NvOSD_RectParams.html)/[C++](https://docs.nvidia.com/metropolis/deepstream/sdk-api/struct__NvOSD__RectParams.html)) to get label, position, etc. of bboxes.

In C++ deepstream-app application, your code need be in analytics_done_buf_prob function.
In C++/Python deepstream-test application, your code need be in osd_sink_pad_buffer_probe/tiler_src_pad_buffer_probe function.

To extract meta-data on the kitti format from the trackers and detectors you can add the following likes to your deepstream-app config under the [application] part.

Make sure to choose the directory you wish to save the metadata accordingly (create the directory previously).
```
[application]
enable-perf-measurement=1
perf-measurement-interval-sec=5
kitti-track-output-dir=/home/Tracker_Info
gie-kitti-output-dir=/home/Detector_Info
```
#### Saving Video Output of DeepStream app
To save the video output of your DeepStream app you must add another [sink] to your config file as such. 

```
[sink1]
enable=1
type=3
container=1
sync=0
codec=1
bitrate=2000000
output-file=out.mp4
```

##

### Acknowledgements
Most of this code is from MarcosLuciano's Repo. Please refer to it if for further information
.
As for my part of the config files you can do whatever you want with them.
Please check Marcos other projects and inference videos :) : https://www.youtube.com/MarcosLucianoTV

[DeepStream-Yolo](https://github.com/marcoslucianops/DeepStream-Yolo)


