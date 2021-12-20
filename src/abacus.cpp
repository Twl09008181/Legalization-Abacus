#include "../header/abacus.hpp"
#include <algorithm>



// user need to enter terminal with the increasing x order.
// the block function check the relative position between termianl node and place-row , and make the  correction : split the subrows or boundary correction.
void row::block(fixed_node &terminal){
    //pre-checking y range
    if(terminal.origin_y + terminal.height <= y  || terminal.origin_y >= y + height)return ;

    subrow* last = &(*subrows.rbegin());// this is why user need to enter terminal with the increasing x.
    int condition;//overlap condition
    int t_x1 = terminal.origin_x;
    int t_x2 = t_x1 + terminal.width;
    if( (t_x2 <= last->x1 )  || ( t_x1 >= last->x2) )condition = 0;//not overlap in x.
    else if(t_x1 <= last->x1  && t_x2 >=last->x2)condition = 1; // completely ovelap
    else if(t_x1 <= last->x1 && t_x2 < last->x2)condition = 2; // x|xx---|
    else if(t_x1 > last->x1 && t_x2 < last->x2)condition = 3;//|--xxx--| need split
    else if(t_x1 > last->x1 && t_x2 >= last->x2)condition = 4; //|---xx|x 
    if(condition==1)
        subrows.pop_back();//delete subrow
    else if(condition==2)
        last->x1 = t_x2;
    else if(condition==3){//split new subrow
        subrows.push_back({t_x2,last->x2-t_x2,y});
        last = &subrows.at(subrows.size()-2);//push back may allocate new memory
        last->x2 = t_x1;
    }
    else if(condition==4)
        last->x2 = t_x1;
    if(condition > 1) last->remainSpace = last->x2 - last->x1;
}



void subrow::Collapse(){
    cluster& lastC = last();
    lastC.xc = lastC.qc / lastC.ec;
    if(lastC.xc < x1) lastC.xc = x1;
    if(lastC.xc > x2 - lastC.wc)lastC.xc = x2 - lastC.wc;

    int c = clusterNum - 1;
    // check if overlap happend.
    for(;c > 0;c--){
        cluster&cur = Clusters.at(c);
        cluster&pred = Clusters.at(c-1);
        if(pred.xc + pred.wc > cur.xc)
            pred.AddCluster(&cur);
        else
            break;
    }
    clusterNum = c + 1;
}

// modified part : boundary fixed.
int modify_x(int x1,int x2,node*n){
    if(n->origin_x < x1)
        return x1;
    if(n->origin_x + n->width > x2)
        return x2 - n->width;
    return n->origin_x;
}

void subrow::AppendCluster(int x){
    if(Clusters.size()==clusterNum){//need allocate new memmory.
        Clusters.push_back(cluster{x});
    }
    else if(Clusters.size() > clusterNum)//update clusterNum
        Clusters.at(clusterNum) = cluster{x};
    else{
        std::cerr<<"subrow::AppendCluster error, clusterNum > clusters.size()\n";
        exit(1);
    }
    clusterNum++;
}


void subrow::place(node*n){
    int m_x = modify_x(x1,x2,n);// abacus fixed.
    if(empty() || last().xc + last().wc <= m_x){//if do not need place compactly.
        AppendCluster(m_x);//new cluster
        last().AddCell(n);//new cluster add cell
    }
    else{
        last().AddCell(n);
        Collapse();//recursive Collapse
    }
}

int subrow::getPos(){
    int cost = 0;
    for(int c = 0;c < clusterNum;c++){
        cluster& clst = Clusters.at(c);
        int x = clst.xc;
        for(node*n : clst.nodes){
            n->x = x;
            n->y = y;
            cost+=std::abs(n->x - n->origin_x);
            cost+=std::abs(n->y - n->origin_y);
            x += n->width;
        }
    }
    return cost;//total cost
}


int SubRowPlace(subrow*r,node*n){
    r->place(n);
    return r->getPos();
}


int BsSub(std::vector<subrow>&subs,node*n){
    int l = 0;
    int r= subs.size()-1;
    while(l<r)
    {
        int mid = (l+r)/2;
        if(subs.at(mid).x1==n->origin_x)return mid;
        else if(subs.at(mid).x1 > n->origin_x)r = mid-1;
        else l = mid+1;
    }
    return std::max(0,l);
}

