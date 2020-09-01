# Dynamic Matching in Practice 
## Downloading DynMatch: 
You can download DynMatch with the following command line:

```console
git clone https://github.com/DynGraphLab/DynMatch
```

## Compiling DynMatch: 
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

Licence
=====
The program is licenced under MIT licence.
If you publish results using our algorithms, please acknowledge our work by quoting the following paper:



```
@inproceedings{DBLP:conf/esa/Henzinger0P020,
  author    = {Monika Henzinger and
               Shahbaz Khan and
               Richard Paul and
               Christian Schulz},
  title     = {Dynamic Matching Algorithms in Practice},
  booktitle = {28th Annual European Symposium on Algorithms, {ESA} 2020, September
               7-9, 2020, Pisa, Italy (Virtual Conference)},
  pages     = {58:1--58:20},
  year      = {2020},
  crossref  = {DBLP:conf/esa/2020},
  url       = {https://doi.org/10.4230/LIPIcs.ESA.2020.58},
  doi       = {10.4230/LIPIcs.ESA.2020.58},
  timestamp = {Wed, 26 Aug 2020 16:56:07 +0200},
  biburl    = {https://dblp.org/rec/conf/esa/Henzinger0P020.bib},
  bibsource = {dblp computer science bibliography, https://dblp.org}
}

@proceedings{DBLP:conf/esa/2020,
  editor    = {Fabrizio Grandoni and
               Grzegorz Herman and
               Peter Sanders},
  title     = {28th Annual European Symposium on Algorithms, {ESA} 2020, September
               7-9, 2020, Pisa, Italy (Virtual Conference)},
  series    = {LIPIcs},
  volume    = {173},
  publisher = {Schloss Dagstuhl - Leibniz-Zentrum f{\"{u}}r Informatik},
  year      = {2020},
  isbn      = {978-3-95977-162-7},
  timestamp = {Wed, 26 Aug 2020 16:29:43 +0200},
  biburl    = {https://dblp.org/rec/conf/esa/2020.bib},
  bibsource = {dblp computer science bibliography, https://dblp.org}
}
```
