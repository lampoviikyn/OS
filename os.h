#ifndef OS_LAB2_DRIVER_H
#define OS_LAB2_DRIVER_H

struct vm_area_struct_info {
 unsigned long start;
 unsigned long end;
 unsigned long s_code;
 unsigned long e_code;
 
 unsigned long size;
 unsigned long offset;
 char flag_read;
 char flag_write;
 char flag_exec;
 char flag_mayshare;
};

//struct pci_dev_info {
 //int devices;
//};
struct devices{
int number[50];
};
struct message {
// struct pci_dev pcidv;
 //struct devices dv;
 struct devices dv;
 struct vm_area_struct_info vma;
};



#define WR_VALUE _IOW('a','a', struct message*)
#define RD_VALUE _IOR('a','b', struct message*)

#define DEVICE_NAME "etx_device"
#define DEVICE_PATH "/dev/etx_device"

#endif //OS_LAB2_DRIVER_H
