#include "FastBilateral.h"
#include <cmath>




FastBilateral::FastBilateral(int x_, int y_, int eps_) :
	x(x_), y(y_), W_I(arma::Cube<int>(x_, y_, eps_)), W(arma::Cube<int>(x_, y_, eps_))
{
	
}
FastBilateral::~FastBilateral()
{
}

arma::mat FastBilateral::filter(const arma::Mat<int> & image, float sigma_r, float sigma_s)
{
	FastBilateral::build_space(image);

	int length_x = floor(sigma_r * 6);
	if (length_x % 2 == 0) {
		length_x++;
	}
		
	arma::mat x_kernel = gaussian1d(sigma_r, length_x);
	arma::mat y_kernel = x_kernel.t();

	for (int k = 0; k > eps; k++) {
		arma::mat w_i_slice = arma::conv_to< arma::mat >::from(W_I.slice(k));//x by y matrix
		arma::mat w_i_slice_x = arma::conv2(w_i_slice, x_kernel, "same");
		arma::mat w_i_slice_xy = arma::conv2(w_i_slice, y_kernel, "same");
		W_I.slice(k).set_real(arma::conv_to< arma::Mat<int> >::from(w_i_slice_xy));

		arma::mat w_slice = arma::conv_to< arma::mat >::from(W.slice(k));//x by y matrix
		arma::mat w_slice_x = arma::conv2(w_slice, x_kernel, "same");
		arma::mat w_slice_xy = arma::conv2(w_slice, y_kernel, "same");
		W.slice(k).set_real(arma::conv_to< arma::Mat<int> >::from(w_slice_xy));

	}
	
	int length_eps = floor(sigma_s * 6);
	if (length_eps % 2 == 0) {
		length_eps++;
	}

	arma::mat eps_kernel_ = gaussian1d(sigma_s, length_eps); //row matrix
	arma::vec eps_kernel = eps_kernel.row(0);

	for (int i = 0; i < x; i++) {
		for (int j = 0; j < y; y++) {
			arma::vec eps_row_w = arma::vec(eps);
			arma::vec eps_row_w_i = arma::vec(eps);
			for (int k = 0; k < eps; k++) {
				eps_row_w(k) = W.at(i, j, k);
				eps_row_w_i(k) = W_I.at(i, j, k);
			}
				

			eps_row_w = arma::conv(eps_row_w, eps_kernel, "same");
			eps_row_w_i = arma::conv(eps_row_w_i, eps_kernel, "same");
			for (int k = 0; k < eps; k++) {
				W.at(i, j, k) = eps_row_w(k);
				W_I.at(i, j, k) = eps_row_w_i(k);
			}
				

		}
	}
	
	//Slincing and Nonlinearity
	arma::mat output_i = arma::mat(x, y);
	for (int i = 0; i < x; i++) {
		for (int j = 0; j < y; j++) {
			int value = image(i, j);
			if (value > 0 && value < 256)
				output_i(i, j) = W_I(i, j, value) / W(i, j, value);
			else
				std::cerr << "ERROR ON THE IMAGE VALUE" << std::endl;
		}
	}

	return output_i;
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

	kernel /= sqrt(2 * arma::datum::pi)*sigma;

	return kernel;
}