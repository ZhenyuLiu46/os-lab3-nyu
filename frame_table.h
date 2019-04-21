//
//  frame_table.h
//  Lab3_zhenyu
//
//  Created by Zhenyu Liu on 4/17/19.
//  Copyright © 2019 Zhenyu Liu. All rights reserved.
//

#ifndef frame_table_h
#define frame_table_h
#include <iostream>
#include <list>
#include <vector>

using namespace std;

class frame{//each frame has the mapping
public:
    int fid;
    bool exited;
    unsigned long age;
    unsigned long tau;
    pair<int, int> reverse_mappings; //process, vpage
    
    frame(int i)
    {tau = 0; age = 0; reverse_mappings = pair<int, int>(-1,-1);
        fid = i; exited = false;
    }
};

class frame_table{
    public:
    int fsize;
    list< frame* > free_list;
    vector< frame* > frame_vec; //saves all the frames with idx, including empty ones.
    frame* allocate_frame_from_free_list();
    void add_to_freelist(int vpage);
    frame_table(int size);
};

#endif /* frame_table_h */
