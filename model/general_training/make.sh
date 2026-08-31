#!/bin/bash
aux="$(uname -s)"
if [ "$aux" = "Darwin" ]; then
    gcc -dynamiclib -fPIC training.c -o libtraining.dylib
elif [ "$aux" = "Linux" ]; then
    gcc -shared -fPIC training.c -o libtraining.so
fi
    
