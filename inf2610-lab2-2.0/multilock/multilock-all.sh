#!/bin/sh

./multilock --check --outer 100000 --inner 100 --verbose
./multilock --check --outer 10  --inner 1000000 --verbose
