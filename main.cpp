#include<iostream>
#include<vector>
#include<list>
#include <algorithm>
#include "header/abacus.hpp" 


int main()
{

	row row1{18,0,13};


	std::vector<fixed_nodes>terminals; 

	terminals.push_back(fixed_nodes{18,5,4});
	terminals.push_back(fixed_nodes{18,12,1});
	row1.block(terminals,0,2);

	for(auto & sub:row1.subrows)
	{
		std::cout<<sub.x1<<" "<<sub.x2<<"\n";
	}
	


	return 0;
}
