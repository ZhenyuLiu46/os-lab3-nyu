//
//  Process.h
//  Lab3_zhenyu
//
//  Created by Zhenyu Liu on 4/16/19.
//  Copyright © 2019 Zhenyu Liu. All rights reserved.
//



#ifndef Process_h
#define Process_h
#include "VMA.h"
#include "pte.h"
#include <iostream>
#include <vector>
#define pgt_size 64

using namespace std;

struct Pstats{
    unsigned long unmaps, maps, ins, outs, fins, fouts, zeros,
    segv, segprot;
    Pstats()
    {unmaps = 0; maps = 0; ins = 0; outs = 0; fins = 0; fouts = 0; zeros = 0; segv = 0; segprot = 0;}
};

class Process {
public:
    Process(int pid);
    vector<VMA*> vmas;
    vector<pte*> pagetable;
    Pstats* pstats;
    int PID;
    int numsOfVmas = 0;
    bool is_file_mapped(int vpage);
    bool is_write_protected(int vpage);
    bool is_in_vmas_table(int vpage);
};

#endif /* Process_h */

