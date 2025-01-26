#!/bin/bash

# Create 64MB disk image
dd if=/dev/zero of=disk.img bs=1M count=64
mkfs.ext2 disk.img

# Mount and populate
mkdir -p mnt
sudo mount -o loop disk.img mnt
sudo mkdir -p mnt/bin
sudo cp lcl.bin mnt/bin/lcl
sudo umount mnt
rmdir mnt
