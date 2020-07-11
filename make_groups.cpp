#include <iostream>
#include <vector>
#include <cstdio>

using namespace std;

vector<vector<int> > groups;

void print_groups(){
    int i,j;
    for(i=0;i<groups.size();i++){
        for(j=0;j<groups[i].size();j++){
            printf("%d",groups[i][j]);
        }
        printf("\t");
    }
    printf("\n");
}

void rrr(vector<int> currentg,int i,int L,int depth){
    int j;
    if(depth!=0){
        for(j=i;j<=L;j++){
            currentg.push_back(j);
            rrr(currentg,j+1,L,depth-1);
            currentg.pop_back();
        }
    }else{
        groups.push_back(currentg);
    }
}

void make_groups(int L, vector<int> currentg){
    int i;
    if(groups.size()==0){
        for(i=0;i<=L;i++){
            currentg.push_back(i);
            groups.push_back(currentg);
            currentg.clear();
        }
    }
    for(i=2;i<=L;i++){
        rrr(currentg,0,L,i);
    }
}

int main(){
    vector<int> start;
    make_groups(20,start);
    print_groups();
}
