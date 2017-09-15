# -*- coding: utf-8 -*-
#!/usr/bin/python3

import cv2
import datetime
import numpy as np
import os
import paramiko
import time
from picamera.array import PiRGBArray
from picamera import PiCamera
from scipy.ndimage.measurements import label
from scp import SCPClient


class active_object_detect(object):
    """An active object detect project based on raspberry pi and picamera.
    
    Functions:
        __init__(self, resolution, framerate, structure, threshold, 
            video_save_path, recording_time, hostname, username, remote_path):
            class initialization.
        camera(self): Prepare camera.
        detect(self, fgmask): Detects whether there is any active object in an
            image.
        get_frame(self, camera, rawCapture): Get a frame from camera.
        recording(self, camera, rawCapture, filename): Records a video.
        process_frame(self, frame, fgbg, kernel): The processes of the image.
        upload(self, filenames): Uploads file.
        active_object_detect(self): The main script of active object detect.

    
    Example:
        rpi=active_object_detect()
        rpi.active_object_detect()
    
    Attributes:
        resolutions: The resolution of the camera.
        framerate: The framerate of the camera.
        structure: The shape of the connected area.
        threshold: The threshold to determine whether there is an active object
            in the image.
        video_save_path: Local path to save video and image.
        recording_time: The length of the video.
        hostname: Host ip.
        username: Host user name.
        remote_path: Host path to save video and image.
    """
    
    def __init__(self, resolution=(640,480), framerate=32, structure=np.ones((3,3)), 
            threshold=1600, video_save_path='video/', recording_time=30,
            hostname='192.168.31.184', username='kevin', 
            remote_path='~/fenglan/rpi-human-detect/video/', abs_diff_threshold=10):
        """Class initialization."""

        self.resolution=resolution
        self.framerate=framerate
        self.structure=structure
        self.threshold=threshold
        self.video_save_path=video_save_path
        self.recording_time=recording_time
        self.hostname=hostname
        self.username=username
        self.remote_path=remote_path
        self.abs_diff_threshold = abs_diff_threshold

    def camera(self):
        """Prepare camera.
        
        Opens picamera, sets resolution and framerate to camera, and create a 
        PiRGBArray object to store frame data.

        Args:
            None.

        Returns:
            camera: A picamera.PiCamera object.
            rawCapture: A picamera.array.PiRGBArray object.
        """
        camera = PiCamera()
        camera.resolution = self.resolution
        camera.framerate = self.framerate
        rawCapture = PiRGBArray(camera, size=self.resolution)
        
        return camera, rawCapture

    def detect(self, fgmask):
        """Detects whether there is any active object in an image.

        Calculates the size of the x-connected areas in the image, if the 
        pixels of the x-connected area is greater than the threshold, returns
        True;returns False for otherwise.
        
        Args:
            fgmask: array, processed image data.

        Returns:
            bool, true if pixels number of the x-connected area is greater
                than the threshold. 
        """
        labeled_array, num_features = label(fgmask, structure=self.structure)
        
        for i in range(num_features):
            if np.sum(labeled_array==i+1) > self.threshold:
                return True
       
        return False

    def get_frame(self, camera, rawCapture):
        """Get a frame from camera.
        
        Args:
            camera: A picamera.PiCamera object.
            rawCapture: A picamera.array.PiRGBArray object.

        Returns:
            rawCapture.array: array, a frame capture from camera.
        """
        camera.capture(rawCapture, format='bgr', use_video_port=True)
        
        return rawCapture.array

    def recording(self, camera, rawCapture, filename):
        """Records a video.
        
        Records a video length of self.recording_time and named filename.

        Args:
            camera: A picamera.PiCamera object.
            rawCapture: A picamera.array.PiRGBArray object.
            filename: The name of the video file.

        Returns:
            None.
        """
        videowriter = cv2.VideoWriter('%s'%filename, cv2.VideoWriter_fourcc('X','2','6','4'),
                self.framerate, self.resolution)
        start = time.time()
        while time.time() - start <= self.recording_time:
            camera.annotate_text = datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S')
            camera.capture(rawCapture, format='bgr', use_video_port=True)
            videowriter.write(rawCapture.array)
            rawCapture.truncate(0)

        videowriter.release()

    def process_frame(self, frame, fgbg, kernel):
        """The processes of the image.

        Some processes of the image, include: BGR2GRAY, Histogram equalization,
        Gaussian smoothing, Background Subtraction, Morphological Transformations.
       
        Args:
            frame: array, image data.
            fgbg: object, Background Substraction object.
            kernel: object, structuring element.

        Returns:
            fgmask, array, processed image data.
        """
        gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
        gray = cv2.equalizeHist(gray)
        gray = cv2.GaussianBlur(gray, (21, 21), 0)
        fgmask = fgbg.apply(gray)
        fgmask = cv2.morphologyEx(fgmask, cv2.MORPH_OPEN, kernel)
        
        return fgmask


    def upload(self, filenames):
        """Uploads file.
        
        Upload videos and images from local disk to the host by ssh.

        Args:
            filenames: A list of filenames waiting for uploading.

        Returns:
            None.
        """
        print('Uploading %s ...' %filenames)
        ssh = paramiko.SSHClient()
        ssh.load_system_host_keys()
        ssh.set_missing_host_key_policy(paramiko.AutoAddPolicy())
        ssh.connect(hostname=self.hostname, username=self.username)
        scp = SCPClient(ssh.get_transport())

        scp.put(files=filenames, remote_path=self.remote_path)
        scp.close()
        [os.remove(name) for name in filenames]
        print('Upload %s finished!' %filenames)


    def active_object_detect(self):
        """The main script of active object detect.

        Firstly, creates a camera, rawCapture, fgbg and kernel object.
        Secondly, captures two frames and detect whether there is any active
        object in the frame after several processes to these two frames.
        If detects any active object in the frame, then records a video and 
        create a child process to uploads the video as well as previous two 
        frames to the host computer. 
        """
        camera, rawCapture = self.camera()
        time.sleep(0.5)
        fgbg = cv2.createBackgroundSubtractorMOG2(detectShadows=True)
        kernel = cv2.getStructuringElement(cv2.MORPH_ELLIPSE, (3,3))
       
        while True:
            frame1 = self.get_frame(camera, rawCapture)
            fgmask = self.process_frame(frame1, fgbg, kernel)
            rawCapture.truncate(0)
            frame2 = self.get_frame(camera, rawCapture)
            fgmask = self.process_frame(frame2, fgbg, kernel)
            rawCapture.truncate(0)
            
            gray1, gray2 = cv2.cvtColor(frame1, cv2.COLOR_BGR2GRAY), cv2.cvtColor(frame2, cv2.COLOR_BGR2GRAY)
            abs_diff = cv2.absdiff(gray1, gray2)
            abs_diff = cv2.threshold(abs_diff, self.abs_diff_threshold, 1, cv2.THRESH_BINARY)[1]
            fgmask = abs_diff * fgmask

            if self.detect(fgmask):
                date_time = datetime.datetime.now().strftime('%Y-%m-%d-%H-%M-%S')
                filename = os.path.join(self.video_save_path, date_time+'.h264')
                print('Start recording at %s'%date_time)
                self.recording(camera, rawCapture, filename)
                fgbg = cv2.createBackgroundSubtractorMOG2(detectShadows=True)
                pid = os.fork()
                if pid == 0:
                    frame1_name = os.path.join(self.video_save_path, date_time+'-1.jpg')
                    frame2_name = os.path.join(self.video_save_path, date_time+'-2.jpg')
                    cv2.imwrite('%s'%frame1_name, frame1)
                    cv2.imwrite('%s'%frame2_name, frame2)
                    self.upload([filename, frame1_name, frame2_name])
                    exit()                    
                else:
                    pass
            else:
                pass
           
            time.sleep(0.2)

def main():
    resolution = (640, 480)
    framerate = 16
    threshold =8000
    rpi = active_object_detect(resolution=resolution, 
            framerate=framerate, threshold=threshold)

    if not os.path.exists(rpi.video_save_path):
        os.mkdir(rpi.video_save_path)

    rpi.active_object_detect()


if __name__ == '__main__':
    main()
