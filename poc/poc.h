//
//  poc.h
//  iomfb_poc
//
//  Created by Saar Amar.
//

#ifndef poc_h
#define poc_h

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <mach/mach.h>
#include <pthread.h>

#include "iokit.h"

io_connect_t get_iomfb_uc(void);
void do_trigger(io_connect_t iomfb_uc);
void poc(void);

#endif /* poc_h */
