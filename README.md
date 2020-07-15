# TestTask

## Dependecies
* [omniORB 4.3](https://sourceforge.net/projects/omniorb/files/omniORB/omniORB-4.3.0/)

## Installation

1. `mkdir -p ~/tmp/cfg`
2. `cp config/config.cfg ~/tmp/cfg`
3. Set the environment variable `OMNIORB_CONFIG` to `~/tmp/cfg/config.cfg`
4. `cmake . -Bbuild`
5. `cd build`
6. `make`

## Launch 
Launch omniNames as background process
```
omniNames -start <port> &
```

Launch server.out 
```
build/server.out
```

In another terminal run client.out app
```
build/client.out
```
