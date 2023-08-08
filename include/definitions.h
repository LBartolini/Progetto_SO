#ifndef definitions_H
#define definitions_H

#define READ 0
#define WRITE 1

// modalità di avvio
#define NORMALE 0
#define INPUT_NORMALE "/dev/random"
#define INPUT_NORMALE_U "/dev/urandom"

#define ARTIFICIALE 1
#define INPUT_ARTIFICIALE "data/randomARTIFICIALE.binary"
#define INPUT_ARTIFICIALE_U "data/urandomARTIFICIALE.binary"

#define FRONT_CAMERA_DATA "res/frontCamera.data"

// log
#define ECU_LOG "log/ECU.log"
#define STEER_LOG "log/steer.log"
#define THROTTLE_LOG "log/throttle.log"
#define BRAKE_LOG "log/brake.log"
#define CAMERA_LOG "log/camera.log"
#define RADAR_LOG "log/radar.log"
#define CAR_SPEED "log/car_speed"
#define ASSIST_LOG "log/assist.log"
#define CAMERAS_LOG "log/cameras.log"

#endif