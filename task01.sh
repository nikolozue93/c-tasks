#!/bin/sh

mkdir -p "logs" && find . -path "./logs" -prune -o -type f -name "*.log" -exec mv -t logs/ {} +
