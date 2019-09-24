#include <libusb-1.0/libusb.h>
#include <ros/ros.h>
#include <std_msgs/UInt8.h>
#include <std_msgs/UInt16.h>
#include <std_msgs/Float64.h>
#include <sensor_msgs/Imu.h>

#include <tf/transform_broadcaster.h> // !! added
#include <eigen3/Eigen/Dense>
#include <eigen3/Eigen/Core>
#include <math.h> 

// VID and PID for Kinect and motor/acc/leds
#define MS_MAGIC_VENDOR 0x45e
#define MS_MAGIC_MOTOR_PRODUCT 0x02b0
// Constants for accelerometers
#define GRAVITY 9.80665
#define FREENECT_COUNTS_PER_G 819.
// The kinect can tilt from +31 to -31 degrees in what looks like 1 degree increments
// The control input looks like 2*desired_degrees
#define MAX_TILT_ANGLE 3.0 
#define MIN_TILT_ANGLE -60.0

#define PI 3.14159265359

ros::Publisher pub_imu;
ros::Publisher pub_tilt_angle;
ros::Publisher pub_tilt_status;

ros::Subscriber sub_tilt_angle;
ros::Subscriber sub_led_option;
 
libusb_device_handle *dev(0);

Eigen::MatrixXf baxterCalibration(4,4); // homogeneus matrix
tf::Transform transformTilt;
tf::Transform transformFrame;

int main(int argc, char* argv[])
{
	int ret = libusb_init(0);
	if (ret)
	{
		ROS_ERROR_STREAM("Cannot initialize libusb, error: " << ret);
		return 1;
	}
	

	// initialise kinect baxter calibration (with respect to tilt=0°)
/*
	baxterCalibration <<   0.0138,    0.999,    -0.0021,   0.0662,
    										 0.3081,   -0.0012,    0.9504,    0.0672,//0.0714,
												 0.9504,    -0.0129,   -0.3072,    0.0396,//0.1204,
         								 0,         0,         0,         1.0;

*/
   
       

    
    
    
  baxterCalibration <<   1.0,    0.0,   0.0,   0.2024,
    										 0.0,    1.0,   0.0,   -0.0848,//0.0714,
												 0.0,    0.0,   1.0,   0.2512,//0.1204,
         								 0.0,    0.0,   0.0,      1.0;
  Eigen::MatrixXf trans(4,4);
	
	trans = baxterCalibration; 
	tf::Vector3 trans_Tf( trans(0,3), trans(1,3), trans(2,3));
	ROS_INFO("dopo trans tf");
  tf::Matrix3x3 trans_Rf(trans(0,0), trans(0,1), trans(0,2),trans(1,0), trans(1,1), trans(1,2),trans(2,0), trans(2,1), trans(2,2));
	ROS_INFO("dopo trans Rf");
	
	transformFrame.setOrigin( trans_Tf);
	transformFrame.setBasis( trans_Rf);	


	ros::init(argc, argv, "kinectOne_aux");
	ros::NodeHandle n;
	
	static tf::TransformBroadcaster br;
	
	ros::Rate rate(50);

	while (ros::ok()) //( n.ok())
	{
		ROS_INFO("firart");
		br.sendTransform(tf::StampedTransform(transformFrame, ros::Time::now(), "/base", "kinect2_link"));
		ROS_INFO("second");

		ros::spinOnce();
		rate.sleep();
	}
	
	libusb_exit(0);
	return 0;
}
