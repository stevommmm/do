#!/bin/bash

expected=$(grep '@SUCCESS' test/*.f | wc -l)

output=$(./ok test/* 2>/dev/null)


failed=$(echo "$output" | grep '@FAILURE' | wc -l)
if [[ "$failed" -gt 0 ]]; then
    echo "We caught a @FAILURE, something ran that shouldn't"
    exit 1
fi

succeeded=$(echo "$output" | grep '@SUCCESS' | wc -l)
if [[ "$expected" -eq "$succeeded" ]]; then
    echo "Successfully parsed all testing corpus"
else
    echo "Tripped up somewhere, manually run this through with -v -v"
    exit 1
fi
