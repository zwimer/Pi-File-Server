# Pi-File-Server

This is a little file server meant for running on an internal network on a raspberry pi, built in an extensible way.

## Disclaimer

This server has *not* been put through rigorous security tests. It may be vulnerable to hackers. I wrote this to be a small server on my internal network.

## Requirements

This application requires the use of C++11, cmake 3.0+ (to build) and the library boost; more specifically boost/interprocess.

## Installation Instructions:

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
cmake .. && make
```

## Usage
This application takes only one argument, the port number to run on. Run it as follows:
```bash
./a.out <port>
```
