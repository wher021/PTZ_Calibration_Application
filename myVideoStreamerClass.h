//Video Thread Class
cv::Mat Zoom_frame, IR_frame, tmp;

public ref class VideoStreamer //ref class is needed when using System objects in CLR
{
public:
	bool ZoomCam_Digital_Zoom_Active,Grid_Active;
	unsigned int D_ZoomLevel_Zoom_Cam, D_ZoomLevel_IR_Cam, O_ZoomLevel_Zoom_Cam;
	unsigned int colMax, rowMax, pan_step_size, tilt_step_size;
	int grid_ref_axis_x, grid_ref_axis_y;
	bool aimOn;
	cv::Mat VideoStreamer::AimDrawer(cv::Mat frame);
	cv::Mat VideoStreamer::GridDrawer(cv::Mat frame);//in progress
	void VideoStreamer::Set_Grid_Reference_Axis(int x, int y);//in progress
	cv::Point VideoStreamer::GetCenterPoints(cv::Mat frame);
	double VideoStreamer::Get_Pixel_Pan_ratio();
	double VideoStreamer::Get_Pixel_Tilt_ratio();

	VideoStreamer()
	{
		ZoomCam_Active = false;
		Grid_Active = false;
		IRCam_Active = false;
		D_ZoomLevel_Zoom_Cam = 1;
		D_ZoomLevel_IR_Cam = 1;
		O_ZoomLevel_Zoom_Cam = 1;
		aimOn = true;
		colMax = 0;
		rowMax = 0;
		grid_ref_axis_x = 0;
		grid_ref_axis_y = 0;
	}
	void StreamZoomVideo(Object^ address)
	{
		//("rtp://@239.0.0.3:10000")ir cam
		//("rtp://@239.47.3.45:22144")//zoom multicast
		//////("rtsp:Admin:Camera@10.10.0.91:554/mjpg");// zoom rtsp

		std::string tmp = "rtp://@239.47.3.45:22144";
		System::String^ system_string;
		system_string = address->ToString();

		std::string ip_address;
		ip_address = msclr::interop::marshal_as<std::string>(system_string);

		cv::VideoCapture ZoomCapture(tmp);//change this to ip_address

		if (!ZoomCapture.isOpened())
		{
			std::cout << "error opening zoom cam stream" << std::endl;
		}
		//cv::namedWindow("IRCam", CV_WINDOW_AUTOSIZE);

		while (1)
		{
			Monitor::Enter(this);//if more than one thread is trying to use this function it will wait one after the other (semaphore)//mutex
			if (ZoomCam_Active)
			{
				cv::namedWindow("ZoomCam", CV_NORMAL);
				if (!ZoomCapture.read(Zoom_frame)) {
					//Error
				}
				if (D_ZoomLevel_Zoom_Cam > 1)//check if zoom is active
				{
					Zoom_frame = ZoomIn(Zoom_frame, D_ZoomLevel_Zoom_Cam);//testing for digital zoom
				}
				if (aimOn)
				{
					Zoom_frame = AimDrawer(Zoom_frame);
				}
				if (Grid_Active)
				{
					Zoom_frame = GridDrawer(Zoom_frame);
				}
				cv::imshow("ZoomCam", Zoom_frame);
				cv::waitKey(1);//Highgui gets time to update frame

				//std::cout << "hej fran zoom" << std::endl;
			}
			else
			{
				cv::destroyWindow("ZoomCam");
			}
			Monitor::Exit(this);
		}
	}

	void StreamIRVideo(Object^ address)
	{
		std::string tmp = "rtp://@239.0.0.3:10000";

		System::String^ system_string;
		system_string = address->ToString();

		std::string ip_address;
		ip_address = msclr::interop::marshal_as<std::string>(system_string);

		cv::VideoCapture IRCapture(tmp);//change this to ip_address

		if (!IRCapture.isOpened())
		{
		}
		while (1)
		{
			Monitor::Enter(this);
			if (IRCam_Active)
			{
				cv::namedWindow("IRCam", CV_NORMAL);
				if (!IRCapture.read(IR_frame)) 
				{
				}
				if (D_ZoomLevel_IR_Cam > 1)//check if zoom is active
				{
					IR_frame = ZoomIn(IR_frame, D_ZoomLevel_IR_Cam);//testing for digital zoom
				}
				if (aimOn)
				{
					IR_frame = AimDrawer(IR_frame);
				}
				cv::imshow("IRCam", IR_frame);
				cv::waitKey(1);//Highgui gets time to update frame

				//std::cout << "hej fran IR" << std::endl;
			}
			else
			{
				cv::destroyWindow("IRCam");
			}
			Monitor::Exit(this);
		}
	}

	void set_Zoom_Flag(bool status)
	{
		ZoomCam_Active = status;
	}
	void set_IR_Status_Flag(bool status)
	{
		IRCam_Active = status;
	}
	bool get_Zoom_Flag()
	{
		return ZoomCam_Active;
	}
	bool get_IR_Flag()
	{
		return IRCam_Active;
	}
	bool get_Grid_Flag()
	{
		return Grid_Active;
	}
	void set_Grid_Flag(bool status)
	{
		Grid_Active = status;
	}
	void set_O_Zoom_Level(int lvl)
	{
		O_ZoomLevel_Zoom_Cam = lvl;
	}
	void setGridDimensions(unsigned int col,unsigned int row,unsigned int param_pan_step_size,unsigned int param_tilt_step_size)
	{
		rowMax = row;
		colMax = col;
		pan_step_size = param_pan_step_size;
		tilt_step_size = param_tilt_step_size;
	}
private:
	bool ZoomCam_Active;//720x480 height width
	bool IRCam_Active;
	cv::Mat ZoomIn(cv::Mat Reference_Frame, unsigned int zoom_level);
	const double pixel_pan_ratio = 0.3281;
	const double pixel_tilt_ratio = 0.3691;
};

