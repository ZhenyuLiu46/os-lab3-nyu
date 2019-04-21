//
//  VMA.h
//  Lab3_zhenyu
//
//  Created by Zhenyu Liu on 4/16/19.
//  Copyright © 2019 Zhenyu Liu. All rights reserved.
//

#ifndef VMA_h
#define VMA_h
#include <iostream>

class VMA {
public:
    VMA(int s, int e, int w, int f);
    
    int start_vpage;
    int end_vpage;
    int write_protected;
    int file_mapped;
    
};


#endif /* VMA_h */
