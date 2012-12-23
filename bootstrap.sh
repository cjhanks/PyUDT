#!/bin/bash 

git submodule init 
git submodule update 

pushd udt 
pushd UDT4 

make -j1 
make install 

popd 
popd 

python setup.py build 
sudo python setup.py install 
