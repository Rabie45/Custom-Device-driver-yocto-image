#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/moduleparam.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/gpio.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Rabie");
MODULE_DESCRIPTION("ٌGPIO Driver");
/*buffer for data*/
static char buffer[255];
static int buffer_pointer;
static dev_t my_device_nr;
static struct class *my_class;
static struct cdev my_device;

// Called when driver file caled
static ssize_t driver_read(struct file *File, char *user_buffer, size_t count, loff_t *offs)
{
    printk(KERN_INFO "%s device read was called\n", __FUNCTION__);

    int to_copy, not_copied, delta;
    to_copy = min(count, buffer_pointer);
    not_copied = copy_to_user(user_buffer, buffer, to_copy);
    delta = to_copy - not_copied;
    return delta;
}
static ssize_t driver_write(struct file *File, const char *user_buffer, size_t count, loff_t *offs)
{
    printk(KERN_INFO "%s device write was called\n", __FUNCTION__);
    char value;
    int to_copy, not_copied, delta;
    to_copy = min(count, sizeof(value));
    not_copied = copy_from_user(&value, user_buffer, to_copy);
    switch (value)
    {
    case '0':
        gpio_set_value(4, 0);
        break;
    case '1':
        gpio_set_value(4, 1);
        break;
    default:
        printk(KERN_INFO "%s Invalid\n", __FUNCTION__);
        break;
    }
    buffer_pointer = to_copy;
    delta = to_copy - not_copied;
    return delta;
}

static int driver_open(struct inode *device_file, struct file *instance)
{
    printk(KERN_INFO "%s device open was called\n", __FUNCTION__);
    return 0;
}
static int driver_close(struct inode *device_file, struct file *instance)
{
    printk(KERN_INFO "%s device close was called\n", __FUNCTION__);
    return 0;
}
static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = driver_open,
    .release = driver_close,
    .read = driver_read,
    .write = driver_write};

#define DRIVER_NAME "GPIODriver"
#define DRIVER_CLASS "MyModuleClass"

static int __init ModuleInit(void)
{

    printk(KERN_INFO "Hello world\n");
    if (alloc_chrdev_region(&my_device_nr, 0, 1, DRIVER_NAME) < 0)
    {
        printk(KERN_INFO "%s Couldnt registered device number \n", __FUNCTION__);
    }
    printk(KERN_INFO "%s registered device number major:%d, minor:%d\n", __FUNCTION__, my_device_nr >> 20, my_device_nr && 0xfffff);

    /* create device class
     */
    if ((my_class = class_create(THIS_MODULE, DRIVER_CLASS)) == NULL)
    {
        printk(KERN_INFO "%s Couldnt registered device number \n", __FUNCTION__);
        goto ClassError;
    }
    if (device_create(my_class, NULL, my_device_nr, NULL, DRIVER_NAME) == NULL)
    {
        printk(KERN_INFO "%s Couldnt registered device number \n", __FUNCTION__);
        goto FileError;
    }
    cdev_init(&my_device, &fops);

    if (cdev_add(&my_device, my_device_nr, 1) == -1)
    {
        printk(KERN_INFO "%s Couldnt registered device number \n", __FUNCTION__);
        goto AddError;
    }
    if (gpio_request(4, "rpi-gpio-4"))
    {
        printk(KERN_INFO "%s Couldnt Allocate gpio \n", __FUNCTION__);
        goto AddError;
    }
    if (gpio_direction_output(4, 0))
    {
        printk(KERN_INFO "%s Couldnt set gpio o output \n", __FUNCTION__);
        goto GPIO4Error;
    }
    return 0;
GPIO4Error:
    gpio_free(4);
AddError:
    device_destroy(my_class, my_device_nr);
FileError:
    class_destroy(my_class);
ClassError:
    unregister_chrdev(my_device_nr, DRIVER_NAME);
    return -1;
}

static void __exit ModuleExit(void)
{
    gpio_set_value(4, 0);
    gpio_free(4);
    cdev_del(&my_device);
    class_destroy(my_class);
    device_destroy(my_class, my_device_nr);
    unregister_chrdev(my_device_nr, DRIVER_NAME);
    printk(KERN_INFO "Goodbye\n");
}

module_init(ModuleInit);
module_exit(ModuleExit);
