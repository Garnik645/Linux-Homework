#include <iostream>
#include <vector>
#include "complex.h"

int main()
{
	// creating a vector for complex numbers
	std::vector<complex> v;
	v.push_back(complex(1, 2));
	v.push_back(complex(21, -2));
	v.push_back(complex(-235, 423));
	v.push_back(complex(0, 1));
	v.push_back(complex(23, 59));
	v.push_back(complex(-4, 4));
	
	// sorting vector
	sort_complex(v);
	
	// printing vector
	for(complex c : v)
	{
		std::cout << '(' << c.real() << ", " << c.img() << ")\n";
	}
	return 0;
}
