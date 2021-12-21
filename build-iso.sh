#!/bin/sh
# Create a directory which will be our ISO root.
ISO_ROOT=`mktemp -d`

function cleanup {      
	rm -rf "$WORK_DIR"
}

trap cleanup EXIT
 
# Copy the relevant files over.
cp -v $1 limine.cfg limine/limine.sys \
	  limine/limine-cd.bin limine/limine-eltorito-efi.bin $ISO_ROOT/
 
# Create the bootable ISO.
xorriso -as mkisofs -b limine-cd.bin \
		-no-emul-boot -boot-load-size 4 -boot-info-table \
		--efi-boot limine-eltorito-efi.bin \
		-efi-boot-part --efi-boot-image --protective-msdos-label \
		$ISO_ROOT -o $2
 
# Install Limine stage 1 and 2 for legacy BIOS boot.
./limine/limine-install $2