bool tryPlace(subrow&sub,node*n,int&bestCost,subrow*&bestp){
    if(sub.remainSpace >= n->width){//still have space
        sub.place(n);
        int afterCost = sub.getPos();
        int dCost = afterCost - sub.cost;//new - original : cost of placing one node.
        sub.RecvoerClusters();
        if(dCost < bestCost){
            bestp = &sub;
            bestCost = dCost;
            return true;
        }
        else 
            return false;
    }
    return true;
}

//first : optimal subrow to place n
//second : delta_cost
std::pair<subrow*,int> row::placeRow(node* n){
    //std::cout<<"placerow\n";
    subrow* p = nullptr;//find a subrow to place p.
    int bestCost = INT_MAX;
    int start = BsSub(subrows,n);
    for(int i = start-1;i<=start+1;i++){
        if(i>=0&&i<subrows.size())
            tryPlace(subrows.at(i),n,bestCost,p);
    }
    /*
    for(int i = start-2;i>=0;i--){
        if(i>=0&&i<subrows.size())
            if(!tryPlace(subrows.at(i),n,bestCost,p))break;
    }
    for(int i = start+2;i<subrows.size();i++){
        if(i>=0&&i<subrows.size())
            if(!tryPlace(subrows.at(i),n,bestCost,p))break;
    }*/
    return {p,bestCost};
}
int row::getCost()
{
    int cost = 0;
    for(auto &sub:subrows)
        cost += sub.getPos();// get final position
    return cost;
}
int row::getRemain()
{
    int space = 0;
    for(auto sub:subrows)space+=sub.remainSpace;
    return space;
}

int binarySearchRow(std::vector<row>&rows,node*n)
{
    int l = 0;
    int r = rows.size()-1;
    while(l<r)
    {
        int mid = (l+r)/2;
        if(rows.at(mid).y==n->origin_y)return mid;
        else if(rows.at(mid).y > n->origin_y)r = mid-1;
        else l = mid+1;
    }
    return std::max(0,l);
}


bool tryPlace2(std::vector<row>&rows,int i,node*n,int&bestCost,subrow*&bestPlace,int&bestRow)
{
    row& r = rows.at(i);
    auto place = r.placeRow(n);
    if(place.first && place.second < bestCost){
        bestCost = place.second;
        bestPlace = place.first;
        bestRow = i;
        return true;
    }
    else if(place.first)
    return false;
    return true;
}

int abacus(std::vector<node*>nodes,std::vector<row>&rows){
    //sort by x
    std::sort(nodes.begin(),nodes.end(),[](node*n1,node*n2){
        if(n1->origin_x==n2->origin_x)
            return n1->width < n2->width; 
        return n1->origin_x < n2->origin_x;
        }
    );

    bool succ = true;
    for(auto n : nodes){
        int bestCost = INT_MAX;
        subrow* bestplace = nullptr;
        int bestRow = -1;
        int startRow = binarySearchRow(rows,n);
        for(int i = startRow-1;i<=startRow+1;i++){
            if(i>=0 && i<rows.size())
                tryPlace2(rows,i,n,bestCost,bestplace,bestRow);
        }
        
        for(int i = startRow-2;i>=0;i--){
            if(i>=0 && i<rows.size())
                if(!tryPlace2(rows,i,n,bestCost,bestplace,bestRow))break;
        }
        for(int i = startRow+2;i<rows.size();i++){
            if(i>=0 && i<rows.size())
                if(!tryPlace2(rows,i,n,bestCost,bestplace,bestRow))break;
        }

        if(bestplace){
            bestplace->place(n);
            bestplace->Backup();
            bestplace->cost = bestplace->getPos();
            bestplace->remainSpace-=n->width;
            if(bestplace->remainSpace<0){ 
                std::cerr<<"abacus remainspace<0!\n";
                exit(1);
            }
        }
        else{
            succ = false;
            break;
        }
    }
    if(succ){
        int cost = 0;
        for(auto &r:rows)
            cost+=r.getCost();
        return cost;
    }
    else{
        std::cout<<"abacus failed\n";
        return -1;
    }
}
