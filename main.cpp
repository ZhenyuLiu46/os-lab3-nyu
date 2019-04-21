//
//  main.cpp
//  Lab3_zhenyu
//
//  Created by Zhenyu Liu on 4/16/19.
//  Copyright © 2019 Zhenyu Liu. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <getopt.h>
#include <cstdio>


#include "Process.h"
#include "VMA.h"
#include "pte.h"
#include "pager.h"
#include "frame_table.h"

using namespace std;

bool get_next_instruction(ifstream& inputFile, string& operation, string& vpage){
    if(inputFile.eof()) return false;
    string line;
    getline(inputFile, line);
    
    while (line[0] == '#') {
        if (inputFile.eof()) {
            return false;
        }
        getline(inputFile, line);
    }
    operation = line[0];
    if(operation != "c" && operation != "r" && operation != "w" && operation != "e" ){
        return false;
    }
    if(line.length() > 0) {
        vpage = line.substr(1);
    }
    return true;
}

void output_PT(vector<Process*> processes){
    string ref, mod, swap;
    Process* process;
    for (int i = 0; i < processes.size(); i++) {
        process = processes[i];
        cout << "PT[" << i <<"]:";
        for (int j = 0; j<process->pagetable.size(); j++) {
            pte* curr_pte = process->pagetable[j];
            if (curr_pte->PRESENT) {
                if (curr_pte->REFERENCED) {
                    ref = "R";
                } else{ ref = "-";}
                
                if (curr_pte->MODIFIED) {
                    mod = "M";
                } else{ mod = "-";}
                
                if (curr_pte->PAGEDOUT) {
                    swap = "S";
                } else{ swap = "-";}
                cout << " " << j << ":" << ref << mod << swap;
            }
            else{
                if (curr_pte->PAGEDOUT) cout<<" #";
                else { cout<<" *";}
            }
        }
        cout << endl;
    }
}

void output_FT (frame_table* frametable){
    cout << "FT:";
    for (int i=0; i< frametable->frame_vec.size(); i++) {
        frame* curr_frame = frametable->frame_vec[i];
        if (curr_frame->reverse_mappings.first != -1) {
            cout << " " << curr_frame->reverse_mappings.first <<":" << curr_frame->reverse_mappings.second;
        }
        else{
            cout << " *";
        }
    }
    cout << endl;
}

void output_sum(vector<Process*> processes){
    Process* process;
    for (int i = 0; i<processes.size(); i++) {
        process = processes[i];
        cout << "PROC["<< process->PID <<"]: U="<< process->pstats->unmaps << " M=" << process->pstats->maps
        << " I="<< process->pstats->ins << " O="<< process->pstats->outs <<" FI="
        << process->pstats->fins << " FO="<< process->pstats->fouts << " Z="<< process->pstats->zeros <<
        " SV="<<process->pstats->segv << " SP=" << process->pstats->segprot << endl;
    }
}


