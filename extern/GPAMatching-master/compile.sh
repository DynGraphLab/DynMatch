#!/bin/bash

rm -rf deploy

scons program=gpa_matching variant=optimized -j 4 

mkdir deploy
cp ./optimized/gpa_matching deploy/

rm -rf ./optimized
