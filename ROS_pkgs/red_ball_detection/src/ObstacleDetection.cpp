#include <ObjectDetection.h>

using namespace cv;
using namespace std;


// This is the object constructor for the class ObstacleDetection. When the object is created in the main, it requires a ros::NodeHandle argument.
// See definition in detector.cpp
ObstacleDetection::ObstacleDetection(const string sub_topic_name, const string pub_topic_name, const string pub_display) : itp(nh_)
{
	
	// Subscribe to camera Image (right or left) 
	img_subscriber = itp.subscribe(topic_name, 1, ObstacleDetection::cv_ros_iface, this);

	// Create publisher to display image (for debugging, no need in the final application)
	img_publisher = itp.advertise(pub_display, 1);
	
	// Create Publisher and topic to broadcast image coordinates of the ball in the form of rosmsg [/red_ball_detection/ballCentrum]
	pub_coord = nh_.adverstise(pub_topic_name ,1);

	// Create OpenCV Window to display
	cv::namedWindow(pub_display);

} // ObstacleDetection()



// Destructor
~ObstacleDetection::ObstacleDetection()
{
	cv::destroyWindow(OPENCV_WINDOW);

} // ~ObstacleDetection()




// Interface between ROS image messages and OpenCV Mat data structure (from ROS tutorial cv_bridge C++)
void ObstacleDetection::cv_ros_iface(const sensor_msgs::ImageConstPtr& msg)
{

	cv_bridge::CvImagePtr cv_ptr;
	namespace enc = sensor_msgs::image_encodings;

   	try
    	{
      		cv_ptr = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::BGR8);

    	} // try
    	
	catch (cv_bridge::Exception& e)
    	{
      		ROS_ERROR("cv_bridge exception: %s", e.what());
      		return;

    	} // catch

    	// Perform detection
    	if (cv_ptr->image.rows > 400 && cv_ptr->image.cols > 600)
	{

		// Call to the detection function
		find_ball(cv_ptr->image);
		
		// Publish thresholded image (debugging)
    		img_publisher.publish(cv_ptr->toImageMsg());

	} // if

} // cv_ros_iface()


//---------------------------------------------------------


// Functions to create the trackbar for adjustment of detection thresholds.
void ObstacleDetection::track_low_h(int,void*)
{
	setTrackbarPos("min_h","RedBall",low_h);
}// track_low_h()

void ObstacleDetection::track_high_h(int,void*)
{
	setTrackbarPos("max_h","RedBall",high_h);
}// track_high_h()


//---------------------------------------------------------------



// Function to detect the red ball in the scene
void ObstacleDetection::find_ball(cv::Mat img, const string window_name)
{

	Mat hsv;
	cv::GaussianBlur(img, hsv, Size(3,3), 2, 2);
	cvtColor(hsv, hsv, CV_BGR2HSV);
	
	Mat RedBall;
	inRange(hsv,Scalar(low_h,120,120),Scalar(high_h,255,255),RedBall);
	
	namedWindow(window_name, WINDOW_NORMAL);
	
	createTrackbar("min_H",window_name,&low_h,255,track_low_h);
	createTrackbar("max_H",window_name,&high_h,255,track_high_h);

	imshow(window_name, RedBall);
	waitKey(1);
	
	int m_size = 2;
	Mat element = getStructuringElement(MORPH_ELLIPSE, Size(m_size+1, m_size+1), Point(m_size,m_size));
	morphologyEx(RedBall,RedBall, MORPH_OPEN, element,Point(-1,-1),6);
	
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	findContours(RedBall, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0,0));
		
	vector<vector<Point> > contours_poly(contours.size());
	vector<Rect> boundRect (contours.size());
	vector<Point2f> center(contours.size());
	vector<float> radius(contours.size());
  	
  	for(int i=0; i<contours.size(); i++)
	{
		approxPolyDP(contours[i],contours_poly[i],5,true);
		boundRect[i] = boundingRect(contours_poly[i]);
		minEnclosingCircle(contours_poly[i],center[i],radius[i]);
	}// for

	Mat Box = Mat::zeros(RedBall.size(), CV_8UC3);
	for(int i=0; i< contours.size(); i++)
	{
		drawContours(Box, contours, -1, Scalar(0,0,255),1);
		rectangle(Box, boundRect[i], Scalar(0,255,255), 1,8,0);
		circle(Box,center[i],radius[i], Scalar(255,0,0),1,8,0);
		cout<< "center = " << center[i]<< endl;
	}// for
	
	// Some additional coding to convert vector<Point2f> center into float64[2] (and filter false positives)


} // find_ball()






