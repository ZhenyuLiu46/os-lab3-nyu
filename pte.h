//
//  pte.h
//  Lab3_zhenyu
//
//  Created by Zhenyu Liu on 4/17/19.
//  Copyright © 2019 Zhenyu Liu. All rights reserved.
//

#ifndef pte_h
#define pte_h
#include <iostream>

class pte{
    public:
    unsigned int PRESENT:1;
    unsigned int WRITE_PROTECT:1;
    unsigned int MODIFIED:1;
    unsigned int REFERENCED:1;
    unsigned int PAGEDOUT:1;
    unsigned int pg_bit:20; 
    unsigned int frame:7;
    pte();
};

#endif /* pte_h */
