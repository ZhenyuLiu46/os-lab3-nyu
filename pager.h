//
//  pager.h
//  Lab3_zhenyu
//
//  Created by Zhenyu Liu on 4/17/19.
//  Copyright © 2019 Zhenyu Liu. All rights reserved.
//

#ifndef pager_h
#define pager_h
#include <iostream>
#include "frame_table.h"
#include "Process.h"

class Pager{
public:
    list<frame*> frame_list;
    virtual frame* select_victim_frame(frame_table* frametable, vector<Process*> processes, unsigned long instructionidx) = 0;

    
    Pager(){};
};

class FIFO : public Pager {
public:
   // list<frame*> frame_list; //when allocated a free list, move that frame from freelist to here
    FIFO() : Pager(){};
    frame* select_victim_frame(frame_table* frametable, vector<Process*> processes, unsigned long instructionidx);
};

class RANDOM : public Pager {
public:
  //  list<frame*> frame_list; //when allocated a free list, move that frame from freelist to here
    //RANDOM();
    RANDOM(string randomfileName);
    vector<int> randomsNums;
    int ofs = 0;
    int randomFileLength;
    frame* select_victim_frame(frame_table* frametable, vector<Process*> processes, unsigned long instructionidx );
};

class CLOCK : public Pager {
public:
//    list<frame*> frame_list; //when allocated a free list, move that frame from freelist to here
    CLOCK() : Pager(){};
    frame* select_victim_frame(frame_table* frametable, vector<Process*> processes, unsigned long instructionidx);

};

class ESCNRU : public Pager {
public:
    list<frame*> frame_list;
    unsigned long prev_instru_cnt = 0;
    int hand = 0;
    frame* ESC_class[4];
    ESCNRU();
    frame* select_victim_frame(frame_table* frametable, vector<Process*> processes, unsigned long instructionidx);
};

class AGE : public Pager {
public:
    list<frame*> frame_list;
    int hand = 0;
    AGE() : Pager() {};
    frame* select_victim_frame(frame_table* frametable, vector<Process*> processes, unsigned long instructionidx);
};

class WORKING_SET : public Pager{
public:
    list<frame*> frame_list;
    int hand = 0;
    WORKING_SET() : Pager() {};
    frame* select_victim_frame(frame_table* frametable, vector<Process*> processes, unsigned long instructionidx);
};


#endif /* pager_h */
