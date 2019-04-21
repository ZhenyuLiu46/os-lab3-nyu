//
//  VMA.cpp
//  Lab3_zhenyu
//
//  Created by Zhenyu Liu on 4/16/19.
//  Copyright © 2019 Zhenyu Liu. All rights reserved.
//

#include <iostream>
#include "VMA.h"

VMA::VMA(int s, int e, int w, int f){
    start_vpage = s;
    end_vpage = e;
    write_protected = w;
    file_mapped = f;
}
