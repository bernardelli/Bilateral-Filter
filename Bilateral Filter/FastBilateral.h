#pragma once
#include <armadillo>
#include <iostream>
class FastBilateral
{
public:
	FastBilateral(int x, int y, int eps);
	~FastBilateral();
	int x, y, eps;
	arma::cube W_I;
	arma::cube W;

	arma::mat filter(const arma::Mat<int> & image, float sigma_r, float sigma_s);

private:
	arma::mat space;

	arma::mat gaussian1d(float sigma, int length);
	void build_space(const arma::Mat<int> & image);
	void convolution();
	void nonlinearity();
	void slicing();


};

