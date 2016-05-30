#pragma once
#include <armadillo>
class FastBilateral
{
public:
	FastBilateral(int x, int y, int eps);
	~FastBilateral();

	arma::mat filter(const arma::mat & image, float sigma_r, float sigma_s);

private:
	arma::mat space;

	void build_space();
	void convolution();
	void nonlinearity();
	void slicing();


};

