#!/bin/bash
gcc -O3 -ffast-math -funsafe-math-optimizations -faggressive-loop-optimizations -floop-parallelize-all -fthread-jumps -fearly-inlining -fcse-skip-blocks -fcrossjumping -fsplit-loops tinshell.c -o tinshell
sudo cp -r tinshell /usr/local/bin/tinshell
sudo mkdir -p /usr/local/man/man1/
install -g 0 -o 0 -m 0644 tinshell.1 /usr/local/man/man1/
sudo gzip -f /usr/local/man/man1/tinshell.1
sudo mandb
echo "tinshell has been installed."
