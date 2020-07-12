#include <stdio.h>
#include <iostream>
#include <cstdlib>
#include <vector>
#include <ctime>
#include <algorithm>

#define decksize 106
#define handsize 14

using namespace std;
//NOTATION
//subset: any random arrangement of cards
//group: a straight or 3-of-a-kind or 4-of-a-kind


//This is the recursive function that supposedly does it all
bool do_stuff(int d_s,int l,int i,vector<int>& ss,vector<int>& skip,vector<int>& newss,vector<int>& stackmonster);

//most of these functions are helpers in case i want to simulate the full game 
void printcard(int card,bool newline){
	char* suit="CHSDCHSD";
	char* num="A234567890JQK";
	if(card==-1){
		printf("## ");
	}else{
		printf("%c", suit[card/13]);
		printf("%c ", num[card%13]);
	}
	if(newline){
		printf("\n");
	}
}

void printdeck(vector<int>& deck){
	int i;
	for (i=0;i<deck.size();i++){
		printcard(deck[i], (i+1)%14==0);
	}
}

void printhand(vector<int>& hand){
	int i;
	for (i=0;i<hand.size();i++){
		printcard(hand[i], (i+1)%hand.size()==0);
	}
}

void filldeck(vector<int>& deck){
	deck.resize(decksize);
	deck[0]=-1;
	deck[1]=-1;
	int i;
	for(i=2;i<decksize;i++){
		deck[i]=i-2;
	}
}

void shuffledeck(vector<int>& deck/*,vector<int>& scarts*/){
	int i, i1, i2, temp;
	for (i=0;i<3*deck.size();i++){
		i1=rand()%deck.size();
		i2=rand()%deck.size();
        //printf("Swapping %d with %d\n",i1,i2);
		if(i1!=i2){
			temp=deck[i1];
			deck[i1]=deck[i2];
			deck[i2]=temp;
		}
	}
    //scarts.push_back(deck[0]);
    //deck.erase(deck.begin());
}

void deal(vector<int>& deck,vector<int>& hand){
    int i;
    for(i=0;i<handsize;i++){
        hand.push_back(deck[0]);
        deck.erase(deck.begin());
    }
}

void get_new(vector<int>& deck,vector<int>& hand){
    hand.push_back(deck[0]);
    deck.erase(deck.begin());
}

void get_scart(vector<int>& scarts,vector<int>& hand){
    hand.push_back(*scarts.end());
    scarts.pop_back();
}

void set_scart(std::vector<int>& hand,std::vector<int>& scarts,int cardpos){
    vector<int>::iterator c=hand.begin()+cardpos;
    scarts.push_back(*c);
    hand.erase(c);
}

//this is the function that is called many many times while checking if some subset of cards form a group
bool is_a_group(vector<int> nums){
    //printf("Sorting: ");//im sorry what
    sort(nums.begin(),nums.end());//is this really nescessary
    //printhand(nums);

    if((nums.size()<3)||(nums.size()>13)){
        printf("Checking sth trivial (size = %d)\n",nums.size());
        return false;
    }

    //straight
    bool straight=true;
    //printf(" - Checking if straight...\n");
    for(int i=1;i<nums.size();i++){
        //printf("\tChecking %d against %d \n",nums[i]%52,(nums[i-1]+1)%52);
        if((nums[i]%52!=(nums[i-1]+1)%52)||(nums[i]%13==0)      &&((nums[i]!=-1)&&(nums[i-1]!=-1))){
            straight=false;
            //printf("Not ok\n");
            fflush(stdout);
            break;
        }
    }
    if(straight){
        //printf("Its straight!");
        fflush(stdout);
        return true;
    }

    //of-a-kind
    bool ofkind=true;
    //printf(" - Checking if gay...\n");
    if(nums.size()>4){
        ofkind=false;
    }
    else{
        for(int i=1;i<nums.size();i++){
            if ((nums[i]%13!=nums[i-1]%13)||(nums[i]%52==nums[i-1]%52)      &&((nums[i]!=-1)&&(nums[i-1]!=-1))){
                ofkind=false;
                break;
            }
        }
    }
    if(nums.size()==4){
        if((nums[1]%52==nums[3]%52)||(nums[0]%52==nums[3]%52)){
            ofkind=false;
        }
    }else{
        if(nums[0]%52==nums[2]%52){
            ofkind=false;
        }
    }
    return ofkind;
}

