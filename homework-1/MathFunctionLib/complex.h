#ifndef MY_COMPLEX_H
#define MY_COMPLEX_H
#include <vector>

// class representing complex number
class complex
{
private:
	double _real = 0; // real part of the number
	double _img = 0; // imaginary part of the number
public:
	// default constructor, copy constructor, copy assignment
	complex() = default;
	complex(const complex&) = default;
	complex& operator=(const complex&) = default;

	complex(double real, double img);

	double& real(); // returns real part of the complex number
	double& img(); // returns imaginary part of the complex number
	double abs() const; // returns absolute value of the complex number

	// +, - and * operators between two complex numbers
	friend complex operator+(const complex& lhs, const complex& rhs);
	friend complex operator-(const complex& lhs, const complex& rhs);
	friend complex operator*(const complex& lhs, const complex& rhs);

	// * operator with const double
	complex& operator*=(double mul);
};

// function for compering complex numbers
bool complex_comp(const complex& lhs, const complex& rhs);

// sorting function for complex numbers which sorts them by there absolute value
void sort_complex(std::vector<complex>& v);

#endif
