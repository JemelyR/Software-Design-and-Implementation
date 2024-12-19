#summarize.sh, JemelyRobles, 1/11/22, makes a summary of files given
#!/bin/bash

##for file in files
##check if redable

for file in "$@"
  do
    basenameV=$(basename $file)
    if [[ ! -r "$file" ]]; then
     echo "error: Unreadable file" >> error.txt
     exit 1
    elif [[ $file = *".sh" ]]; then
     languageV=bash
    elif [[ $file = *".c" ]] || [[ $file = *".h" ]]; then
      languageV=c
    fi      
    echo "##"$basenameV
    echo "'''"$languageV
    sed -e '/^#!/d;/^$/,$d' $file
    echo "'''" 
done
exit 0