bool is_double(int n1, int n2){//checks if 2 card go together

    if(n1>n2){
        int temp=n1;
        n1=n2;
        n2=temp;
    }

    //straight - this optimize
    bool straight=true;
    if((n2%52!=(n1+1)%52)     &&((n1!=-1)&&(n2!=-1))){
        straight=false;
    }
    if(straight){
        return true;
    }

    //of-a-kind - this can be optimized
    bool ofkind=true;
    if (((n2%13!=n1%13)||(n1%52==n2%52))        &&((n1!=-1)&&(n2!=-1))){
        ofkind=false;
    }
    return ofkind;
}

void next_enum(vector<int>& start,int limit){
    //e.g limit=3
    //start=01
    //01
    //02
    //03
    //12
    //13
    //23
    //012
    //013
    //023
    //123

    int pos=start.size()-1;
    int l=limit;
    bool ended=false;

    for (int i=pos;i>=0;i--){
        if(start[i]==l){
            l--;
        }else{
            ended=true;
            break;
        }
    }

    if((ended)&&(start.size()<limit)){
        start.push_back(start.size());
        pos=start.size()-1;
        for (int i=pos;i>=0;i--){
            start[i]=start[i+1]-1;
        }
    }

    while(pos>0){
        start[pos]++;
        if(start[pos]>limit){
            pos--;
            start[pos]++;
            start[pos+1]=start[pos]+1;
        }
        return;
    }
}

//This is old, just saving it for reference
bool is_subset_valid(vector<int> ss){//ss stands for subset
    //YOU ARE VALID
    //for any group of 2+ cards do we have at least 1 that can join them
    //for any join check the subset that remains recursively.
    printf("Increasing stack...");
    int i;
    int j;
    int k,m;
    vector<int> skip;
    vector<int> newss;
    vector<int> stackmonster;

    if(is_a_group(ss)){
        //printf("EXIT STATEMENT!!!!!!! (insetr a nimber to continu)");
        return true;
    }
    else if(ss.size()<=2){
        return false;
    }
    else{
        printf("Doing stuff\n");
        return do_stuff(0,2,-1,ss,skip,newss,stackmonster);
    }
    printf("Decreasing stack...");
    return false;
}

