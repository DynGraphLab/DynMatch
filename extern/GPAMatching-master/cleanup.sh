#!/bin/bash


scons program=gpa_matching variant=optimized -j 4 -c 

rm -rf deploy
rm -rf optimized
