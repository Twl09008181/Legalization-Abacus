#include<iostream>
#include<vector>
#include<list>
#include <algorithm>
#include "header/abacus.hpp" 


int main()
{

	row row1{18,0,13};


	std::vector<fixed_node>terminals; 

	terminals.push_back(fixed_node{18,3});
	terminals.push_back(fixed_node{18,12});
	row1.block(terminals,0,2);

	for(auto & sub:row1.subrows)
	{
		std::cout<<sub.x1<<" "<<sub.x2<<"\n";
	}

	
	node n{18,6};
	n.setShape(2,4);

	subrow* sub = row1.getSub(&n);

	std::cout<<sub->x1<<" "<<sub->x2<<"\n";


	return 0;
}
