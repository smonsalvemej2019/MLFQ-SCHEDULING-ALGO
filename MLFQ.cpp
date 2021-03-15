#include <iostream>
#include <iomanip>
#include<fstream>
using namespace std;


//------------------------------Node Structure for a single process--------------------------------------// 
struct node{//structure of the node 

    //---------------------------------process variables------------------------------//
    //data array and supplemental
    int * data;//given data array
    int datasize;//size of the array
    int internalCount;//array iterator
    int totaltime;//addition of the array elements (burst+i/o)
    static int globalcount;
    
    //process information 
    string name;//name of the process(P1-P8)
    int contextSwitch;//number of context switches
    int responsetime ;//time the process started
    int turnaroundtime;//time the process ended


    struct node * next;
    struct node * prev;
};

//------------------------------linked list class--------------------------------------// 
class linked_list{
    // structure for the link list that will contain the functions for 
    // creation, traversal, and delition. 
    //set the structs for the head and tail 
    public: 
    node *head,*tail;
    linked_list();//constructor
    void create_node(string name, int data[], int datasize);//create node and inserts it at the end
    node* gethead();//get head method
    node* gettail();//get tail method

    static void detach(node *child, linked_list *parent);//detaches input node from parent list
    static void moveNode(node *from, linked_list *parent, linked_list *target);//moves one node from one list to the other
};
//----------------------------methods definition------------------------------------//

linked_list::linked_list(){//set the head of the node to null automatically 
    head=NULL; 
    tail=NULL; 
}
void linked_list::create_node(string name, int data[], int datasize){//create node function definition

node *tmp = new node;
tmp->name = name;
tmp->datasize = datasize;
tmp->data = data;
tmp->internalCount = 0;
tmp->totaltime = 0;
tmp->contextSwitch = 0;
tmp->responsetime = -1;
tmp->turnaroundtime = -1;
tmp->prev = NULL;
tmp->next = NULL;

if (head == NULL){//creates initial node
    head = tmp;
    tail = tmp;
}else{//adds a node to the tail
    tail->next = tmp;
    tmp->prev = tail;
    tail = tmp;
}
}
node * linked_list::gethead(){return head;}
node * linked_list::gettail(){
    node * tmp;
    tmp = gethead();
    if(tmp == NULL){
        return tmp;
    }else if(tmp->next == NULL){
        return tmp;
    }else
    while(tmp->next != NULL){tmp = tmp->next;}
    tail = tmp;
    return tail;
}
//------------------------Static methods definition------------------------------------//
void linked_list::detach(node *child, linked_list *parent){
    if(child->prev == NULL && child->next != NULL){//if the detaching node is the head (most common at the beginning of the program, applicable to all)
        parent->head = child->next;
        child->next = NULL;
    }else if(child->prev == NULL && child->next == NULL){//if the detaching node is the only node in its parent list(most common at the end of the program)
        parent->head = NULL;
        parent->tail = NULL;
    }else if (child->prev != NULL && child->next == NULL){//if the detaching node is the tail of its parent list(only applicable to i/o)
        parent->tail = NULL;
        child->prev = NULL;
    }else{//if the detaching node node is in the middle of its parent list (only applicable to i/o)
        child->next->prev = child->prev;
        child->prev->next = child->next;
        child->next = NULL;
        child->prev = NULL;
    }
}
void linked_list::moveNode(node *child,linked_list *parent, linked_list *target){
}

//-----------------------------function prototype----------------------------------//
void setup(linked_list *queue0, int **ref, int *sizeOfarr);
//------------------------------driver function------------------------------------//
int main(){
    linked_list cpu;//cpu is a linked list with one node
    linked_list queue0;//priority queue 0 has time quanta of 6
    linked_list queue1;//priority queue 1 has a time quanta of 12
    linked_list queue2;//priority queue 2 is FCFS
    linked_list io;// I/O queue has an iterator 
    linked_list finished;//queue used to display all essential information 
    //---------------------------Processes 1 - 8-----------------------------------//
    int P1[] = {6, 21, 9, 28, 5, 26, 4, 22, 3, 41, 6, 45, 4, 27, 8 , 27, 3};
    int P2[] = {19, 48, 16, 32, 17, 29, 6, 44, 8, 34, 21, 34, 19, 39, 10, 31, 7};
    int P3[] = {12, 14, 6, 21, 3, 29, 7, 45, 8, 54, 11, 44, 9};
    int P4[] = {11, 45, 5, 41, 6, 45, 8, 51, 4, 61, 13, 54, 11, 61, 10};
    int P5[] = {16, 22, 15, 21, 12, 31, 14, 26, 13, 31, 16, 18, 12, 21, 10, 33, 11};
    int P6[] = {20, 31, 22, 30, 25, 29, 11, 44, 17, 34, 18, 31, 6, 22, 16};
    int P7[] = {3, 44, 7, 24, 6, 34, 5, 54, 4, 24, 7, 44, 6, 54, 5, 21, 6, 43, 4};
    int P8[] = {15, 50, 4, 23, 11, 31, 4, 31, 3, 47, 5, 21, 8, 31, 6, 44, 9};
    //-----------------------------------------------------------------------------//
    int *ref[] = {P1,P2,P3,P4,P5,P6,P7,P8};//array of pointers to the process data
    int sizeOfarr[] = {17,17,13,15,17,15,19,17};//array of sizes of the processes arrays
    node::globalcount = 0;//global counter
    setup(&queue0, ref, sizeOfarr);//set up function inputs the array into the data structure
    node *run = cpu.gethead();

    
    while(queue0.gethead() != NULL|| queue1.gethead() != NULL || queue2.gethead() != NULL || io.gethead() != NULL)
    {





    }
}

void setup(linked_list *queue0, int **ref, int *sizeOfarr){
    string names[] = {"P1","P2","P3","P4","P5","P6","P7","P8"};
    for(int i = 0; i<8; i++){
        queue0->create_node(names[i], ref[i], sizeOfarr[i]);
    }
}


