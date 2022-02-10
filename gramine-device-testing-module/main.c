/* These must come before includes. */
#define GRAMINE_TEST_DEV_NAME "gramine_test_dev"
#define pr_fmt(fmt) GRAMINE_TEST_DEV_NAME ": " fmt

#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>

// TODO: add locking

MODULE_AUTHOR("Gramine Authors");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Dummy module just for testing purposes");

#define GRAMINE_TEST_DEV_MINOR_NUMS 1
#define GRAMINE_TEST_DEV_MAX_SIZE (0x100 * PAGE_SIZE)

struct gramine_test_dev_data {
    size_t size;
    char* buf;
};

static struct class* gramine_test_dev_class = NULL;
static dev_t dev_num = 0;
struct cdev cdev;
struct device* device = NULL;

static int gramine_test_dev_open(struct inode* inode, struct file* filp) {
    struct gramine_test_dev_data* data = kmalloc(sizeof(*data), GFP_KERNEL);
    if (!data) {
        return -ENOMEM;
    }

    data->size = 0;
    data->buf = NULL;

    filp->private_data = data;
    return 0;
}

static int gramine_test_dev_release(struct inode* inode, struct file* filp) {
    struct gramine_test_dev_data* data = filp->private_data;
    filp->private_data = NULL;
    if (data) {
        kfree(data->buf);
        kfree(data);
    }
    return 0;
}

static ssize_t gramine_test_dev_write(struct file* filp, const char __user* buf, size_t size,
                                      loff_t* offp) {
    size_t end;
    struct gramine_test_dev_data* data;

    if (*offp < 0) {
        return -EFBIG;
    }
    if (check_add_overflow(size, (size_t)*offp, &end)) {
        return -EFBIG;
    }
    if (end > GRAMINE_TEST_DEV_MAX_SIZE) {
        return -EFBIG;
    }

    data = filp->private_data;
    if (data->size < end) {
        char* tmp_buf = krealloc(data->buf, end, GFP_KERNEL);
        if (!tmp_buf) {
            return -ENOMEM;
        }
        memset(tmp_buf + data->size, 0, end - data->size);
        data->buf = tmp_buf;
        data->size = end;
    }

    if (copy_from_user(data->buf + *offp, buf, size)) {
        /* We might clobber the internal buffer if part of `buf` is invalid, but who cares. */
        return -EFAULT;
    }

    *offp += size;
    return size;
}

static ssize_t gramine_test_dev_read(struct file* filp, char __user* buf, size_t size,
                                     loff_t* offp) {
    struct gramine_test_dev_data* data;
    size_t copy_size;

    if (*offp < 0) {
        return -EFBIG;
    }

    data = filp->private_data;
    if (data->size <= *offp) {
        return 0;
    }

    copy_size = min((size_t)(data->size - *offp), size);
    if (copy_to_user(buf, data->buf + *offp, copy_size)) {
        return -EFAULT;
    }

    *offp += copy_size;
    return copy_size;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .llseek = default_llseek,
    .read = gramine_test_dev_read,
    .write = gramine_test_dev_write,
    .open = gramine_test_dev_open,
    .release = gramine_test_dev_release,
};

static void module_cleanup(void) {
    if (device) {
        device_destroy(gramine_test_dev_class, dev_num);
    }
    if (dev_num) {
        cdev_del(&cdev);
        unregister_chrdev_region(dev_num, GRAMINE_TEST_DEV_MINOR_NUMS);
    }
    if (gramine_test_dev_class) {
        class_destroy(gramine_test_dev_class);
    }
}

/* TODO: for some reason does not work. It runs successfully, but perms stay 0600. */
static int gramine_test_dev_change_perms_uevent(struct device* dev, struct kobj_uevent_env* env) {
    /* Let everyone use this device. */
    return add_uevent_var(env, "DEVMODE=%#o", 0666);
}

static int __init gramine_test_dev_init_module(void) {
    int ret;

    gramine_test_dev_class = class_create(THIS_MODULE, GRAMINE_TEST_DEV_NAME);
    if (IS_ERR(gramine_test_dev_class)) {
        ret = PTR_ERR(gramine_test_dev_class);
        gramine_test_dev_class = NULL;
        pr_warn("class_create failed: %d\n", ret);
        goto init_fail;
    }
    gramine_test_dev_class->dev_uevent = gramine_test_dev_change_perms_uevent;

    ret = alloc_chrdev_region(&dev_num, 0, GRAMINE_TEST_DEV_MINOR_NUMS, GRAMINE_TEST_DEV_NAME);
    if (ret < 0) {
        pr_warn("alloc_chrdev_region failed: %d\n", ret);
        goto init_fail;
    }

    cdev_init(&cdev, &fops);
    ret = cdev_add(&cdev, dev_num, 1);
    if (ret < 0) {
        pr_warn("cdev_add failed: %d\n", ret);
        goto init_fail;
    }

    device = device_create(gramine_test_dev_class, /*parent=*/NULL, dev_num, /*drvdata=*/NULL,
                           GRAMINE_TEST_DEV_NAME);
    if (IS_ERR(device)) {
        ret = PTR_ERR(device);
        device = NULL;
        pr_warn("device_create failed: %d\n", ret);
        goto init_fail;
    }

    return 0;

init_fail:
    module_cleanup();
    return ret;
}

static void __exit gramine_test_dev_cleanup_module(void) {
    module_cleanup();
}

module_init(gramine_test_dev_init_module);
module_exit(gramine_test_dev_cleanup_module);