int main(int argc, char * argv[]) {
    
    //read input from argument
    int c=0;
    Pager* pager;
    int frameTableSize = 16;
    bool isRandomAlgo = false;
    bool optionS=false, optionalP=false, optionalF = false, optionalO = false;
    
    while (( c = getopt(argc, argv, "a:f:o:")) != -1) {
        switch (c) {
            case 'a': {
                string algo = optarg;
                
                switch (algo[0]) {
                    case 'f':
                        pager = new FIFO();
                        break;
                        
                    case 'r':
                        isRandomAlgo = true;
                        //pager = new RANDOM(randomfileName);
                        break;
                        
                    case 'c':
                        pager = new CLOCK();
                        break;
                        
                    case 'e':
                        pager = new ESCNRU();
                        break;
                        
                    case 'a':
                        pager = new AGE();
                        break;
                        
                    case 'w':
                        pager = new WORKING_SET();
                        break;
                        
                    default:
                        cout << "algorithm not correct"<<endl;
                        exit(0);
                }
                
                break;
            }
                
            case 'f':
                frameTableSize = atoi(optarg);
                break;
                
            case 'o':
                string optionalOutput = optarg;
                for (int i = 0; i < optionalOutput.size(); i++) {
                    switch (optionalOutput[i]) {
                        case 'S':
                            optionS = true;
                            break;
                            
                        case 'F':
                            optionalF = true;
                            break;
                            
                        case 'O':
                            optionalO = true;
                            break;
                            
                        case 'P':
                            optionalP = true;
                            break;
                            
                        default:
                            cout << "optional wrong input"<<endl;
                            break;
                    }
                }
                break;
        }
    } //read input done, here always print, did not implement condition to not print
    
    //read file names
    int idx = optind;
    string inputfileName = argv[idx];
    string randomfileName = argv[idx+1];
    
    ifstream inputFile;
    //inputFile.open("/Users/zhenyuliu/Desktop/Lab3_Stuff/lab3_assign/inputs/in1");
    inputFile.open(inputfileName);
    
    if(isRandomAlgo == true){
        pager = new RANDOM(randomfileName);
    }
    
    //variables
    int numOfProcess = 0;
    vector<Process*> processes;
    Process* currProcess = nullptr;
    //int frameTableSize = 16;
    frame_table* frameTable = new frame_table(frameTableSize);
    //FIFO* pager = new FIFO(); // all works
    //RANDOM* pager = new RANDOM(); //all works
    //CLOCK* pager = new CLOCK(); // all works
    //ESCNRU* pager = new ESCNRU(); // all works
    //AGE* pager = new AGE(); //all works
    //WORKING_SET* pager = new WORKING_SET(); // all works
    
    unsigned long long execCycles = 0; //cost
    unsigned long ctx_switches = 0;
    unsigned long process_exits = 0;
    unsigned long instructionidx = 0; //instruction count
    
    if(inputFile.is_open()){
        string line;
        int s, e, w, f; //for vmas 4 fields;
        
        while (getline(inputFile, line))
        {
            istringstream ss(line);
            char firstLetter;
            ss >> firstLetter;
            if(firstLetter == '#'){
                continue;
            }
            numOfProcess = firstLetter - '0';
            
            for(int i = 0; i < numOfProcess; i++){
                Process *p = new Process(i);
                getline(inputFile, line);
                istringstream ss(line);
                ss >> firstLetter;
                if(firstLetter == '#') {
                    i--;
                    continue;
                }
                p->numsOfVmas = firstLetter - '0';
                for(int j = 0; j < p->numsOfVmas; j++){
                    getline(inputFile, line);
                    istringstream ss(line);
                    ss >> s >> e >> w >> f ;
                    VMA *v = new VMA(s,e,w,f);
                    p->vmas.push_back(v);
                }
                processes.push_back(p);
            }
            //to read instrustions
            string operation;
            string vpage;
            
            while(get_next_instruction(inputFile, operation, vpage)){
                
                int vpagenum = stoi(vpage);
                cout << instructionidx <<": ==> "<<operation<<" "<<vpagenum<<endl;
                instructionidx++;
                
                // deal with 'w' and 'r'
                if (operation == "w" || operation == "r") {
                    execCycles += 1;
                    //check if is in vmas
                    if (!currProcess->is_in_vmas_table(vpagenum)) {
                        execCycles += 240;
                        currProcess->pstats->segv++;
                        cout<<" SEGV"<<endl;
                        continue;
                    }
                    
                    pte* curr_pte = currProcess->pagetable[vpagenum];
                    frame* selectedFrame = nullptr;
                    if (!curr_pte->PRESENT) {
                        selectedFrame = frameTable->allocate_frame_from_free_list();
                        if (selectedFrame == nullptr) {
                            selectedFrame = pager->select_victim_frame(frameTable, processes, instructionidx);
                            //deal with unmap;
                            int original_pid = selectedFrame->reverse_mappings.first;
                            int original_pteIdx = selectedFrame->reverse_mappings.second;
                            cout<<" UNMAP "<< original_pid <<":"<< original_pteIdx <<endl;
                            
                            execCycles += 400;
                            Process* original_proc = processes[original_pid];
                            pte* orignal_pte = original_proc->pagetable[original_pteIdx];
                            original_proc->pstats->unmaps++;
                            orignal_pte->PRESENT = 0;
                            orignal_pte->REFERENCED = 0;
                            
                            //MODIFIED, deal with pageout, OUT OR FOUT
                            if (orignal_pte->MODIFIED) {
                                orignal_pte->MODIFIED = 0;
                                if (original_proc->is_file_mapped(original_pteIdx)) { //FOUT
                                    execCycles += 2500;
                                    original_proc->pstats->fouts++;
                                    //orignal_pte->PAGEDOUT = 1; do not change?here relied on this logic, be careful
                                    cout << " FOUT"<<endl;
                                }
                                else { //OUT
                                    execCycles += 3000;
                                    original_proc->pstats->outs++;
                                    orignal_pte->PAGEDOUT = 1;
                                    cout << " OUT"<<endl;
                                }
                            }
                        } //end select_victim_frame == nullptr here;
                        
                        //deal with IN (swap device) OR FIN OR ZEROS
                        if (curr_pte->PAGEDOUT) {
                            currProcess->pstats->ins++;
                            execCycles += 3000;
                            cout << " IN"<<endl;
                        }
                        
                        else if (currProcess->is_file_mapped(vpagenum)){
                            currProcess->pstats->fins++;
                            execCycles += 2500;
                            cout << " FIN" << endl;
                        }
                        else{
                            currProcess->pstats->zeros++;
                            execCycles += 150;
                            cout << " ZERO" << endl;
                        }
                        selectedFrame->reverse_mappings.first = currProcess->PID; //frame saves process, vapageid
                        selectedFrame->reverse_mappings.second = vpagenum;
                        curr_pte->frame = selectedFrame->fid; //pte saves the related frame
                        
                        execCycles += 400;
                        currProcess->pstats->maps++;
                        selectedFrame->age = 0;
                        curr_pte->PRESENT = 1;
                       if(selectedFrame->exited == false){
                            pager->frame_list.push_back(selectedFrame); //add back to pager, for next victim selection
                        }
                        selectedFrame->exited = false; 
                        cout << " MAP " << selectedFrame->fid<<endl;
                        
                    } else{//curr_pte is already present;
                        selectedFrame = frameTable->frame_vec[curr_pte->frame];
                    }
                    // deal with w
                    if (operation == "w") {
                        if (currProcess->is_write_protected(vpagenum)) {
                            execCycles += 300;
                            currProcess->pstats->segprot++;
                            cout << " SEGPROT" << endl;
                        }
                        else{
                            curr_pte->MODIFIED = 1;
                        }
                    }
                    curr_pte->REFERENCED = 1;
                    selectedFrame->tau = instructionidx-1; //
                    
                } //end r,w here
                
                // deal with special case 'c' and 'e'
                else if (operation == "c"){
                    currProcess = processes[vpagenum];
                    execCycles += 121;
                    ctx_switches++;
                }
                else if (operation == "e"){
                    process_exits++;
                    execCycles += 175;
                    cout << "EXIT current process " << vpagenum << endl;
                    for (int i = 0; i < pgt_size; i++) {
                        pte* curr_pte = currProcess->pagetable[i];
                        if (curr_pte->PRESENT) {
                            int frame_idx = curr_pte->frame;
                            
                            frameTable->add_to_freelist(frame_idx); //add back the used frames to freelist
                            frame* curr_frame = frameTable->frame_vec[frame_idx];
                            cout<<" UNMAP "<<curr_frame->reverse_mappings.first<<":"<<curr_frame->reverse_mappings.second<<endl;
                            curr_frame->reverse_mappings = pair<int, int>(-1, -1);
                            curr_frame->exited = true;
                            
                            execCycles += 400;
                            currProcess->pstats->unmaps++;
                            
                            if (curr_pte->MODIFIED) {
                                curr_pte->MODIFIED = 0;
                                //FOUT
                                if (currProcess->is_file_mapped(i)) {
                                    execCycles += 2500;
                                    currProcess->pstats->fouts++;
                                    cout << " FOUT"<<endl;
                                }
                            }
                            curr_pte->PRESENT = 0;
                            curr_pte->REFERENCED = 0;
                        }
                        curr_pte->PAGEDOUT = 0; 
                    }
                }
            }//while get instruction loop end here
            //break;
        }//while get line
        output_PT(processes);
        output_FT(frameTable);
        output_sum(processes);
        printf("TOTALCOST %lu %lu %lu %llu\n", instructionidx, ctx_switches, process_exits, execCycles);
    } else {
        cout << "file not exsit";
    }
    
    //print out process
//    for(Process* p : processes){
//        printf("Process %d \n%d \n", p->PID, p->numsOfVmas);
//        for(int i = 0; i < p->vmas.size(); i++){
//            printf("%d %d %d %d \n", p->vmas[i]->start_vpage, p->vmas[i]->end_vpage, p->vmas[i]->write_protected, p->vmas[i]->file_mapped);
//        }
//    }
    inputFile.close();
    return 0;
}

