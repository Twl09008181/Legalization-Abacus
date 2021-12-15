#ifndef ABACUS_HPP
#define ABACUS_HPP

#include<iostream>
#include<vector>
#include<list>
#include <algorithm>

struct nodes{
	nodes(int y,int x,int w)
		:origin_y{y},origin_x{x},width{w}{}
	int origin_x,origin_y;//global placement result
	int width;
};
struct subrow{
	subrow(int startx,int width)
		: x1{startx},x2{startx + width}{remainSpace = x2-x1;}
	std::list<nodes*>nodes;
	int x1,x2;
	int remainSpace;
};

using fixed_nodes = nodes;
struct row{
	row(int coordinate,int x,int width)
		:y{coordinate}{
		subrows.push_back({x,x + width});
	}
	int y;
	std::list<subrow> subrows;
	//need sorted by y and sort by x(same y).
	void block(std::vector<fixed_nodes>&fixed,int i,int j);
};


#endif