//
//  frame_table.cpp
//  Lab3_zhenyu
//
//  Created by Zhenyu Liu on 4/17/19.
//  Copyright © 2019 Zhenyu Liu. All rights reserved.
//

#include <stdio.h>
#include <iostream>
#include "frame_table.h"

frame_table :: frame_table(int size){
    fsize = size;
    for (int i = 0; i < size; ++i){
        frame_vec.push_back(new frame(i));
        free_list.push_back(frame_vec[i]);
    }
};

frame* frame_table ::  allocate_frame_from_free_list(){
    if (free_list.size()>0){
        frame* newframe = free_list.front();
        free_list.pop_front();
        return newframe;
        //need to add newframe to pager frame_list
    }
    return nullptr;
}

void frame_table :: add_to_freelist(int idx){
    free_list.push_back(frame_vec[idx]);
}
