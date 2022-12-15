#!/bin/sh

fil="read_content/"$2
echo $fil
{
for a in 20000 40000 80000 120000
do
    echo "block_size = $a for $fil"
    ./"$1" "$fil" "$a"
    echo "----------"
done
} > "read_results/$1_$2_results.txt"