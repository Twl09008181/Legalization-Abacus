#include<iostream>
#include<vector>
#include<list>
#include <algorithm>
#include "header/abacus.hpp" 


int main()
{

	row row1{18,18,18,4};//x = 18, y = 18, width = 18, height = 4


	fixed_node t0{16,0,2,40}; // not overlap
	fixed_node t1{18,0,1,30}; // x = 18~19 , y = 0~30 
	fixed_node t2{24,0,5,30}; // x = 19~24 , y = 5~35 
	fixed_node t3{30,0,2,40};
	fixed_node t4{34,0,10,40};
	fixed_node t5{40,0,10,40};

	// ** need block according to increasing x. **
	row1.block(t0);//not overlap
	for(auto sub:row1.subrows){std::cout<<"["<<sub.x1<<" "<<sub.x2<<"] ";}std::cout<<"\n";
	row1.block(t1);//cut to 19~36
	for(auto sub:row1.subrows){std::cout<<"["<<sub.x1<<" "<<sub.x2<<"] ";}std::cout<<"\n";
	row1.block(t2);//cut to 19~24 29~36
	for(auto sub:row1.subrows){std::cout<<"["<<sub.x1<<" "<<sub.x2<<"] ";}std::cout<<"\n";
	row1.block(t3);//cut to 19~24 29~30 32~36
	for(auto sub:row1.subrows){std::cout<<"["<<sub.x1<<" "<<sub.x2<<"] ";}std::cout<<"\n";
	row1.block(t4);//cut to 19~24 29~30 32~34
	for(auto sub:row1.subrows){std::cout<<"["<<sub.x1<<" "<<sub.x2<<"] ";}std::cout<<"\n";
	row1.block(t5);//not overlap
	for(auto sub:row1.subrows){std::cout<<"["<<sub.x1<<" "<<sub.x2<<"] ";}std::cout<<"\n";

	


	return 0;
}