bool check_subset(vector<int> ss,bool printstatus){
    //the goal is to cut the subset into smaller subsets and check those.
    //the problem is how to do it without being O(n^n)

    //for any set of 2+ cards do we have at least 1 that can join them
    //for any join check the subset that remains recursively.

    //find 2 cards that go together (O(n^2))
    int i,j,k,l,m,n;
    if(ss.size()==0){//all cards have been made into groups
        return true;
    }
    if(ss.size()<=2){//left with 1 or 2 cards, cannot make another group
        printf("cannot make another group with this config\n");
        return false;
    }

    vector<int> current_group;
    vector<int> reduced_ss;
    current_group.reserve(5);
    reduced_ss.reserve(ss.size());

    bool found_group=true;
    for(i=0;(i<ss.size()-1) && (found_group);i++){
        if(printstatus){
            printf("\r%d",i);
        }
        found_group=false;//assume no group found, so that if i dont find something we just exit.
        for(j=i+1;j<ss.size();j++){
            if (is_double(ss[i],ss[j])){
                //ok now you have 2 cards, you need to find 3,4,5 (more than 5 is just 3+3)...
                current_group.push_back(ss[i]);
                current_group.push_back(ss[j]);
                for(k=0;k<ss.size();k++){
                    if(k!=i&&k!=j){
                        current_group.push_back(ss[k]);
                        if(is_a_group(current_group)){//found 3 cards
                            found_group=true;
                            for(n=0;n<ss.size();n++){//make the reduced subset
                                if(n!=i&&n!=j&&n!=k){
                                    reduced_ss.push_back(ss[n]);
                                }
                            }
                            if(check_subset(reduced_ss,false)){
                                printhand(current_group);
                                printf("\n");
                                return true;
                            }//recursive step
                            reduced_ss.clear();

                            for(l=0;l<ss.size();l++){
                                if(l!=i&&l!=j&&l!=k){
                                    current_group.push_back(ss[l]);
                                    if(is_a_group(current_group)){//found 4 cards

                                        for(n=0;n<ss.size();n++){//make the reduced subset
                                            if(n!=i&&n!=j&&n!=k&&n!=l){
                                                reduced_ss.push_back(ss[n]);
                                            }
                                        }
                                        if(check_subset(reduced_ss,false)){
                                            printhand(current_group);
                                            printf("\n");
                                            return true;
                                        }//recursive step
                                        reduced_ss.clear();

                                        for(l=0;l<ss.size();l++){
                                            if(m!=i&&m!=j&&m!=k&&m!=l){
                                                current_group.push_back(ss[m]);
                                                if(is_a_group(current_group)){//found 5 cards
                                                    for(n=0;n<ss.size();n++){//make the reduced subset
                                                        if(n!=i&&n!=j&&n!=k&&n!=l&&n!=m){
                                                            reduced_ss.push_back(ss[n]);
                                                        }
                                                    }
                                                    if(check_subset(reduced_ss,false)){
                                                        printhand(current_group);
                                                        printf("\n");
                                                        return true;
                                                    }//recursive step
                                                    reduced_ss.clear();
                                                }
                                                current_group.pop_back();
                                            }
                                        }
                                    }
                                    current_group.pop_back();
                                }
                            }
                        }
                        current_group.pop_back();
                    }
                }
                current_group.clear();
            }
        }
    }
    if(printstatus)
        printf("\n");
    //printf("Full recursion ended\n");
    return false;//could not go to a configuration where 0 cards are left.

}

int main(){
    setvbuf (stdout, NULL, _IONBF, BUFSIZ);//make printf print immediately
	int seed=1233;
    vector<int> deck,hand1,hand2,scarts;
    srand(time(NULL));

	printf("Hello. Remember 0 is 10, ## is joker. thx\n");
    vector<int> test;

    int i;
    //TODO: implement jokers
    //TODO: read this from a file
    for(i=1;i<54;i++){
        filldeck(test);
        //shuffledeck(test);
        test.resize(i);
        sort(test.begin(),test.end());

        printf("Testing card set (%d cards): ",i);
        printhand(test);
        //printf(check_subset(test)?"CAN BE DONE":"CANNOT BE DONE");
        if(check_subset(test,true)){
            break;
        }
        printf("\n");
        test.clear();
    }
}

//this is old, saving it for reference.
bool do_stuff(int d_s,int l,int i,vector<int>& ss,vector<int>& skip,vector<int>& newss,vector<int>& stackmonster){
    int m;
    //if(i>ss.size()){
    //    printf("i = %d, size = %d\n",i,ss.size());
//        printf("i>size: %s",i>ss.size()?"yes\n":"no\n");
//        return false;
//    }
    if(d_s>l){
        printf("ds=%d, l=%d\n",d_s,l);
        return false;
    }
    printf("doing stuff\n");
    fflush(stdout);
    for(int j=i+1;j<ss.size();j++){
        stackmonster.push_back(ss[j]);
        skip.push_back(j);
        return do_stuff(d_s+1,l,j,ss,skip,newss,stackmonster);
        if(is_a_group(stackmonster)){
            printf("It's a group!\n");
            newss.clear();
            sort(skip.begin(),skip.end());
            m=0;
            printf("Skips: ");
            for(int l=0;l<skip.size();l++){
                printf("%d ",skip[l]);
            }
            for(int l=0;l<ss.size();l++){
                if(l!=skip[m]){
                    newss.push_back(ss[l]);
                }else{
                    ++m;
                }
            }
            printf("\tStack monster: ");
            printhand(stackmonster);
            printf("\tNew ss: ");
            printhand(newss);
            printf("\tOld ss: ");
            printhand(ss);

            if(is_subset_valid(newss)){
                return true;
            }
        }
        stackmonster.pop_back();
        skip.pop_back();
    }
}

