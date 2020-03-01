#include <asm/current.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/types.h>

MODULE_LICENSE("Dual MIT/GPL");
MODULE_AUTHOR("Liu Yu - source@liuyu.com");
MODULE_DESCRIPTION("Linux Kernel Module: Character Device Driver.");

typedef struct file *FILEPTR;
typedef struct inode *INODEPTR;

dev_t gDev = 0;

loff_t _llseek(struct file *pf, loff_t offset, int n)
{
    return 0;
}

ssize_t _read(struct file *pf, char __user *buffer, size_t size, loff_t *offset)
{
    return snprintf(buffer, size, "%s", "Character Device Driver");
}

ssize_t _write(struct file *pf, const char __user *buffer, size_t size, loff_t *offset)
{
    return size;
}

long _ioctl(FILEPTR pf, unsigned int i, unsigned long l)
{
    return -ENOTTY;
}

int _open(INODEPTR nod, FILEPTR pf)
{
    return 1;
}

int _release(INODEPTR nod, FILEPTR pf)
{
    return 1;
}

int snprintf(char *buf, size_t size, const char *fmt, ...)
{
    va_list args;
    int i;

    va_start(args, fmt);
    i = vsnprintf(buf, size, fmt, args);
    va_end(args);

    return i;
}
// EXPORT_SYMBOL(snprintf);

static __init int driver_init(void)
{
    int result;

    printk(KERN_ALERT "Character Device Driver\n");
    printk(KERN_ALERT "current Process is: %s, PID is: %d\n", current->comm, current->pid);

    // alloc device id
    printk(KERN_INFO "alloc device id\n");
    result = alloc_chrdev_region(&gDev, 0, 4, "_test_02_character_device_driver_");
    if (result < 0)
    {
        printk(KERN_ERR "alloc device id fail\n");
        return result;
    }
    printk(KERN_INFO "device id: %u\n", gDev);

    // file operation
    printk(KERN_INFO "file operation\n");
    struct file_operations fops = {
        .owner = THIS_MODULE,
        .llseek = _llseek,
        .read = _read,
        .write = _write,
        .compat_ioctl = _ioctl,
        .open = _open,
        .release = _release,
    };
    printk(KERN_INFO "file operation: %p\n", &fops);

    return 0;
}

static __exit void driver_exit(void)
{
    printk(KERN_ALERT "current Process is: %s, PID is: %d\n", current->comm, current->pid);
    printk(KERN_ALERT "Character Device Driver Exist\n");

    // release device id
    printk(KERN_INFO "release device id[%u]\n", gDev);
    unregister_chrdev_region(gDev, 4);
}


EXPORT_SYMBOL(_llseek);
EXPORT_SYMBOL(_read);
EXPORT_SYMBOL(_write);
EXPORT_SYMBOL(_ioctl);
EXPORT_SYMBOL(_open);

module_init(driver_init);
module_exit(driver_exit);
