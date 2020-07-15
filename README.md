# TestTask

##Installation

```
mkdir -p ~/tmp/cfg
cp <repo_path>/config/config.cfg ~/tmp/cfg
```

Set in .bashrc 
```
export OMNIORB_CONFIG=~/tmp/cfg/config.cfg
```

```
cd <repo_path>
cmake . -Bbuild
cd build
make
```

##Launch 
Firstly you have to launch omniNames as background process
```
omniNames -start <port> &
```

And then you launch server.out in build directory
```
./server.out
```

In another terminal you run client.out app
```
./client.out
```
