#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/slab.h>                 //kmalloc()
#include <linux/uaccess.h>              //copy_to/from_user()
#include <linux/ioctl.h>
#include <linux/pci.h>
#include <linux/pid.h>
#include <linux/debugfs.h>
#include <linux/sched.h>
#include <linux/types.h>
#include <linux/fdtable.h>
#include <linux/mm.h>
#include "os.h"

 //define ioctl command


pid_t value = 0;
static int pid = 1234;

dev_t dev = 0;
static struct class *dev_class;
static struct cdev etx_cdev;
 struct mm_struct *a_mm;

//static struct pci_dev *dev2;
static struct task_struct *ts;

//static struct file f;
//static atomic_t c;

//function pro
static int      __init etx_driver_init(void);
static void     __exit etx_driver_exit(void);
static int      etx_open(struct inode *inode, struct file *file);
static int      etx_release(struct inode *inode, struct file *file);
static long     etx_ioctl(struct file *file, unsigned int cmd, unsigned long arg);

struct message *fill_structs(void);

/*
** File operation sturcture
*/
static struct file_operations fops =
{
        .owner          = THIS_MODULE,
        .open           = etx_open,
        .unlocked_ioctl = etx_ioctl,
        .release        = etx_release,
};

/*
** This function will be called when we open the Device file
*/
static int etx_open(struct inode *inode, struct file *file)
{
        pr_info("Device File Opened...!!!\n");
        return 0;
}
/*
** This function will be called when we close the Device file
*/
static int etx_release(struct inode *inode, struct file *file)
{
        pr_info("Device File Closed...!!!\n");
        return 0;
}

/*
** This function will be called when we write IOCTL on the Device file
*/
static long etx_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    struct message *msg;
         switch(cmd) {
                case WR_VALUE:
                        if( copy_from_user(&value ,(pid_t*) arg, sizeof(value)) )
                        {
                                pr_err("Data Write : Err!\n");
                        }
                        pr_info("Value = %d\n", value);

                        break;
                case RD_VALUE:
                    msg = fill_structs();
                    printk("msg = %p\n", msg);
                    if (msg == NULL) {
                        break;
                    }
                        if (copy_to_user((struct message *) arg, msg, sizeof(struct message))) {
                printk("driver - Error copying data to user!\n");
            } else {
                printk("driver - The pid was copied!\n");
            }
                    vfree(msg);
            break;
                default:
                        pr_info("Default\n");
                        break;
        }
        return 0;
}

/*
** Module Init function
*/
static int __init etx_driver_init(void)
{
        /*Allocating Major number*/
        if((alloc_chrdev_region(&dev, 0, 1, "etx_Dev")) <0){
                pr_err("Cannot allocate major number\n");
                return -1;
        }
        pr_info("Major = %d Minor = %d \n",MAJOR(dev), MINOR(dev));

        /*Creating cdev structure*/
        cdev_init(&etx_cdev,&fops);

        /*Adding character device to the system*/
        if((cdev_add(&etx_cdev,dev,1)) < 0){
            pr_err("Cannot add the device to the system\n");
            goto r_class;
        }

        /*Creating struct class*/
        if((dev_class = class_create(THIS_MODULE,"etx_class")) == NULL){
            pr_err("Cannot create the struct class\n");
            goto r_class;
        }

        /*Creating device*/
        if((device_create(dev_class,NULL,dev,NULL,"etx_device")) == NULL){
            pr_err("Cannot create the Device 1\n");
            goto r_device;
        }
        pr_info("Device Driver Insert...Done!!!\n");
        return 0;

r_device:
        class_destroy(dev_class);
r_class:
        unregister_chrdev_region(dev,1);
        return -1;
}


struct message *fill_structs()
{
    struct message *msg = vmalloc(sizeof(struct message));
    struct pci_dev *pcidv = NULL;
    int i;
    if (msg == NULL) {
        printk("vmalloc failed\n");
        return NULL;
    }
// int arr[50];
 if (value == -1){
 for (i = 0; i<50 && (pcidv = pci_get_device(PCI_ANY_ID, PCI_ANY_ID, pcidv)); i++)
        {
            msg->dv.number[i] = pcidv->device;;
        printk(KERN_INFO "%d. pci found %d\n", i, msg->dv.number[i]);
    }
 }
 else{
 ts = get_pid_task(find_get_pid(value), PIDTYPE_PID);
 if (ts->mm) {
  a_mm = ts->active_mm;

 msg->vma.s_code = a_mm->start_data;
 msg->vma.e_code = a_mm->end_data;
 msg->vma.start = ts->mm->mmap->vm_start;
 msg->vma.end = ts->mm->mmap->vm_end;
 msg->vma.size = ts->mm->mmap->vm_end - ts->mm->mmap->vm_start;
 msg->vma.offset = ts->mm->mmap->vm_pgoff;
 msg->vma.flag_read = (ts->mm->mmap->vm_flags & VM_READ) ? 'r' : '-';
 msg->vma.flag_write = (ts->mm->mmap->vm_flags & VM_WRITE) ? 'w' : '-';
 msg->vma.flag_exec = (ts->mm->mmap->vm_flags & VM_EXEC) ? 'x' : '-';
 msg->vma.flag_mayshare = (ts->mm->mmap->vm_flags & VM_MAYSHARE) ? 's' : 'p';
 }
 }
 return msg;
}


/*
** Module exit function
*/
static void __exit etx_driver_exit(void)
{
        device_destroy(dev_class,dev);
        class_destroy(dev_class);
        cdev_del(&etx_cdev);
        unregister_chrdev_region(dev, 1);
        pr_info("Device Driver Remove...Done!!!\n");
}

module_init(etx_driver_init);
module_exit(etx_driver_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Simple Linux device driver (IOCTL)");
MODULE_VERSION("1.0");