cv::Mat VideoStreamer::ZoomIn(cv::Mat Reference_Frame, unsigned int zoom_level)
{
	cv::Size s = Reference_Frame.size();
	std::cout << s.height << " " << s.width << std::endl;

	int zoom = zoom_level;//zoom is 1 then no zoom
	cv::Mat img = Reference_Frame;
	cv::Size img_size = img.size();
	cv::Rect ROI;

	ROI.x = cvFloor((((img_size.width / zoom) * (zoom / 2.0)) - ((img_size.width / zoom) / 2.0)));
	ROI.y = cvFloor((((img_size.height / zoom) * (zoom / 2.0)) - ((img_size.height / zoom) / 2.0)));

	ROI.width = cvFloor((img_size.width / zoom));
	ROI.height = cvFloor((img_size.height / zoom));

	//cv::resize(img(ROI), Reference_Frame, s, 1);

	//cv::Size(1024, 768)
	resize(img(ROI), Reference_Frame, cv::Size(800, 600), 0, 0, cv::INTER_CUBIC);

	return Reference_Frame;
}
cv::Point VideoStreamer::GetCenterPoints(cv::Mat frame)
{
	cv::Size frame_dimensions = frame.size();
	cv::Point center;
    center.x = frame_dimensions.width / 2;
	center.y = frame_dimensions.height / 2;
	return center;
}

double VideoStreamer::Get_Pixel_Pan_ratio()
{
	return pixel_pan_ratio;
}

double VideoStreamer::Get_Pixel_Tilt_ratio()
{
	return pixel_tilt_ratio;
}

cv::Mat VideoStreamer::AimDrawer(cv::Mat frame)
{
	cv::Size frame_dimensions = frame.size();
	cv::Point hp1, hp2;
	int line_length = 100;
	hp1.x = frame_dimensions.width / 2 - line_length / 2;
	hp1.y = frame_dimensions.height / 2;
	hp2.x = frame_dimensions.width / 2 + line_length / 2;
	hp2.y = frame_dimensions.height / 2;

	cv::Point vp1, vp2;
	vp1.x = frame_dimensions.width / 2;
	vp1.y = frame_dimensions.height / 2 - line_length / 2;
	vp2.x = frame_dimensions.width / 2;
	vp2.y = frame_dimensions.height / 2 + line_length / 2;

	unsigned int blue, green, red;
	unsigned int row, col;
	unsigned int pixelcount = 0;

	for (row = vp1.y; row < vp2.y; row++)
	{
		for (col = hp1.x; col < hp2.x; col++)
		{
			cv::Vec3b intensity = frame.at<cv::Vec3b>(cv::Point(col, row));
			pixelcount++;
			blue += intensity.val[0];
			green += intensity.val[1];
			red += intensity.val[2];
		}
	}
	blue = blue / pixelcount;
	green = green / pixelcount;
	red = red / pixelcount;

	cv::Mat bgr(1, 1, CV_8UC3, cv::Scalar(blue, green, red));//dynamically set color from ROI
	//bgr = cv::Scalar(blue, green, red);

	cv::Mat ycbcr = cv::Mat::zeros(1, 1, CV_8UC3);

	cv::cvtColor(bgr, ycbcr, CV_BGR2YCrCb);

	//std::cout << (int)ycbcr.data[0] << std::endl; //kom ihåg att casta before print
	
	cv::Mat chan[3];
	cv::split(ycbcr, chan);

	cv::Mat y = chan[0];

	//std::cout << (int)y.data[0] << std::endl;

	float check_lum = (float)y.data[0] / 255.0;

	//std::cout << check_lum << std::endl;

	if (check_lum >= 0 && check_lum < 0.25)
		check_lum = 1;
	else if (check_lum >= 0.25 && check_lum < 0.5)
		check_lum = 0.85;
	else if (check_lum >= 0.5 && check_lum < 0.75)
		check_lum = 0.25;
	else if (check_lum >= 0.75 && check_lum < 1.0)
		check_lum = 0;
	else
		check_lum = 0;

	y.data[0] = (check_lum * 255);

	//std::cout << (int)y.data[0] << std::endl;

	chan[0] = y;

	std::vector<cv::Mat> array_to_merge;
	array_to_merge.push_back(chan[0]);
	array_to_merge.push_back(chan[1]);
	array_to_merge.push_back(chan[2]);

	cv::Mat tmp;
	cv::merge(array_to_merge, tmp);

	//std::cout << (int)tmp.data[0] << std::endl;

	cv::cvtColor(tmp, bgr, CV_YCrCb2BGR);

	//std::cout << (int)bgr.data[0] << std::endl;
	
	cv::Mat _BGR[3];
	cv::split(bgr, _BGR);

	//std::cout << (int)_BGR[0].data[0] << "	 " << (int)_BGR[1].data[0] << "  " << (int)_BGR[2].data[0] << std::endl;

	int B = (int)_BGR[0].data[0];
	int G = (int)_BGR[1].data[0];
	int R = (int)_BGR[2].data[0];

	cv::line(frame, hp1, hp2, cv::Scalar(B, G, R), 2, CV_AA);
	cv::line(frame, vp1, vp2, cv::Scalar(B, G, R), 2, CV_AA);
	return frame;
}

