#include <linux/init.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <asm/current.h>

MODULE_LICENSE("Dual MIT/GPL");
MODULE_AUTHOR("Liu Yu - source@liuyu.com");
MODULE_DESCRIPTION("Linux Kernel Module: Hello World.");

static __init int hello_init(void)
{
    printk(KERN_ALERT "Hello, world\n");
    printk(KERN_ALERT "current Process is: %s, PID is: %d\n", current->comm, current->pid);
    return 0;
}

static __exit void hello_exit(void)
{
    printk(KERN_ALERT "current Process is: %s, PID is: %d\n", current->comm, current->pid);
    printk(KERN_ALERT "Boodbye, cruel world\n");
}

module_init(hello_init);
module_exit(hello_exit);
