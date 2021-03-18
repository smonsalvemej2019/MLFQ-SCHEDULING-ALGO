#include <iostream>
#include <iomanip>
#include <fstream>
using namespace std;
//------------------------------Node Structure for a single process--------------------------------------//
struct node
{ //structure of the node

    //---------------------------------process variables------------------------------//
    //data array and supplemental
    int *data;              //given data array
    int datasize;           //size of the array
    int iterator;           //array iterator
    int totaltime;          //addition of the array elements (burst+i/o)
    int clock;              //keeps track of the time quanta used for round robin
    static int globalcount; //global counter that keeps track of the overall time

    //process information
    string name;        //name of the process(P1-P8)
    int contextSwitch;  //number of context switches
    int responsetime;   //time the process started
    int turnaroundtime; //time the process ended
    int parentqueue;    //the queue where this process belongs to (will increase +1 if the cpu does not finish its burst)
    int nextqueue;      //next queue where the process is going (wait(q1,q2,q3)->cpu->I/O)
    static int idle;    //keeps track of the idle time in cpu

    struct node *next;
    struct node *prev;
};

//------------------------------linked list class--------------------------------------//
class linked_list
{
    // structure for the link list that will contain the functions for
    // creation, traversal, and deletion.
    //set the structs for the head and tail
public:
    node *head, *tail;
    linked_list();                                           //constructor
    void create_node(string name, int data[], int datasize); //create node and inserts it at the end
    node *gethead();                                         //get head method
    node *gettail();                                         //get tail method

    static void detach(node *child, linked_list *parent);                          //detaches input node from parent list
    static void removeNode(node *from, linked_list *parent, linked_list **target); //moves one node from cpu to the parent list
    static void addNode(node *child, linked_list *parent, linked_list *cpu);       //moves node from parent list to cpu
    void runCPU();
    void runio(linked_list **queues);
};

