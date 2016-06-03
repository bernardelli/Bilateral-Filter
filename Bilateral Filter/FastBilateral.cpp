#include "FastBilateral.h"
#include <cmath>




FastBilateral::FastBilateral(int x_, int y_, int eps_) :
	x(x_), y(y_), eps(eps_), W_I(arma::cube(x_, y_, eps_)), W(arma::cube(x_, y_, eps_))
{
	std::cout << "Init" << std::endl;
}
FastBilateral::~FastBilateral()
{
}

arma::mat FastBilateral::filter(const arma::Mat<int> & image, float sigma_r, float sigma_s)
{
	std::cout << "Start filter" << std::endl;
	FastBilateral::build_space(image);

	int length_x = floor(sigma_r * 2);
	if (length_x % 2 == 0) {
		length_x++;
	}
	
	std::cout << "Precompute kernels" << std::endl;
	
	arma::mat x_kernel = gaussian1d(sigma_r, length_x);
	arma::mat y_kernel = x_kernel.t();

	x_kernel.print();

	std::cout << "Start convolutions on slices...";
	for (int k = 0; k < eps; k++) {
		//std::cout << k << std::endl;
		arma::mat w_i_slice = arma::conv_to< arma::mat >::from(W_I.slice(k));//x by y matrix
		arma::mat w_i_slice_x = arma::conv2(w_i_slice, x_kernel, "same");
		arma::mat w_i_slice_xy = arma::conv2(w_i_slice, y_kernel, "same");
		W_I.slice(k) = w_i_slice_xy;

		arma::mat w_slice = arma::conv_to< arma::mat >::from(W.slice(k));//x by y matrix
		arma::mat w_slice_x = arma::conv2(w_slice, x_kernel, "same");
		arma::mat w_slice_xy = arma::conv2(w_slice, y_kernel, "same");
		W.slice(k) = w_slice_xy;

	}

	std::cout << " ... done!" << std::endl;
	
	int length_eps = floor(sigma_s * 2);
	if (length_eps % 2 == 0) {
		length_eps++;
	}

	arma::mat eps_kernel_ = gaussian1d(sigma_s, length_eps).t(); //row matrix


	arma::vec eps_kernel = eps_kernel_.col(0); //PROBLEM HERE

	//eps_kernel.print();

	std::cout << "Start convolutions on eps...";
	for (int i = 0; i < x; i++) {
		for (int j = 0; j < y; j++) {
			arma::vec eps_row_w = arma::vec(eps);
			arma::vec eps_row_w_i = arma::vec(eps);
			for (int k = 0; k < eps; k++) {
				eps_row_w(k) = W(i, j, k);
				eps_row_w_i(k) = W_I(i, j, k);
			}
				

			eps_row_w = arma::conv(eps_row_w, eps_kernel, "same");
			eps_row_w_i = arma::conv(eps_row_w_i, eps_kernel, "same");
			for (int k = 0; k < eps; k++) {
				W(i, j, k) = eps_row_w(k);
				W_I(i, j, k) = eps_row_w_i(k);
			}
				

		}
	}
	std::cout << " ... done!" << std::endl;

	std::cout << "Start slicing & nonlinearity .... ";
	//Slincing and Nonlinearity
	arma::mat output_i = arma::mat(x, y);
	for (int i = 0; i < x; i++) {
		for (int j = 0; j < y; j++) {
			int value = image(i, j);
			if (value > 0 && value < 256) {
				if (W(i, j, value) > 0){
					output_i(i, j) = W_I(i, j, value) / W(i, j, value);
				}
				else
					std::cerr << "Attempt divition by zero" << W_I(i, j, value) << "/"<< W(i, j, value) <<std::endl;

			} else
				std::cerr << "ERROR ON THE IMAGE VALUE" << std::endl;
		}
	}
	std::cout << " ... done!" << std::endl;

	return output_i/256;
}


void FastBilateral::build_space(const arma::Mat<int> & image)
{
	W_I.zeros();
	W.zeros();
	for (int i = 0; i < x; i++) {
		for (int j = 0; j < y; j++) {
			int value = image(i, j);
			if (value > 0 && value < 256) {
				W_I(i, j, value) = value;
				W(i, j, value) = 1;
			}
			else
				std::cerr << "ERROR ON THE IMAGE VALUE" << std::endl;
		}
	}
}
void FastBilateral::convolution()
{
}
void FastBilateral::nonlinearity()
{
}
void FastBilateral::slicing()
{
}

arma::mat FastBilateral::gaussian1d(float sigma, int length) {

	arma::mat kernel(1, length);

	for (int i = 0; i < length; i++) {
		kernel(i) = exp(-0.5*pow((length / 2 - i) / sigma,2));
	}

	kernel /= accu(kernel);

	return kernel;
}