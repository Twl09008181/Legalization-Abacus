#include<iostream>
#include<vector>
#include<list>
#include <algorithm>
#include "header/abacus.hpp" 


int main()
{

	row row1{18,18,18,4};//x = 18, y = 18, width = 18, height = 4
	std::cout<<row1.subrows.rbegin()->x2<<"\n";


	fixed_node t1{17,0,5,30}; // x = 19~22 , y = 5~35 

	row1.block(t1);

	for(auto sub:row1.subrows)
	{
		std::cout<<sub.x1<<" "<<sub.x2<<"\n";
	}

	


	return 0;
}