//----------------------------methods definition------------------------------------//
int node::globalcount;
int node::idle;
linked_list::linked_list()
{ //set the head of the node to null automatically
    head = NULL;
    tail = NULL;
}
void linked_list::create_node(string name, int data[], int datasize)
{ //create node function definition

    node *tmp = new node;
    tmp->name = name;
    tmp->parentqueue = 1;
    tmp->nextqueue = 0;
    tmp->datasize = datasize;
    tmp->data = data;
    tmp->clock = 0;
    tmp->iterator = 0;
    tmp->totaltime = 0;
    tmp->contextSwitch = 0;
    tmp->responsetime = -1;
    tmp->turnaroundtime = -1;
    tmp->prev = NULL;
    tmp->next = NULL;

    if (head == NULL)
    { //creates initial node
        head = tmp;
        tail = tmp;
    }
    else
    { //adds a node to the tail
        tail->next = tmp;
        tmp->prev = tail;
        tail = tmp;
    }
}
node *linked_list::gethead() { return head; }
node *linked_list::gettail()
{
    node *tmp;
    tmp = gethead();
    if (tmp == NULL)
    {
        return tmp;
    }
    else if (tmp->next == NULL)
    {
        return tmp;
    }
    else
        while (tmp->next != NULL)
        {
            tmp = tmp->next;
        }

    return tmp;
}
void linked_list::runCPU()
{

    if (this->gethead()->responsetime < 0)
    {
        this->gethead()->responsetime = node::globalcount;
    }

    switch (this->gethead()->parentqueue)
    {
    case 1: //if the process belongs to queue 1
        this->gethead()->data[this->gethead()->iterator]--;
        this->gethead()->totaltime++;
        this->gethead()->clock++;
        if (this->gethead()->data[this->gethead()->iterator] == 0 && this->gethead()->clock <= 6)
        { //if the process finishes its burst when or before the clock runs out
            this->gethead()->clock = 0;
            this->gethead()->iterator++;
            if (this->gethead()->nextqueue != 5)
            {
                this->gethead()->nextqueue = 4;
            }
        }
        else if (this->gethead()->data[this->gethead()->iterator] > 0 && this->gethead()->clock == 6)
        {
            this->gethead()->clock = 0;
            this->gethead()->parentqueue++;
            if (this->gethead()->nextqueue != 5)
            {
                this->gethead()->nextqueue = this->gethead()->parentqueue;
            }
        }
        break;
    case 2: //if the process belongs to queue 2
        this->gethead()->data[this->gethead()->iterator]--;
        this->gethead()->totaltime++;
        this->gethead()->clock++;
        if (this->gethead()->data[this->gethead()->iterator] == 0 && this->gethead()->clock <= 12)
        {
            this->gethead()->clock = 0;
            this->gethead()->iterator++;
            if (this->gethead()->nextqueue != 5)
            {
                this->gethead()->nextqueue = 4;
            }
        }
        else if (this->gethead()->data[this->gethead()->iterator] > 0 && this->gethead()->clock == 12)
        {
            this->gethead()->clock = 0;
            this->gethead()->parentqueue++;
            if (this->gethead()->nextqueue != 5)
            {
                this->gethead()->nextqueue = this->gethead()->parentqueue;
            }
        }
        break;
    case 3: //if the process belongs to queue 3
        this->gethead()->data[this->gethead()->iterator]--;
        this->gethead()->totaltime++;
        if (this->gethead()->data[this->gethead()->iterator] == 0)
        {

            if (this->gethead()->nextqueue != 5)
            {
                this->gethead()->nextqueue = 4;
                this->gethead()->iterator++;
            }
        }
    }
    if (this->gethead()->iterator == this->gethead()->datasize)
    {
        this->gethead()->turnaroundtime = node::globalcount;
        this->gethead()->nextqueue = 5;
    }
}
void linked_list::runio(linked_list **queues)
{
    node *temp = this->gethead();
    node *temp2 = NULL;
    while (temp != NULL)
    {
        temp->data[temp->iterator]--;
        temp->totaltime++;
        if (temp->data[temp->iterator] == 0)
        {
            temp->iterator++;
            temp->nextqueue = temp->parentqueue;
            temp2 = temp;
            temp = temp->next;
            linked_list::removeNode(temp2, this, queues);
        }
        else
        {
            temp = temp->next;
        }
    }
}
//------------------------Static methods definition------------------------------------//
void linked_list::detach(node *child, linked_list *parent)
{
    if (child->prev == NULL && child->next != NULL)
    { //if the detaching node is the head (most common at the beginning of the program, applicable to all)
        parent->head = child->next;
        parent->head->prev = NULL;
        child->next = NULL;
        return;
    }
    else if (child->prev == NULL && child->next == NULL)
    { //if the detaching node is the only node in its parent list(most common at the end of the program)
        parent->head = NULL;
        parent->tail = NULL;
        return;
    }
    else if (child->prev != NULL && child->next == NULL)
    { //if the detaching node is the tail of its parent list(only applicable to i/o)
        parent->tail = child->prev;
        parent->tail->next = NULL;
        child->prev = NULL;
        return;
    }
    else
    { //if the detaching node node is in the middle of its parent list (only applicable to i/o)
        child->next->prev = child->prev;
        child->prev->next = child->next;
        child->next = NULL;
        child->prev = NULL;
        return;
    }
}
void linked_list::removeNode(node *child, linked_list *parent, linked_list **target)
{
    detach(child, parent);

    if (target[child->nextqueue]->gethead() == NULL)
    {
        target[child->nextqueue]->head = child;
        target[child->nextqueue]->tail = target[child->nextqueue]->head;
    }
    else
    {
        node *tmp = target[child->nextqueue]->gettail();
        tmp->next = child;
        child->prev = tmp;
        target[child->nextqueue]->tail = target[child->nextqueue]->gettail();
    }
}
void linked_list::addNode(node *child, linked_list *parent, linked_list *cpu)
{
    detach(child, parent);
    cpu->head = child;
    cpu->tail = cpu->head;
}

