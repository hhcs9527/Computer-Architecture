#!/bin/bash
g++ project.cpp -o project
for org in *.org; do
    for lst in *.lst; do
        ./project $org $lst index.rpt
        #./verify $org $lst index.rpt
    done
done