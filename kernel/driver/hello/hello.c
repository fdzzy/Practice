#include <linux/init.h> /* Needed for the macros */
#include <linux/module.h> /* Needed for all modules */
MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Joey");

static int __init hello_init(void) {
    printk(KERN_ALERT "Hello, world!\n");
    return 0;
}

static void __exit hello_exit(void) {
    printk(KERN_ALERT "Good bye, cruel world!\n");
}

module_init(hello_init);
module_exit(hello_exit);