//-----------------------------function prototype----------------------------------//
void setup(linked_list *queue0, int **ref, int *sizeOfarr);  //initial set up function
void displayCurrent(linked_list **queues, ofstream &output); //information output function
void getAverages(linked_list **queues, ofstream &output);    //final information output function
//------------------------------driver function------------------------------------//
int main()
{
    ofstream output;
    output.open("output.txt");
    linked_list cpu;       //cpu is a linked list with one node
    linked_list queue1;    //priority queue 0 has time quanta of 6
    linked_list queue2;    //priority queue 1 has a time quanta of 12
    linked_list queue3;    //priority queue 2 is FCFS
    linked_list io;        // I/O queue has an iterator
    linked_list finished;  //queue used to display all essential information
    node::idle = 0;        //cpu idle counter
    node::globalcount = 0; //global counter
    //----------------------Processes 1 - 8 --------------------------------------//
    int P1[] = {6, 21, 9, 28, 5, 26, 4, 22, 3, 41, 6, 45, 4, 27, 8, 27, 3};
    int P2[] = {19, 48, 16, 32, 17, 29, 6, 44, 8, 34, 21, 34, 19, 39, 10, 31, 7};
    int P3[] = {12, 14, 6, 21, 3, 29, 7, 45, 8, 54, 11, 44, 9};
    int P4[] = {11, 45, 5, 41, 6, 45, 8, 51, 4, 61, 13, 54, 11, 61, 10};
    int P5[] = {16, 22, 15, 21, 12, 31, 14, 26, 13, 31, 16, 18, 12, 21, 10, 33, 11};
    int P6[] = {20, 31, 22, 30, 25, 29, 11, 44, 17, 34, 18, 31, 6, 22, 16};
    int P7[] = {3, 44, 7, 24, 6, 34, 5, 54, 4, 24, 7, 44, 6, 54, 5, 21, 6, 43, 4};
    int P8[] = {15, 50, 4, 23, 11, 31, 4, 31, 3, 47, 5, 21, 8, 31, 6, 44, 9};
    //-----------------------------supplemental----------------------------------------//
    int sizeOfarr[] = {17, 17, 13, 15, 17, 15, 19, 17};                        //array of sizes of the processes arrays
    int *ref[] = {P1, P2, P3, P4, P5, P6, P7, P8};                             //array of pointers to the process data
    linked_list *queues[] = {&cpu, &queue1, &queue2, &queue3, &io, &finished}; //array containing each queue
    //------------------------------Initial set up-------------------------------------//
    setup(&queue1, ref, sizeOfarr);          //set up function inputs the array into the data structure
    node *q1 = queue1.gethead();             //sets a temp node to the head of queue 1
    linked_list::addNode(q1, &queue1, &cpu); //moves the head of queue 1 to the cpu
    q1->responsetime = node::globalcount;    //set response time to 0
    displayCurrent(queues, output);          //first display of data
    //------------------------------Driver loop-----------------------------------------//
    while (queue1.gethead() != NULL || queue2.gethead() != NULL || queue3.gethead() != NULL || io.gethead() != NULL || cpu.gethead() != NULL)
    {

        if (queue1.gethead() != NULL && queue1.gethead()->nextqueue != 0)
        {
            queue1.gethead()->nextqueue = 0;
        }
        if (queue2.gethead() != NULL && queue2.gethead()->nextqueue != 0)
        {
            queue2.gethead()->nextqueue = 0;
        }
        if (queue3.gethead() != NULL && queue3.gethead()->nextqueue != 0)
        {
            queue3.gethead()->nextqueue = 0;
        }

        if (cpu.gethead() != NULL)
        {
            cpu.runCPU();
            if (cpu.gethead()->nextqueue == 4)
            {
                cpu.gethead()->contextSwitch++;
                linked_list::removeNode(cpu.gethead(), &cpu, queues);
                if (!queue1.gethead() && !queue2.gethead() && !queue3.gethead() && io.gethead())
                {
                    displayCurrent(queues, output);
                }
            }
            else if (cpu.gethead()->nextqueue == 5)
            {
                cpu.gethead()->contextSwitch++;
                linked_list::removeNode(cpu.gethead(), &cpu, queues);
                if (!queue1.gethead() && !queue2.gethead() && !queue3.gethead() && io.gethead())
                {
                    displayCurrent(queues, output);
                }
            }
            else
            {
                cpu.gethead()->contextSwitch++;
                linked_list::removeNode(cpu.gethead(), &cpu, queues);
            }
        }
        else
        {
            node::idle++;
        }

        node::globalcount++;

        if ((cpu.gethead() == NULL || cpu.gethead()->parentqueue > 1) && queue1.gethead() != NULL)
        { //if the cpu is empty or if the process in cpu can be preempted and queue 1 is not empty
            if (cpu.gethead() != NULL)
            { //preempt
                cpu.gethead()->nextqueue = cpu.gethead()->parentqueue;
                cpu.gethead()->contextSwitch++;
                linked_list::removeNode(cpu.gethead(), &cpu, queues);
                linked_list::removeNode(queue1.gethead(), &queue1, queues);
                displayCurrent(queues, output);
            }
            else
            { //move process from q1 to cpu if empty
                linked_list::removeNode(queue1.gethead(), &queue1, queues);
                displayCurrent(queues, output);
            }
        }
        else if ((cpu.gethead() == NULL || cpu.gethead()->parentqueue > 2) && queue2.gethead() != NULL)
        { //if the cpu is empty or if the process in cpu can be preempted and queue 2 is not empty
            if (cpu.gethead() != NULL)
            { //preempt
                cpu.gethead()->nextqueue = cpu.gethead()->parentqueue;
                cpu.gethead()->contextSwitch++;
                linked_list::removeNode(cpu.gethead(), &cpu, queues);
                linked_list::removeNode(queue2.gethead(), &queue2, queues);
                displayCurrent(queues, output);
            }
            else
            { //move process from q2 to cpu if empty
                linked_list::removeNode(queue2.gethead(), &queue2, queues);
                displayCurrent(queues, output);
            }
        }
        else if (cpu.gethead() == NULL && queue3.gethead() != NULL)
        { //moves process from q3 if cpu is empty
            linked_list::removeNode(queue3.gethead(), &queue3, queues);
            displayCurrent(queues, output);
        }

        if (io.gethead() != NULL)
        {
            io.runio(queues);
        }
    }
    displayCurrent(queues, output);
    getAverages(queues, output);
}

