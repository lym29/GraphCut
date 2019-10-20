#include "segmenter.h"
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <math.h>

Segmenter::Segmenter()
{
	smoothness_term_weight_ = 1;
	graph_ = new Graph();
	hist_size_ = 8;
}

Segmenter::~Segmenter()
{

}

void Segmenter::SetSrcImg(cv::Mat& src_img)
{
	img_ = src_img;
	graph_cut_label_ = cv::Mat(img_.size(), CV_8U, cv::Scalar(DEFAULT));
}

void Segmenter::SetUserLabel(cv::Mat& obj_mask, cv::Mat& bkg_mask)
{
	obj_mask_ = obj_mask;
	bkg_mask_ = bkg_mask;
	CalcHist(obj_mask_, obj_hist_, obj_min_, obj_max_);
	CalcHist(bkg_mask_, bkg_hist_, bkg_min_, bkg_max_);
	
	graph_cut_label_.setTo(OBJ, obj_mask_ > 0);
	graph_cut_label_.setTo(BKG, bkg_mask_ > 0);
}

void Segmenter::CalcHist(cv::Mat & mask, cv::Mat & hist, double min[], double max[])
{
	//double t_min, t_max;
	cv::Mat tmp = img_.clone();
	tmp.convertTo(tmp, CV_32FC3);
	for (int dim = 0; dim < 3; dim++)
	{
		cv::Mat onechannel;
		cv::extractChannel(img_, onechannel, dim);
		cv::Mat index = mask > 0;
		cv::Point minLoc; cv::Point maxLoc;
		cv::minMaxLoc(onechannel, min + dim, max + dim, & minLoc, & maxLoc);
	}

	const int histSize[] = { hist_size_, hist_size_, hist_size_ };
	float range0[] = { min[0], max[0]+1 }; //the upper boundary is exclusive
	float range1[] = { min[1], max[1]+1 };
	float range2[] = { min[2], max[2]+1 };
	const float* histRange[] = { range0, range1, range2 };
	const int channels[] = { 0, 1, 2 };
	bool uniform = true, accumulate = false;
	
	cv::Mat hist_per_channel[3];
	cv::calcHist(&tmp, 1, channels, mask, hist, 3, histSize, histRange, uniform, accumulate);
	/*for (int dim = 0; dim < 3; dim++)
	{
		const int c[] = { dim };
		hist_per_channel[dim].convertTo(hist_per_channel[dim], CV_32FC3);
		cv::calcHist(&tmp, 1, c, mask, hist_per_channel[dim], 1, histSize, histRange+dim, uniform, accumulate);
	}

	for (int i = 0; i < hist_size_; i++)
	{
		for (int j = 0; j < hist_size_; j++)
		{
			for (int k = 0; k < hist_size_; k++)
			{
				hist.at<float>(i, j, k) = hist_per_channel[0].at<float>(i, 0) 
					+ hist_per_channel[1].at<float>(j, 0) + hist_per_channel[2].at<float>(k, 0);
				hist.at<float>(i, j, k) /= 3;
			}
		}
	}*/
	
}

void Segmenter::SetDataCost(cv::Point pt)
{
	int dim = 3;
	int K = img_.rows * img_.cols;
	cv::Vec3b color = img_.at<cv::Vec3b>(pt);
	cv::Vec3i id_in_h;
		
	double w_obj = 0, w_bkg = 0;
	if (obj_mask_.at<char>(pt) != 0)
	{
		w_obj = K;
	}
	else if (bkg_mask_.at<char>(pt) != 0)
	{
		w_bkg = K;
	}
	//else
	//{
	//	for (int dim = 0; dim < 3; dim++)
	//	{
	//		int bin = (obj_max_[dim] - obj_min_[dim]) / hist_size_ + 1;
	//		id_in_h[dim] = (color[dim] - obj_min_[dim]) / bin;
	//	}
	//	w_obj = obj_hist_.at<float>(id_in_h[0], id_in_h[1], id_in_h[2]);
	//	//w_obj = obj_hist_.at<float>((id_in_h[0]+id_in_h[1]+id_in_h[2])/3, 0);

	//	for (int dim = 0; dim < 3; dim++)
	//	{
	//		int bin = (bkg_max_[dim] - bkg_min_[dim]) / hist_size_ + 1;
	//		id_in_h[dim] = (color[dim] - bkg_min_[dim]) / bin;
	//	}
	//	w_bkg = bkg_hist_.at<float>(id_in_h[0], id_in_h[1], id_in_h[2]);
	//	//w_bkg = bkg_hist_.at<float>((id_in_h[0] + id_in_h[1] + id_in_h[2]) / 3, 0);
	//	if(w_obj > 1)
	//		w_obj = log(w_obj);
	//	if(w_bkg > 1)
	//		w_bkg = log(w_bkg);
	//	
	//}
	
	int n = pt.y * img_.cols + pt.x;
	
	graph_->add_terminal_weight(n, w_obj, w_bkg);
}

void Segmenter::SetSmoothnessCost(cv::Point p, cv::Point q)
{
	double sigma = 1;
	int label_p = graph_cut_label_.at<char>(p);
	int label_q = graph_cut_label_.at<char>(q);
	double w = 0;
	if (label_p != label_q || label_p == 0)
	{
		cv::Vec3f cp = img_.at<cv::Vec3b>(p) / 255.0;
		cv::Vec3f cq = img_.at<cv::Vec3b>(q) / 255.0;
		//w = std::exp(-(cp - cq).dot(cp - cq) / 2 / sigma / sigma);
		w = 1/(1+(cp - cq).dot(cp - cq));
		int np = p.y * img_.cols + p.x;
		int nq = q.y * img_.cols + q.x;
		w *= smoothness_term_weight_;
		graph_->add_weight(np, nq, w, 0);
	}
	
}

void Segmenter::ExcuteGC()
{
	graph_->add_nodes(img_.rows * img_.cols);
	for (int i = 0; i < img_.rows; i++)
	{
		for (int j = 0; j < img_.cols; j++)
		{
			cv::Point p = cv::Point(j, i);
			for (int di = -1; di <= 1; di++)
			{
				for (int dj = -1; dj <= 1; dj++)
				{
					int new_i = i + di;
					int new_j = j + dj;

					if (new_i == i && new_j == j)
						continue;
					if (new_i < 0 || new_j < 0 || new_i >= img_.rows || new_j >= img_.cols)
						continue;
					cv::Point q = cv::Point(j + dj, i + di);
					SetSmoothnessCost(p, q);
				}
			}
			SetDataCost(p);
			
		}
	}
	graph_->maxflow();
	std::vector<int> obj_set, bkg_set;
	graph_->get_min_cutted_sets(obj_set, bkg_set);
	graph_cut_label_.setTo(DEFAULT);
	for (int i = 0; i < obj_set.size(); i++)
	{
		cv::Point p = cv::Point(obj_set[i] % img_.cols, obj_set[i] / img_.cols);
		graph_cut_label_.at<char>(p) = OBJ;
	}

	cv::Mat labels;
	std::vector<cv::Mat> labelVector(3);
	labelVector[0] = labelVector[1] = labelVector[2] = graph_cut_label_;
	cv::merge(labelVector, labels);
	obj_graph = img_.mul(labels);

	cv::Mat mask = cv::Mat(img_.size(), CV_8UC3);
	mask.setTo(cv::Vec3b(0, 0, 255), graph_cut_label_ == OBJ);
	cv::addWeighted(img_, 1, mask, 0.3, 0, img_with_mask_);
}

