#!/bin/bash

SUDO=$(which sudo)

$SUDO docker run -p 127.0.0.1:1502:1502 -it --rm \
    -v $(pwd):/src 9b127e712631
