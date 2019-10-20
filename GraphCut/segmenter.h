#include <opencv2/core.hpp>
#include <vector>
#include "Graph.h"
class Segmenter
{
public:
	Segmenter();
	~Segmenter();
	void SetSrcImg(cv::Mat& src_img);
	void SetUserLabel(cv::Mat & obj_mask, cv::Mat& bkg_mask);
	typedef enum {DEFAULT = 0, OBJ = 1, BKG = 2} label_type;
private:
	Graph* graph_;
	cv::Mat img_;
	cv::Mat obj_hist_, bkg_hist_;
	cv::Mat obj_mask_, bkg_mask_;

	double obj_min_[3], obj_max_[3], bkg_min_[3], bkg_max_[3];
	int hist_size_;
	double smoothness_term_weight_;

	void CalcHist(cv::Mat & mask, cv::Mat & hist, double min[], double max[]);
	void SetDataCost(cv::Point pt);
	void SetSmoothnessCost(cv::Point p, cv::Point q);

public:
	void ExcuteGC();
	cv::Mat graph_cut_label_;
	cv::Mat obj_graph;
	cv::Mat img_with_mask_;

};