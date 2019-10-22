#!/bin/bash

./main | sort -nr | head -n 10 > top10.txt
