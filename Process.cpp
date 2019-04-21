//
//  Process.cpp
//  Lab3_zhenyu
//
//  Created by Zhenyu Liu on 4/16/19.
//  Copyright © 2019 Zhenyu Liu. All rights reserved.
//

#include <iostream>
#include "Process.h"



Process::Process(int pid){
    PID = pid;
    pstats = new Pstats();
    for (int i=0; i<pgt_size; i++) {
        pagetable.push_back(new pte());
    }
}

bool Process::is_file_mapped (int vpage){
    for (int i = 0; i < numsOfVmas; i++) {
        if (vpage >= vmas[i]->start_vpage && vpage <= vmas[i]->end_vpage) {
            if (vmas[i]->file_mapped){
                return true;
            }
            else return false;
        }
    }
    return false;
}

bool Process::is_write_protected (int vpage){
    for (int i = 0; i < numsOfVmas; i++) {
        if (vpage >= vmas[i]->start_vpage && vpage <= vmas[i]->end_vpage) {
            if (vmas[i]->write_protected){
                return true;
            }
            else return false;
        }
    }
    return false;
}

bool Process::is_in_vmas_table(int vpage){
    for (int i = 0; i < numsOfVmas; i++) {
        if (vpage >= vmas[i]->start_vpage && vpage <= vmas[i]->end_vpage) {
            return true;
        }
    }
    return false;
}
