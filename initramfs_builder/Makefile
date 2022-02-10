.PHONY: all
all: initramfs.cpio.gz

BUSYBOX_MIRRORS := https://busybox.net/downloads
BUSYBOX_FNAME := busybox-1.34.1
BUSYBOX_PACKED_EXT := .tar.bz2
BUSYBOX_SHA256 := 415fbd89e5344c96acf449d94a6f956dbed62e18e835fc83e064db33a34bd549
BUSYBOX_SRC_DIR := src

INITRAMFS_DIR := initramfs

busybox$(BUSYBOX_PACKED_EXT):
	../tools/download --timeout 60 --output $@ --sha256 $(BUSYBOX_SHA256) \
		$(foreach mirror,$(BUSYBOX_MIRRORS),--url $(mirror)/$(BUSYBOX_FNAME)$(BUSYBOX_PACKED_EXT))

$(BUSYBOX_SRC_DIR)/.unpacked: busybox$(BUSYBOX_PACKED_EXT)
	$(RM) -r $(BUSYBOX_SRC_DIR)
	mkdir $(BUSYBOX_SRC_DIR)
	tar -C $(BUSYBOX_SRC_DIR) --strip-components=1 -xf $<
	touch $@

$(BUSYBOX_SRC_DIR)/.config: $(BUSYBOX_SRC_DIR)/.unpacked
	$(MAKE) -C $(BUSYBOX_SRC_DIR) clean
	$(MAKE) -C $(BUSYBOX_SRC_DIR) defconfig
	sed -e 's/.*CONFIG_STATIC is not set.*/CONFIG_STATIC=y/' -i $@

$(BUSYBOX_SRC_DIR)/busybox: $(BUSYBOX_SRC_DIR)/.config
	$(MAKE) -C $(BUSYBOX_SRC_DIR)

$(BUSYBOX_SRC_DIR)/_install/bin/busybox: $(BUSYBOX_SRC_DIR)/busybox
	$(MAKE) -C $(BUSYBOX_SRC_DIR) install

$(INITRAMFS_DIR)/bin/busybox: $(BUSYBOX_SRC_DIR)/_install/bin/busybox
	$(RM) -r $(INITRAMFS_DIR) && mkdir $(INITRAMFS_DIR)
	cp -r $(BUSYBOX_SRC_DIR)/_install/. $(INITRAMFS_DIR)

INIT_FILES := init new_init

$(addprefix $(INITRAMFS_DIR)/,$(INIT_FILES)): $(INITRAMFS_DIR)/%: % $(INITRAMFS_DIR)/bin/busybox
	cp $< $@

KERNEL_VER := $(shell uname -r)
# TODO: generate this with modprobe
MODULES := kernel/net/9p/9pnet_virtio.ko \
	kernel/net/9p/9pnet.ko \
	kernel/fs/9p/9p.ko \
	kernel/fs/fscache/fscache.ko \
	kernel/fs/netfs/netfs.ko \
	kernel/fs/overlayfs/overlay.ko
MODULES := $(foreach module,$(MODULES),/lib/modules/$(KERNEL_VER)/$(module))

$(addprefix $(INITRAMFS_DIR)/,$(notdir $(MODULES))) &: $(MODULES) $(INITRAMFS_DIR)/bin/busybox
	cp $(MODULES) $(INITRAMFS_DIR)

initramfs.cpio.gz: $(addprefix $(INITRAMFS_DIR)/,$(INIT_FILES)) $(addprefix $(INITRAMFS_DIR)/,$(notdir $(MODULES)))
	cd $(INITRAMFS_DIR) && find . | cpio -o -H newc -R +0:+0 | gzip -9 > ../$@

.PHONY: clean
clean:
	$(RM) -r $(INITRAMFS_DIR) initramfs.cpio.gz

.PHONY: distclean
distclean: clean
	$(RM) -r busybox$(BUSYBOX_PACKED_EXT) $(BUSYBOX_SRC_DIR)
