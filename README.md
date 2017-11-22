# OS Loader
[multiboot Specification] (https://www.gnu.org/software/grub/manual/multiboot/multiboot.html)


# How to Load

in grub.cfg

```
set timeout=1
set default=0

menuentry "rtems"{

        multiboot /osloader
        module /kernel_64.elf
        boot
}


```

# support
* load elf_64 image
* detect the load address from the image itself
* load binary
