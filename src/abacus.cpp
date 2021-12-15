#include "../header/abacus.hpp"




// user need to check the row y range is covered by this terminal node.
void row::block(fixed_node &terminal){
    //pre-checking y range
    if(terminal.origin_y + terminal.height <= y  || terminal.origin_y >= y + height){
        std::cerr<<"row::block error , error terminal y range !\n";
        std::cerr<<"row range: "<< y <<" to "<< y + height<<"\n";
        std::cerr<<"terminal range: "<< terminal.origin_y << " "<<terminal.origin_y + terminal.height<< "\n";
        exit(1);
    }
    subrow& last = *subrows.rbegin();       
    
    int condition;//overlap condition
    int t_x1 = terminal.origin_x;
    int t_x2 = t_x1 + terminal.width;
    if( (t_x2 <= last.x1 )  || ( t_x1 >= last.x2) )condition = 0;//not overlap in x.
    else if(t_x1 <= last.x1  && t_x2 >=last.x2)condition = 1; // completely ovelap
    else if(t_x1 <= last.x1 && t_x2 < last.x2)condition = 2; // x|xx---|
    else if(t_x1 > last.x1 && t_x2 < last.x2)condition = 3;//|--xxx--| need split
    else if(t_x1 > last.x1 && t_x2 >= last.x2)condition = 4; //|---xx|x 
    if(condition==1)
        subrows.pop_back();//delete subrow
    else if(condition==2)
        last.x1 = t_x2;
    else if(condition==3){//split new subrow
        subrows.push_back({t_x2,last.x2-t_x2});
        last.x2 = t_x1;
    }
    else if(condition==4)
        last.x2 = t_x1;
    if(condition > 1) last.remainSpace = last.x2 - last.x1;
    std::cout<<"condition:"<<condition<<"\n";
}
subrow* row::getSub(node*n){
    auto ptr = subrows.begin();
    auto last = --subrows.end();
    while(ptr!=last && ptr->x1 < n->origin_x)
        ++ptr;
    if(ptr!=subrows.begin())--ptr;

    bool CanFit = ptr->remainSpace >= n->width;
    while(!CanFit && ptr!=last)
    {
        ++ptr;
        if(ptr!= subrows.end())
            CanFit = ptr->remainSpace >= n->width;
    }
    return CanFit ? &(*ptr) : nullptr;
}