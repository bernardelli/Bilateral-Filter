#include <armadillo>
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "FastBilateral.h"
#define WIN32

template<typename T>
static void Cv_mat_to_arma_mat(const cv::Mat_<T>& cv_mat_in, arma::Mat<T>& arma_mat_out);
template<typename T>
static void Arma_mat_to_cv_mat(const arma::Mat<T>& arma_mat_in, cv::Mat_<T>& cv_mat_out);


int main() {


	cv::Mat image;
	image = cv::imread("len_std.jpg", CV_LOAD_IMAGE_GRAYSCALE);   // Read the file
	
	cv::namedWindow("Original image", cv::WINDOW_AUTOSIZE);// Create a window for display.
	cv::imshow("Original image", image);
	
	if (!image.data)                              // Check for invalid input
	{
		std::cout << "Could not open or find the image" << std::endl;
		getchar();
		return -1;
	}

	

	arma::Mat<int> image_arma;///(image.cols, image.rows);
	Cv_mat_to_arma_mat<int>(image, image_arma);


	FastBilateral fb = FastBilateral(image_arma.n_rows, image_arma.n_cols, 256);

	float sigma_r = 3;
	float sigma_s = 13;
	arma::mat image_bf = fb.filter(image_arma, sigma_r, sigma_s);



/*
	image_arma /= image_arma.max();

	arma::mat kernel;
					kernel	<< 0.0 << -1.0 << 0.0 << arma::endr\
							<< -1.0 << 4.0 << -1.0 << arma::endr\
							<< 0.0 << -1.0 << 0.0<< arma::endr;
	
	arma::mat image_conv_arma_ = arma::conv2(image_arma_float, kernel, "full");
	image_conv_arma_ /= image_conv_arma_.max();
	arma::Mat<float> image_conv_arma = arma::conv_to<arma::Mat<float>>::from(image_conv_arma_);
	cv::Mat_<float> image_filtered;
	Arma_mat_to_cv_mat<float>(image_conv_arma, image_filtered);*/
	//show image with opencv


	cv::Mat_<float> image_filtered;
	Arma_mat_to_cv_mat<float>(arma::conv_to<arma::Mat<float>>::from(image_bf), image_filtered); 
	cv::namedWindow("Filtered image", cv::WINDOW_AUTOSIZE);// Create a window for display.
	cv::imshow("Filtered image", image_filtered);                   // Show our image inside it.
	cv::imwrite("../../images/Result.jpg", image_filtered); //Where is it??
	cv::waitKey(0);
	return 0;
}


template<typename T>
static void Cv_mat_to_arma_mat(const cv::Mat_<T>& cv_mat_in, arma::Mat<T>& arma_mat_out)
{
	cv::Mat_<T> temp(cv_mat_in.t()); //todo any way to not create a temporary?
#if defined(WIN32)
									 //This compiles on VC10 but not g++ 4.9 (on Debian with OpenCV 2.4.9.1 and Arma 4.320.0)
	arma_mat_out = arma::Mat<T>(temp.ptr<T>(), //<<"error: expected primary-expression before '(' token"
		static_cast<arma::uword>(temp.cols),
		static_cast<arma::uword>(temp.rows),
		true,
		true);
#elif defined(LINUX)
									 //This compiles on both but is not as nice
	arma_mat_out = arma::Mat<T>(reinterpret_cast<T*>(temp.data),
		static_cast<arma::uword>(temp.cols),
		static_cast<arma::uword>(temp.rows),
		true,
		true);
#endif
};



//Convert Opencv matrix to armadillo
template<typename T>
static void Arma_mat_to_cv_mat(const arma::Mat<T>& arma_mat_in, cv::Mat_<T>& cv_mat_out)
{
	cv::transpose(cv::Mat_<T>(static_cast<int>(arma_mat_in.n_cols),
		static_cast<int>(arma_mat_in.n_rows),
		const_cast<T*>(arma_mat_in.memptr())),
		cv_mat_out);
};