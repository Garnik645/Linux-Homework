#include "complex.h"
#include <cmath>
#include <vector>
#include <algorithm>

complex::complex(double real, double img)
	: _real(real)
	, _img(img)
	{}

double& complex::real()
{
	return _real;
}

double& complex::img()
{
	return _img;
}

double complex::abs() const
{
	return sqrt(std::pow(_real, 2) + std::pow(_img, 2));
}

complex operator+(const complex& lhs, const complex& rhs)
{
	return complex(lhs._real + rhs._real, lhs._img + rhs._img);
}

complex operator-(const complex& lhs, const complex& rhs)
{
	return complex(lhs._real - rhs._real, lhs._img - rhs._img);
}

complex operator*(const complex& lhs, const complex& rhs)
{
	return complex(lhs._real * rhs._real - lhs._img * rhs._img,
		lhs._real * rhs._img + lhs._img * rhs._real);
}

complex& complex::operator*=(double mul)
{
	_real *= mul;
	_img *= mul;
	return *this;
}

bool complex_comp(const complex& lhs, const complex& rhs)
{
	if(lhs.abs() < rhs.abs()) return true;
	else return false;
}

void sort_complex(std::vector<complex>& v)
{
	std::sort(v.begin(), v.end(), complex_comp);
}
