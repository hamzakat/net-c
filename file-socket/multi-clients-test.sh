#!/bin/bash

for N in {1..40}
do
    python3 client.py &
done
wait