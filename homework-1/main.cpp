#include <iostream>
#include <vector>
#include "complex.h"

int main()
{
	std::vector<complex> v;
	v.push_back(complex(1, 2));
	v.push_back(complex(21, -2));
	v.push_back(complex(-235, 423));
	v.push_back(complex(0, 1));
	v.push_back(complex(23, 59));
	v.push_back(complex(-4, 4));
	sort_complex(v);
	for(complex c : v)
	{
		std::cout << '(' << c.real() << ", " << c.img() << ")\n";
	}
	return 0;
}
