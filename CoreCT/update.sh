rm -r -f src/*
tar xvf 1.tar.bz2 -C src
mkdir -p src/build
cd src/build && cmake .. && make -j`grep -c ^processor /proc/cpuinfo` && cd ../..

cp src/init.mac .
chmod a-x src/build/*.mac

`find src/build -maxdepth 1 -type f -perm /001` `cat params`


