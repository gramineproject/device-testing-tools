.PHONY: all
all: qemu-system-x86_64

QEMU_MIRRORS := https://download.qemu.org
QEMU_FNAME := qemu-6.2.0
QEMU_PACKED_EXT := .tar.xz
QEMU_SHA256 := 68e15d8e45ac56326e0b9a4afa8b49a3dfe8aba3488221d098c84698bca65b45
QEMU_SRC_DIR := src

qemu$(QEMU_PACKED_EXT):
	../tools/download --timeout 60 --output $@ --sha256 $(QEMU_SHA256) \
		$(foreach mirror,$(QEMU_MIRRORS),--url $(mirror)/$(QEMU_FNAME)$(QEMU_PACKED_EXT))

$(QEMU_SRC_DIR)/.unpacked: qemu$(QEMU_PACKED_EXT)
	$(RM) -r $(QEMU_SRC_DIR)
	mkdir $(QEMU_SRC_DIR)
	tar -C $(QEMU_SRC_DIR) --strip-components=1 -xf $<
	for patch in patches/*; do patch --quiet -p1 --directory "$(QEMU_SRC_DIR)" < "$$patch"; done
	touch $@

$(QEMU_SRC_DIR)/.configured: $(QEMU_SRC_DIR)/.unpacked
	$(RM) -r $(QEMU_SRC_DIR)/build
	mkdir $(QEMU_SRC_DIR)/build
	cd $(QEMU_SRC_DIR)/build && ../configure --target-list=x86_64-softmmu --enable-kvm --enable-virtfs
	touch $@

qemu-system-x86_64: $(QEMU_SRC_DIR)/.configured
	$(MAKE) -C $(QEMU_SRC_DIR)/build
	ln -sf $(QEMU_SRC_DIR)/build/qemu-system-x86_64 qemu-system-x86_64
	@echo 'QEMU was built successfully. You might want to install it globally now, or add this directory to your $$PATH.'

.PHONY: clean
clean:
	$(RM) -r qemu-system-x86_64
	$(MAKE) -C $(QEMU_SRC_DIR) clean

.PHONY: distclean
distclean:
	$(RM) -r qemu$(QEMU_PACKED_EXT) $(QEMU_SRC_DIR)
