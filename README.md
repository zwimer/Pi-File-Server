# Pi-File-Server
A little file server to be run on a raspberry pi

## --- UNDER CONSTRUCTION ---


## Disclaimer


## Requirements

This application requires the use of C++11 and the library boost, more specifically boost/interprocess.

## Installation Instructions:

First, cd into the directory you would like to install this application in

Then git clone this repository
```bash
git clone https://github.com/zwimer/Pi-File-Server
```

Then compile the program as follows
```bash
g++ Pi-File-Server/*.cpp -pthread -o a.out
```

### Cmake users

First, cd into the directory you would like to install this application in

Then git clone this repository
```bash
git clone https://github.com/zwimer/Pi-File-Server
```

Create your build directory as follows
```bash
mkdir Pi-File-Server/build && cd Pi-File-Server/build/
```

After that run cmake and make with the command below
```bash
cmake ../Pi-File-Server/cmake && make
```

Finally relocate a.out as follows
```bash
mv a.out ../../
```

## Usage
This application takes no arguments. Just open it like you would any executable. From the directory a.out is in, run the following
```bash
./a.out
```
