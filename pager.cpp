//
//  pager.cpp
//  Lab3_zhenyu
//
//  Created by Zhenyu Liu on 4/17/19.
//  Copyright © 2019 Zhenyu Liu. All rights reserved.
//

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <climits>
#include "pager.h"


using namespace std;

frame* FIFO::select_victim_frame(frame_table* frametable, vector<Process*> processes, unsigned long  instructionidx){
    if(frame_list.size() == 0) return nullptr;
    frame* frame = frame_list.front();
    frame_list.pop_front();
   // while (frame->exited) {
   //     frame->exited = false;
   //     frame = select_victim_frame(frametable, processes, instructionidx);
   // }
    return frame;
}

RANDOM::RANDOM(string randomfileName){
    ifstream randomFile;
    //randomFile.open("/Users/zhenyuliu/Desktop/Lab3_Stuff/lab3_assign/inputs/rfile");
    randomFile.open(randomfileName);
    randomFile >> randomFileLength;
    for(int i=0; i<randomFileLength; i++) {
        int val;
        randomFile >> val;
        randomsNums.push_back(val);
    }
}

frame* RANDOM::select_victim_frame(frame_table* frametable, vector<Process*> processes, unsigned long  instructionidx){
    if(frame_list.size() == 0) return nullptr;
    int idx = randomsNums[(ofs++)%randomFileLength] % frametable->fsize;
    frame* frame = frametable->frame_vec[idx];
    return frame;
}


frame* CLOCK::select_victim_frame(frame_table* frametable, vector<Process*> processes, unsigned long  instructionidx){
    if(frame_list.size() == 0) return nullptr;
    frame* frame = frame_list.front();
    int pid = frame->reverse_mappings.first;
    int pteNum = frame->reverse_mappings.second;
    Process* process = processes[pid];
    pte* cur_page = process->pagetable[pteNum];
    while(cur_page->REFERENCED){
        cur_page->REFERENCED = 0;
        frame_list.pop_front();
        frame_list.push_back(frame);
        frame = frame_list.front();
        pid = frame->reverse_mappings.first;
        pteNum = frame->reverse_mappings.second;
        process = processes[pid];
        cur_page = process->pagetable[pteNum];
    }
    
    frame_list.pop_front();
    while (frame->exited) {
        frame->exited = false;
        frame = select_victim_frame(frametable, processes, instructionidx);
    }
    return frame;
}

ESCNRU::ESCNRU(){
    //initialize escnru to nullptr
    for (int i = 0; i < 4; i++) {
        ESC_class[i] = nullptr;
    }
}

frame* ESCNRU::select_victim_frame(frame_table* frametable, vector<Process*> processes, unsigned long instructionidx){
    frame* cur_frame;
    frame* victim = nullptr;
    Process* cur_process;
    pte* curr_pte;
    int curr_pte_idx;
    int process_pid;
    int classid;
    //has to clear again, every time
    for (int i = 0; i < 4; i++) {
        ESC_class[i] = nullptr;
    }
    
    for (int i = 0; i < frametable->fsize; i++) {
        //to get curr_pte
        int fidx = (i + hand) % frametable->frame_vec.size(); // frame idx
        cur_frame = frametable->frame_vec[fidx];
        process_pid = cur_frame->reverse_mappings.first;
        curr_pte_idx = cur_frame->reverse_mappings.second;
        cur_process = processes[process_pid];
        curr_pte = cur_process->pagetable[curr_pte_idx];
        //calculate ESC_class classid
        classid = curr_pte->MODIFIED + curr_pte->REFERENCED * 2;
        
        if (ESC_class[classid] == nullptr){
            if (classid == 0) {
                victim = cur_frame;
                //set hand to victim next
                hand = (fidx + 1) % frametable->fsize;
                break;
            }
            ESC_class[classid] = cur_frame;
        }
    }
    if (victim == nullptr) {
        for (int i = 1; i < 4; i++) {
            if (ESC_class[i] != nullptr) {
                victim = ESC_class[i];
                //set hand to victim next
                hand = (victim->fid + 1) % frametable->fsize;
                break;
            }
        }
    }
    
    //reset after consider the page, after 50 instructions,
    if (instructionidx - prev_instru_cnt >= 50) {
        for (int i = 0; i < frametable->fsize; i++) {
            cur_frame = frametable->frame_vec[i];
            process_pid = cur_frame->reverse_mappings.first;
            if (process_pid != -1) {
                cur_process = processes[process_pid];
                curr_pte_idx = cur_frame->reverse_mappings.second;
                cur_process->pagetable[curr_pte_idx]->REFERENCED = 0;
            }
        }
        prev_instru_cnt = instructionidx;
    }
    return victim;
}

frame* AGE ::select_victim_frame(frame_table *frametable, vector<Process *> processes, unsigned long instructionidx){
    frame* cur_frame;
    frame* victim = nullptr;
    Process* cur_process;
    pte* curr_pte;
    int curr_pte_idx;
    int process_pid;
    
    victim = frametable->frame_vec[hand];
    
    for (int i = 0; i < frametable->fsize; i++) {
        int fidx = (i + hand) % frametable->frame_vec.size(); // curr frame idx
        cur_frame = frametable->frame_vec[fidx];
        process_pid = cur_frame->reverse_mappings.first;
        curr_pte_idx = cur_frame->reverse_mappings.second;
        cur_process = processes[process_pid];
        curr_pte = cur_process->pagetable[curr_pte_idx];
        
        cur_frame->age >>= 1; //right shift 1 bit of the age
        
        if (curr_pte->REFERENCED){ //find the one is currently mapped.
            cur_frame->age = cur_frame->age | ((unsigned long)1 << 31);
            curr_pte->REFERENCED = 0;  //reset ref bit after each selectvictim
        }
        if (cur_frame->age < victim->age) {
            victim = cur_frame;
        }
    }
    hand = (victim->fid + 1) % frametable->fsize;  //set hand to victim next
    return victim;
}

frame* WORKING_SET::select_victim_frame(frame_table *frametable, vector<Process *> processes, unsigned long instructionidx){
    frame* cur_frame;
    frame* victim = nullptr;
    Process* cur_process;
    pte* curr_pte;
    int curr_pte_idx;
    int process_pid;
    //unsigned long smallThanFifty = LONG_MIN; // smallest closes to 50
    int smallThanFifty = INT_MIN; // smallest closes to 50
    
    for (int i = 0; i < frametable->fsize; i++) {
        int fidx = (i + hand) % frametable->frame_vec.size(); // curr frame idx
        cur_frame = frametable->frame_vec[fidx];
        process_pid = cur_frame->reverse_mappings.first;
        curr_pte_idx = cur_frame->reverse_mappings.second;
        cur_process = processes[process_pid];
        curr_pte = cur_process->pagetable[curr_pte_idx];
        
        if (curr_pte->REFERENCED){ //find the one is currently mapped.
            
            curr_pte->REFERENCED = 0; // reset last referenced and skip choosing them
            cur_frame->tau = instructionidx - 1;
        } else {
            //unsigned long diff = instructionidx - 1 - cur_frame->tau; ?not working
            int diff = instructionidx - 1 - cur_frame->tau;
            if (diff < 50) { // find the one most far away used
                if (diff > smallThanFifty) {
                    smallThanFifty = diff;
                    victim = cur_frame;
                }
            }
            else if (diff >= 50) {
                victim = cur_frame;
                break;
            }
        }
    } // end for here
    if (victim == nullptr) {
        victim = frametable->frame_vec[hand];
    }
    
    hand = (victim->fid + 1) % frametable->fsize;
    return victim;
}
