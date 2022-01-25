#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <malloc.h>
#include "os.h"

 
int main(int argc, char *argv[])
{
       if (argc < 2) {
 printf("Enter pid\n");
 printf("Write pid -1 if you want to see pci devices\n");
 return 0;
 }
 pid_t value;
 pid_t pid = atoi(argv[1]);
 int fd;
 struct message msg;
 struct devices dv;
 printf("\nOpening Driver\n");
 fd = open(DEVICE_PATH, O_RDWR);
 if (fd < 0) {
 printf("Cannot open device file\n");
 return 0;
 }
 if (pid != -1){
 printf("Writing Pid to Driver\n");
 ioctl(fd, WR_VALUE, (pid_t * ) & pid);
 printf("Reading Value from Driver\n\n");
 ioctl(fd, RD_VALUE, (struct message *) &msg);
 printf("\nvm_area_struct_info for PID %d: \n", pid);
 printf("\tstart = %lu\n", msg.vma.start);
 printf("\tend = %lu\n", msg.vma.end);
 printf("\tsize = %lu\n", msg.vma.size);
 printf("\tflags = %c%c%c%c\n", msg.vma.flag_read, msg.vma.flag_write, msg.vma.flag_exec, msg.vma.flag_mayshare);
 printf("\toffset = %lu\n", msg.vma.offset);
 printf("\tstart_code = %lu\n", msg.vma.s_code);
 printf("\tend_code = %lu\n", msg.vma.e_code);
 
 }
 else{
 printf("Use dmesg to see pci devices\n");
 ioctl(fd, WR_VALUE, (pid_t * ) & pid);
 ioctl(fd, RD_VALUE, (pid_t *) & value);
 int i = 0;
 for(int i =0;i<50;i++){
 printf("%d\n",dv.number[i]);
 }
 }
 printf("\nClosing Driver\n");
 close(fd);
 
}
