#include "../header/abacus.hpp"



void row::block(std::vector<fixed_nodes>&fixed,int i,int j){
    for(int idx = i ; idx < j ;idx++){
        fixed_nodes& terminal = fixed.at(idx);
        //pre-checking
        if(terminal.origin_y!=y){std::cerr<<"row::block error , error terminal coordinate y !\n";exit(1);}
        subrow& last = *subrows.rbegin();       
        if(last.x1 > terminal.origin_x || last.x2 < terminal.origin_x + terminal.width){//can't accommodate.
            std::cerr<<"row ::block error, can't accommodate this terminal node\n";exit(1);
        }
        if(last.x2 > terminal.origin_x+terminal.width)
            subrows.push_back({terminal.origin_x+terminal.width,terminal.width});
        last.x2 = terminal.origin_x;
        last.remainSpace = last.x2 - last.x1;
    }
}