#!/bin/bash
sudo rm -rf /usr/local/man/man1/tinshell.1.gz
sudo rm -rf /usr/local/bin/tinshell
sudo mandb
echo "tinshell has been uninstalled."
