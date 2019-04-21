//
//  pte.cpp
//  Lab3_zhenyu
//
//  Created by Zhenyu Liu on 4/17/19.
//  Copyright © 2019 Zhenyu Liu. All rights reserved.
//

#include <stdio.h>

#include "pte.h"


pte::pte(){
    PRESENT=0;
    WRITE_PROTECT=0;
    MODIFIED=0;
    REFERENCED=0;
    PAGEDOUT=0;
    pg_bit=0;
    frame=0;
}
