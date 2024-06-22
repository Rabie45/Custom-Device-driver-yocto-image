# Custom Device driver yocto image
To add a custom device driver to Yocto
## Steps 
  - Go to ```PATH/poky/meta-skeleton/recipes-kernel``` u would find ```hello-mod``` file this an example for device driver for yocto
    
![image](https://github.com/Rabie45/Custom-Device-driver-yocto-image/assets/76526170/7a71c792-e69c-4639-aeaf-358ecba0652a)

  - Take a copy and put in your layer
    
![image](https://github.com/Rabie45/Custom-Device-driver-yocto-image/assets/76526170/def194ef-2ac0-48b8-a018-3e504c051315)

  - Modify the recipe ```hello-kernel.bb``` as you want in my case it would be 
```
SUMMARY = "GPIO kernel module"
DESCRIPTION = "${SUMMARY}"
LICENSE = "CLOSED"

inherit module

SRC_URI = "file://Makefile \
           file://hello.c \
          "

S = "${WORKDIR}"

# The inherit of module.bbclass will automatically name module packages with
# "kernel-module-" prefix as required by the oe-core build environment.

RPROVIDES:${PN} += "kernel-module-hello"
```
  - dont forget to add ur makefile and device driver code (i made a gpio simple driver uploaded with the project) makefile
    
```
obj-m := hello.o

SRC := $(shell pwd)

all:
	$(MAKE) -C $(KERNEL_SRC) M=$(SRC)

modules_install:
	$(MAKE) -C $(KERNEL_SRC) M=$(SRC) modules_install

clean:
	rm -f *.o *~ core .depend .*.cmd *.ko *.mod.c
	rm -f Module.markers Module.symvers modules.order
	rm -rf .tmp_versions Modules.symvers
```

  - Go to conf file of ur layer and add this line  ```IMAGE_INSTALL:append += " hello-kernel"```
  - Untill here every thing is good but the driver is not loaded in the kernel yet it loaded in the image only
  - In conf file also add this line ```KERNEL_MODULE_AUTOLOAD += " hello"``` to automatic load of the ```.ko``` file
    
 ## building the image 
   - ```source oe-init-build-env build```
   - ```bitbake hello-kernel```
   - ``` bitbake diploma-minimal ``` my layer name

 ## SD card 
   - ```cp PATH/poky/build/tmp/deploy/images/raspberrypi3/diploma-minimal-raspberrypi3.wic.bz2 .``` copy the image to the work dir
   - ```bzip2 -dk diploma-minimal-raspberrypi3.wic.bz2 ``` Extract the file
   - ```sudo dd if=diploma-minimal-raspberrypi3.wic of=/dev/sdc status=progress ``` load into the sd card
   - Open the sdcard
   - Go to ```lib/modules/<UR kernel version>``` u have to find ur ko module


```lsmod``` command to see the loaded drivers 

https://github.com/Rabie45/Custom-Device-driver-yocto-image/assets/76526170/12184d06-db46-423d-868e-fe482e5a1744