void setup(linked_list *queue0, int **ref, int *sizeOfarr)
{
    string names[] = {"P1", "P2", "P3", "P4", "P5", "P6", "P7", "P8"};
    for (int i = 0; i < 8; i++)
    {
        queue0->create_node(names[i], ref[i], sizeOfarr[i]);
    }
}
void displayCurrent(linked_list **queues, ofstream &output)
{

    output << "\n\n\n\n"
           << setw(30) << right << "Current Time: " << node::globalcount;
    output << "\n----------------------------------------";

    if (queues[0]->gethead() == NULL)
    {
        output << "\n"
               << setw(20) << right << "CPU is currently IDLE";
    }
    else
    {
        output << "\n"
               << setw(20) << right << "Currently in CPU" 
               << setw(20) << right << "Burst";
        output << "\n\n"
               << setw(20) << right << queues[0]->gethead()->name 
               << setw(20) << right << queues[0]->gethead()->data[queues[0]->gethead()->iterator];
    }
    output << "\n----------------------------------------";
    if (queues[1]->gethead() == NULL)
    {
        output << "\n"
               << setw(20) << right << "Priority queue 1 is empty";
    }
    else
    {
        output << "\n"
               << setw(20) << right << "Currently Waiting";
        output << "\n"
               << setw(20) << right << "Process" 
               << setw(20) << right << "Burst";
        node *temp = queues[1]->gethead();
        while (temp != NULL)
        {
            output << "\n"
                   << setw(20) << right << temp->name << setw(20) << right << temp->data[temp->iterator];
            temp = temp->next;
        }
    }
    output << "\n----------------------------------------";
    if (queues[2]->gethead() == NULL)
    {
        output << "\n"
               << setw(20) << right << "Priority queue 2 is empty";
    }
    else
    {
        output << "\n"
               << setw(20) << right << "Currently Waiting";
        output << "\n"
               << setw(20) << right << "Process" 
               << setw(20) << right << "Burst";
        node *temp = queues[2]->gethead();
        while (temp != NULL)
        {
            output << "\n"
                   << setw(20) << right << temp->name 
                   << setw(20) << right << temp->data[temp->iterator];
            temp = temp->next;
        }
    }
    output << "\n----------------------------------------";
    if (queues[3]->gethead() == NULL)
    {
        output << "\n"
               << setw(20) << right << "Priority queue 3 is empty";
    }
    else
    {
        output << "\n"
               << setw(20) << right << "Currently Waiting";
        output << "\n"
               << setw(20) << right << "Process" 
               << setw(20) << right << "Burst";
        node *temp = queues[3]->gethead();
        while (temp != NULL)
        {
            output << "\n"
                   << setw(20) << right << temp->name << setw(20) << right << temp->data[temp->iterator];
            temp = temp->next;
        }
    }
    output << "\n----------------------------------------";
    if (queues[4] == NULL)
    {
        output << "\n"
               << setw(20) << right << "The I/O Queue is empty";
    }
    else
    {
        output << "\n"
               << setw(20) << right << "Currently in I/O";
        output << "\n"
               << setw(20) << right << "Process" 
               << setw(20) << right << "I/O time left";
        node *temp = queues[4]->gethead();
        while (temp != NULL)
        {
            output << "\n"
                   << setw(20) << right << temp->name << setw(20) << right << temp->data[temp->iterator];
            temp = temp->next;
        }
    }
    output << "\n----------------------------------------";
}
void getAverages(linked_list **queues, ofstream &output)
{
    node *tmp = queues[5]->gethead();
    float ttAvg = 0;
    float rtAvg = 0;
    float watAvg = 0;
    int contextSwitch = 0;
    output << "\n--------------------------------------------------------------------------------";
    output << "\n"
           << setw(20) << right << "Process" 
           << setw(20) << right << "Response Time" 
           << setw(20) << right << "Turnaround Time"
           << setw(20) << right << "Waiting Time";
    while (tmp != NULL)
    {
        contextSwitch += tmp->contextSwitch;
        ttAvg += tmp->turnaroundtime;
        rtAvg += tmp->responsetime;
        watAvg += tmp->turnaroundtime - tmp->totaltime;
        output << "\n"
               << setw(20) << right << tmp->name
               << setw(20) << right << tmp->responsetime
               << setw(20) << right << tmp->turnaroundtime
               << setw(20) << right << tmp->turnaroundtime - tmp->totaltime;
        tmp = tmp->next;
    }
    output << "\n--------------------------------------------------------------------------------";
    output << "\n"
           << setw(20) << right << "Averages:"
           << setw(20) << right << rtAvg / 8
           << setw(20) << right << ttAvg / 8
           << setw(20) << right << watAvg / 8;
    output << "\n--------------------------------------------------------------------------------";
    float cpuUtil = node::globalcount - node::idle;
    cpuUtil /= node::globalcount;
    cpuUtil *= 100;
    output << "\n\n"
           << setw(20) << right << "Total context switches"
           << setw(20) << right << contextSwitch;
    output << "\n\n"
           << setw(20) << right << "CPU utilization"
           << setw(20) << right << cpuUtil << "%";
    output.close();
}