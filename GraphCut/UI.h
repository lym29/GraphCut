#pragma once
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include "segmenter.h"

class UI
{
	typedef struct 
	{
		cv::Mat image_;
		cv::Point curr_pt_;
		cv::Mat obj_mask_, bkg_mask_;
		bool is_drawing_;
	} MouseParam;
private:
	cv::Mat orig_img_;
	std::string inputfile;
	MouseParam* para_;
	Segmenter* seg_ = NULL;
public:
	UI() 
	{
		para_ = new MouseParam;
	}
	~UI() 
	{
		if (para_)
			delete para_;
		if (seg_)
			delete seg_;
	}
	static void mouseHandler(int event, int x, int y, int flags, void* param)
	{
		MouseParam* para = (MouseParam*)param;
		cv::Point p = cv::Point(x, y);
		if (event == cv::EVENT_LBUTTONDOWN)
		{
			para->is_drawing_ = true;
			para->curr_pt_ = cv::Point(x, y);
		}
		else if (event == cv::EVENT_MOUSEMOVE && flags == cv::EVENT_FLAG_LBUTTON)
		{
			// draw obj label
			if (para->is_drawing_)
			{
				cv::line(para->image_, para->curr_pt_, cv::Point(x, y), cv::Scalar(255, 0, 0), 3);
				para->curr_pt_ = p;
				if(x > 0 && y > 0 && x < para->image_.cols && y < para->image_.rows)
					para->obj_mask_.at<char>(p) = 1;
			}
		}
		else if (event == cv::EVENT_MOUSEMOVE && flags == cv::EVENT_FLAG_LBUTTON + cv::EVENT_FLAG_ALTKEY)
		{
			// draw bkg label
			if (para->is_drawing_)
			{
				cv::line(para->image_, para->curr_pt_, cv::Point(x, y), cv::Scalar(0, 255, 0), 3);
				para->curr_pt_ = p;
				if (x > 0 && y > 0 && x < para->image_.cols && y < para->image_.rows)
					para->bkg_mask_.at<char>(p) = 1;
			}
		}
		else if(event == cv::EVENT_LBUTTONUP)
		{
			para->is_drawing_ = false;
		}
	}
	bool input_img(std::string filename)
	{
		inputfile = filename;
		orig_img_ = cv::imread(filename);
		para_->image_ = orig_img_.clone();
		if (para_->image_.empty())                      // Check for invalid input
		{
			std::cout << "Could not open or find the image" << std::endl;
			return false;
		}
		int r = para_->image_.rows, c = para_->image_.cols;
		para_->obj_mask_ = cv::Mat(cv::Size(c, r), CV_8U, cv::Scalar(0));
		para_->bkg_mask_ = cv::Mat(cv::Size(c, r), CV_8U, cv::Scalar(0));
		return true;
	}
	int show()
	{
		cv::namedWindow("Display window", cv::WINDOW_NORMAL);
		cv::setMouseCallback("Display window", mouseHandler, para_);
		while (true)
		{
			cv::imshow("Display window", para_->image_);
			int k = cv::waitKey(1);
			if ( k == 27) // esc key
				break;
			if (k == 32) // space key
			{
				if (seg_)
					delete seg_;
				seg_ = new Segmenter();
				seg_->SetSrcImg(orig_img_);
				seg_->SetUserLabel(para_->obj_mask_, para_->bkg_mask_);
				seg_->ExcuteGC();
				cv::namedWindow("Result", cv::WINDOW_NORMAL);
				cv::imshow("Result", seg_->obj_graph);
				cv::namedWindow("mask", cv::WINDOW_NORMAL);
				cv::imshow("mask", seg_->img_with_mask_);
			}
			if (k == 's')
			{
				if (seg_)
				{
					std::string outputfile = inputfile.substr(0, inputfile.find_last_of('.')) + "_out.jpg";
					cv::imwrite(outputfile, seg_->obj_graph);

					std::string drawed_file = inputfile.substr(0, inputfile.find_last_of('.')) + "_drawed.jpg";
					cv::imwrite(drawed_file, para_->image_);

					std::string mask_file = inputfile.substr(0, inputfile.find_last_of('.')) + "_mask.jpg";
					cv::imwrite(mask_file, seg_->img_with_mask_);
				}
			}
		}
		return 0;
	}
};

