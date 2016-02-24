#!/bin/bash

# Clones dependencies
# !Run this script from the project root!
# In terminal cd to project root then "sh cloneaddons.sh"

# switch to addons directory
cd ../../../addons

# passing no arguments means read only
if [ -z $1 ]; then
CLONE="git clone https://github.com/"
echo "Cloning read-only"
else
CLONE="git clone git@github.com:"
echo "Cloning writable"
fi

# git clone specific revision of the addons
# this ensures that things won't break if repositories are updated

${CLONE}brucelane/ofxImGui
cd ofxImGui
# git checkout ???
cd ..

${CLONE}bakercp/ofxIpVideoGrabber
cd ofxIpVideoGrabber
git checkout 8f0775dc988175003d0aeb6d3795e6ff94293235
cd ..

${CLONE}kylemcdonald/ofxCv
cd ofxCv
git checkout 005c0a7ae35e45fc505546599e06ff3b9724ac11
cd ..

${CLONE}outsidecontext/ofxRemoteUI
cd ofxRemoteUI
# git checkout 3c73fbed1e004c8bd0c542985088b14a6969a648
cd ..

${CLONE}outsidecontext/ofxArgs
cd ofxArgs
# git checkout 9dfbf491591ebe0f8a83fb4ab675116b9d1040ce
cd ..

${CLONE}julapy/ofxFFT
cd ofxFFT
git checkout f33c6d581b9a912eb3d24d2413c5aced866cf5b9
cd ..

${CLONE}arturoc/ofxTween
cd ofxTween
git checkout 549da2d35ef8fbd9d7553f053c549a7c66312d3d
cd ..

${CLONE}Flightphase/ofxFTGL
cd ofxFTGL
git checkout 3a562f3ca3fb002b728f6949810169587bfa751b
cd ..

${CLONE}outsidecontext/ofxImageSequence
cd ofxImageSequence
git checkout 18507f2c7f7af877ee7d0e11a072cf24eb503707
cd ..

${CLONE}lukemalcolm/ofxTextSuite
cd ..

${CLONE}Flightphase/ofxProjectorBlend


# final step is to cd back to the start dir
cd ../apps/invisibleTreasure