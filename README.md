# Dynamic Matching in Practice 
Installation Notes
=====
## Downloading DynMatch: 
You can download DynMatch with the following command line:

```console
git clone https://github.com/DynGraphLab/DynMatch
```

## Compiling KaHIP: 
To compile the codes, just type 
```console
./compile_withcmake.sh. 
```
In this case, all binaries, libraries and headers are in the folder ./deploy/ 

Alternatively use the standard cmake build process:
```console 
mkdir build
cd build 
cmake ../ -DCMAKE_BUILD_TYPE=Release     
make 
cd ..
```

