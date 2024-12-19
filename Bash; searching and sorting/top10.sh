#!/bin/bash
#JemelyR, lab1

if [[ -r "$1" ]]; then
  echo "| Area | Doses administered |"
  echo "| :--------- | --------: |"
  find . -name $1 -exec grep 'All' {} \; | tr ',' '\t' | cut -f 2,10 | tr -s '\t' ',' | sort -t$',' -k2 -nr | head -10 | tr -s ',' '|' | sed -r 's/(^|$)/|/g'

   exit 1
else
  echo error: file missing or unreadable
  exit 1
fi



