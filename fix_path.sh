#!/bin/bash

FILE="test_delete.c"
BASE="$(pwd)"

sed -i "s|/home/docteurbadluck/Desktop/rearanged_webserver/|$BASE/|g" "$FILE"