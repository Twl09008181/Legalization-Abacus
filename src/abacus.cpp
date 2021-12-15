#include "../header/abacus.hpp"




// user need to check the row y range is covered by this terminal node.
void row::block(fixed_node &terminal){
    //pre-checking y range
    if(terminal.origin_y + terminal.height <= y  || terminal.origin_y >= y + height){
        std::cerr<<"row::block error , error terminal y range !\n";
        std::cerr<<"row range: "<< y <<" to "<< y + h<<"\n";
        std::cerr<<"terminal range: "<< terminal.origin_y << " "<<terminal.origin_y + terminal.height<< "\n";
        exit(1);
    }
    subrow& last = *subrows.rbegin();       

    int condition;


    //not done

    if( (terminal.origin_x + terminal.width <= last.x1 )  || (terminal.origin_x + terminal.width >= last.x2) )condition = 0;//not overlap in x.
    else if(terminal.origin_x <= last.x1  && terminal.origin_x + terminal.width >=last.x2)condition = 1; // completely ovelap
    else if(terminal.origin_x <= last.x1 && terminal.origin_x + terminal.width <= last.x2)
    



    if(last.x2 > terminal.origin_x+terminal.width){
        int x1 = terminal.origin_x + terminal.width;
        subrows.push_back({x1,last.x2 - x1});
    }
    last.x2 = terminal.origin_x;
    last.remainSpace = last.x2 - last.x1;

    std::cout<<"last x2:"<<last.x2<<"\n";
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