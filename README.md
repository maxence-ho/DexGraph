# DexGraph
Modified DexDump to build control flow graph from classes.dex

A white paper for this project is available in the project root, [here](https://github.com/ChiminhTT/DexGraph/blob/master/dexgraph-reverse-engineering.pdf)

## Dependencies
Dependencies are statically linked, hence they have to be built from sources.
Dexgraph dependencies are:
  - zlib

### Building the dependencies
#### zlib
- Download sources [here](http://www.zlib.net/zlib-1.2.11.tar.gz) (zlib version 1.2.11)
- Inside src folder, call: ```./configure --static```
- ```make```
- libz.a should now be available. Move it into **Dexgraph/deps/zlib/**

## Run
Enter the following command:

```dexdump -D $PATH_TO_DEX_CLASSES```

The edg file will be at binary root.

The dot output is dumped to stdout. You can pipe it to a file.
