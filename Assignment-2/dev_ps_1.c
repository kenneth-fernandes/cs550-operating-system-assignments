/* Required linux module and kernel header file */
#include <linux/module.h>
#include <linux/kernel.h>

/* File operations like open/close and read/ write to device*/
#include <linux/fs.h>

#include <linux/init.h>
#include <linux/sched.h>
#include <linux/sched/signal.h>
#include <linux/uaccess.h>
#include <linux/miscdevice.h>
#include <linux/slab.h>

/* Definition of the structure task_struct FOR for processes/tasks in sched.h  */
struct task_struct *process;

static int majorNumber;
static int chrBuffrSize = 256;
static int index = 0;
char **resultBufferArr;

// The prototype functions for the character driver -- must come before the struct definition
static int device_open(struct inode *, struct file *);
static ssize_t device_read(struct file *, char __user *, size_t, loff_t *);

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = device_open,
    .read = device_read};

static struct miscdevice process_lst_device = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = "process_lst",
    .fops = &fops};

static ssize_t device_read(struct file *file, char __user *out, size_t len, loff_t *offset)
{
    printk(KERN_INFO "%s", "\n Loading the Character Device for listing processes. \n");

    return 0;
}

static int device_open(struct inode *inodep, struct file *filep)
{
    printk(KERN_INFO "Process_Lst_Device: Device has been opened");

    resultBufferArr = kmalloc((sizeof(char *) * chrBuffrSize), GFP_KERNEL);

    for_each_process(process)
    {
        sprintf(resultBufferArr[index], "\n PROCESS = %s | PID = %d | PPID = %d | CPU = %d | STATE = %ld ",
                process->comm, process->pid, process->real_parent->pid, task_cpu(process), process->state);
        index += 1;
        /*printk(KERN_INFO "\n PROCESS = %s | PID = %d | PPID = %d | CPU = %d | STATE = %ld ",
               process->comm, process->pid, process->real_parent->pid, task_cpu(process), process->state);*/
    }
    return 0;
}

/* Defining the function that is called when module is installed */
int __init init_module()
{
    majorNumber = misc_register(&process_lst_device);

    if (majorNumber < 0)
    {
        printk(KERN_ALERT "Process_Lst_Device: Failed to register a major number\n");
        return majorNumber;
    }
    printk(KERN_INFO "Process_Lst_Device: Registered correctly with major number %d\n", majorNumber);

    return 0;
}

/* Defining the function that is called when module is un-installed */
void __exit cleanup_module()
{
    misc_deregister(&process_lst_device);
    printk(KERN_INFO "%s", "\n Un-installing the Character Device for listing processes. \n");
}