void VideoStreamer::Set_Grid_Reference_Axis(int c, int r)
{
	grid_ref_axis_x = c * pan_step_size * 0.3281 * O_ZoomLevel_Zoom_Cam;
	grid_ref_axis_y = r * tilt_step_size * 0.3691 * O_ZoomLevel_Zoom_Cam;
}

cv::Mat VideoStreamer::GridDrawer(cv::Mat frame)
{
	
	cv::Point center = GetCenterPoints(frame);
	center.x = center.x - grid_ref_axis_x;//these 2 are continiously update by robot movement
	center.y = center.y + grid_ref_axis_y;

	double Roi_width = colMax * ((double)pan_step_size * (double)pixel_pan_ratio) * O_ZoomLevel_Zoom_Cam;
	double Roi_height = rowMax * ((double)tilt_step_size * (double)pixel_tilt_ratio) * O_ZoomLevel_Zoom_Cam;

	double center_rect;//need to position grid in center of Crosshair
	double upper_left_corner_rect;//need to position grid in center of Crosshair
	
	if (grid_ref_axis_x == 0 && grid_ref_axis_y == 0)//if negative center it
	{
		center_rect = center.x - Roi_width / 2;
		upper_left_corner_rect = center.y - Roi_height / 2;
	}
	else
	{
		center_rect = center.x;
		upper_left_corner_rect = center.y - Roi_height;
	}

	cv::Rect Roi = cv::Rect(center_rect, upper_left_corner_rect, Roi_width, Roi_height);//draw ROI
	cv::rectangle(frame, Roi, CV_RGB(0, 0, 255), 1);

	cv::Point col_p1, col_p2;

	col_p1.x = center_rect;//center
	col_p2.x = center_rect;//center
	col_p1.y = upper_left_corner_rect;
	col_p2.y = upper_left_corner_rect + Roi_height;

	for (int i = 0; i < colMax - 1; i++)
	{
		col_p1.x = col_p1.x + ((pan_step_size * pixel_pan_ratio) * O_ZoomLevel_Zoom_Cam);
		col_p2.x = col_p2.x + ((pan_step_size * pixel_pan_ratio) * O_ZoomLevel_Zoom_Cam);
		cv::line(frame, col_p1, col_p2, CV_RGB(255, 0, 0), 1);
	}

	cv::Point row_p1, row_p2;

	row_p1.x = center_rect;
	row_p1.y = upper_left_corner_rect;
	row_p2.x = row_p1.x + Roi.width;
	row_p2.y = row_p1.y;

	for (int j = 0; j < rowMax - 1; j++)
	{
		row_p1.y = row_p1.y + ((tilt_step_size * pixel_tilt_ratio) * O_ZoomLevel_Zoom_Cam);
		row_p2.y = row_p2.y + ((tilt_step_size * pixel_tilt_ratio) * O_ZoomLevel_Zoom_Cam);
		cv::line(frame, row_p1, row_p2, CV_RGB(255, 0, 0), 1);
	}

	return frame;
	

	/*
	int thickness = 2;
	int lineType = 8;

	cv::Point center = GetCenterPoints(frame);
	
	cv::Point p1 = center;
	cv::Point p2 = center;
	p1.x = center.x - 100 * 0.3281;
	p2.x = center.x - 100 * 0.3281;

	p2.y = center.y - 100 * 0.36;

	if (!Grid_Active)
	{
		cv::line(frame, p1, p2, CV_RGB(0, 0, 0), 2);
	}
	else
	{
		//p1.x = p1.x + 100 * 0.36;
		//p2.x = p2.x + 100 * 0.36;

		cv::line(frame, p1, p2, CV_RGB(0, 0, 0), 2);
		//cv::calcOpticalFlowPyrLK
	}
	return frame;
	*/	
}