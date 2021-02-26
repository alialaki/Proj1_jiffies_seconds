/**
 * Jiffies.c
 *
 * Kernel module that communicates with /proc file system.
 * 
 * The makefile must be modified to compile this program.
 */

#include<linux/fs.h>
#include<linux/sched.h>
#include<linux/device.h>
#include<linux/slab.h>
#include <linux/string.h>
#include <linux/timer.h>

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>
#include <linux/hash.h>
#include <linux/gcd.h>
#include <linux/jiffies.h>

#define BUFFER_SIZE 128
#define PROC_NAME "jiffies"
#define MESSAGE "Kernel Module is running\n"

/**
 * Function prototypes
 */
ssize_t proc_read(struct file *file, char *buf, size_t count, loff_t *pos);

static struct file_operations proc_ops = {
        .owner = THIS_MODULE,
        .read = proc_read,
};

/* This function is called when the module is loaded. */
int proc_init(void) {
        // creates the /proc/jiffies entry
        // the following function call is a wrapper for
        // proc_create_data() passing NULL as the last argument
        proc_create(PROC_NAME, 0, NULL, &proc_ops);
        printk(KERN_INFO "/proc/%s created\n", PROC_NAME);
        printk( KERN_INFO "In proc_init(), Jiffies is: %lu\n", jiffies);
        /* Find out time in Hz when kernel module initialize */
	return 0;
}

/* This function is called when the module is removed. */
void proc_exit(void) {
    
        // removes the /proc/jiffies entry
        remove_proc_entry(PROC_NAME, NULL);
        printk( KERN_INFO "In proc_exit(), jiffies is: %lu\n", jiffies);
        printk( KERN_INFO "/proc/%s removed\n", PROC_NAME);

}

/**
 * This function is called each time the /proc/jiffies is read,
 * and is called repeatedly until it returns 0, so
 * there must be logic that ensures it ultimately returns 0
 * once it has collected the data that is to go into the 
 * corresponding /proc file.
 * params:
 * file:
 * buf: buffer in user space
 * count:
 * pos:
 */
ssize_t proc_read(struct file *file, char __user *usr_buf, size_t count, loff_t *pos) {
       
        int rv = 0;
        char buffer[BUFFER_SIZE];
        static int completed = 0;

        if (completed) {
                completed = 0;
                return 0;
        }
        completed = 1;
        /*Everytime I call proc/read */
        rv = sprintf(buffer,"The value of jiffies in /proc/read is: %lu\n", jiffies);
        /*Actual value of of jifiies at the time that were happened*/
        printk( KERN_INFO "In proc_read(), jiffies is: %lu\n", jiffies);
        // copies the contents of buffer to userspace usr_buf
        copy_to_user(usr_buf, buffer, rv);

        return rv;
}


/* Macros for registering module entry and exit points. */
module_init( proc_init );
module_exit( proc_exit );

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Jiffies Module");
MODULE_AUTHOR("SGG");


//=======================================================
//Shell script for jiffies.ko
//=======================================================

/*
#!/bin/bash

# clear the message queue, then
# insert jiffies.ko into the kernel and remove it again, then
# display the messages as a result of doing so

sudo -p "osc"
sudo dmesg -c 
clear 


printf "beginning script...\n\n"
sudo insmod jiffies.ko

cat /proc/jiffies
sleep 1
cat /proc/jiffies
sleep 2
cat /proc/jiffies
sleep 3

sudo rmmod jiffies 
sudo dmesg 

printf "\n...end of script\n\n"
*/


//=======================================================
//Possible OUTPUT from jiffies.ko//////
//=======================================================

/*
beginning script...

The value of jiffies in /proc/read is: 4297364682
The value of jiffies in /proc/read is: 4297364941
The value of jiffies in /proc/read is: 4297365446
[ 9890.132683] /proc/jiffies created
[ 9890.132685] In proc_init(), Jiffies is: 4297364682
[ 9896.203112] In proc_exit(), jiffies is: 4297366200
[ 9896.203114] /proc/jiffies removed

...end of script
*/

//=======================================================
//End of OUTPUT from jiffies.ko
//=======================================================


