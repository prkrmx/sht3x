#!/bin/bash

sudo /usr/bin/gdb "$@"


# To use this file need to add to /etc/sudoers using 'sudo visudo' line: 'max ALL=(ALL) NOPASSWD:/usr/bin/gdb'