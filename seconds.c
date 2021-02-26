/**
 * Jiffies.c
 *
 * Kernel module that communicates with /proc file system.
 * 
 * The makefile must be modified to compile this program.
 * Change the line "jiffies.o" to "jiffies.o"
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
#include <linux/jiffies.h>

#define BUFFER_SIZE 128
#define PROC_NAME "seconds"
#define MESSAGE "Kernel Module is running\n"
 unsigned long s_time, e_time;
 unsigned long seconds;
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
         s_time = jiffies;
        // creates the /proc/jiffies entry
        // the following function call is a wrapper for
        // proc_create_data() passing NULL as the last argument
        proc_create(PROC_NAME, 0, NULL, &proc_ops);
        printk(KERN_INFO "/proc/%s created\n", PROC_NAME);
        printk( KERN_INFO "In proc_init(), Jiffies is: %lu and HZ is: %d\n", jiffies, HZ);
        /* Find out time in Hz when kernel module initialize */
        printk( KERN_INFO "In proc_init(), initial time is: %lu\n", s_time);
	return 0;
}

/* This function is called when the module is removed. */
void proc_exit(void) {
        // removes the /proc/seconds entry
        remove_proc_entry(PROC_NAME, NULL);
        printk( KERN_INFO "In proc_exit(), jiffies is: %lu\n", jiffies);
        printk( KERN_INFO "/proc/%s removed\n", PROC_NAME);
}

/**
 * This function is called each time the /proc/seconds is read,
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
         e_time = jiffies;
        seconds = ((e_time-s_time)/HZ);
        rv = sprintf(buffer,"seconds in /proc/read is: %lu\n", seconds);
        //value of seconds, when it is actuall time it called
        printk( KERN_INFO "In proc_read(), seconds is: %lu\n", seconds);
        // copies the contents of buffer to userspace usr_buf
        copy_to_user(usr_buf, buffer, rv);

        return rv;
}


/* Macros for registering module entry and exit points. */
module_init( proc_init );
module_exit( proc_exit );

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Seconds Module");
MODULE_AUTHOR("SGG");


//=======================================================
//Shell script for seconds.ko
//=======================================================
/*
#!/bin/bash

# clear the message queue, then
# insert seconds.ko into the kernel and remove it again, then
# display the messages as a result of doing so

sudo -p "osc"
sudo dmesg -c 
clear 


printf "beginning script...\n\n"
sudo insmod seconds.ko

cat /proc/seconds
sleep 1
cat /proc/seconds
sleep 2
cat /proc/seconds
sleep 3

sudo rmmod seconds 
sudo dmesg 

printf "\n...end of script\n\n"
*/


//=======================================================
//Possible OUTPUT from seconds.ko
//=======================================================

/*
beginning script...

beginning script...

seconds in /proc/read is: 0
seconds in /proc/read is: 1
seconds in /proc/read is: 3
[  499.388417] /proc/seconds created
[  499.388419] In proc_init(), Jiffies is: 4295017001 and HZ is: 250
[  499.388419] In proc_init(), initial time is: 4295017001
[  499.389574] In proc_read(), seconds is: 0
[  500.431821] In proc_read(), seconds is: 1
[  502.450997] In proc_read(), seconds is: 3
[  505.457218] In proc_exit(), jiffies is: 4295018518
[  505.457220] /proc/seconds removed

...end of script
*/

//=======================================================
//End of OUTPUT from seconds.ko
//=======================================================

