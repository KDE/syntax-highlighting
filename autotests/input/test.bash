#!/bin/bash

for i in `ls tests/auto/output/*.html`; do
    refFile=`echo $i | sed -e s,build,src, | sed -e s,output,reference, | sed -e s,.html,.ref.html,`
    cp -v $i $refFile
done
