
# Introduction
This code has been compiled with g++ ver 9 and uses no 3rd party libraries. There's a google test setup in place with few unit tests.
The following instructions have been tested on an AWS EC2 instance running Ubuntu 20.04.5 LTS (Focal Fossa) instance.

# Prerequisites
```
sudo apt-get install g++-9 cmake git build-essential
git clone https://github.com/google/googletest.git -b v1.13.0

# Google test build instructions copied from:
# https://github.com/google/googletest/blob/main/googletest/README.md

cd googletest
mkdir build
cd build
cmake .. -DBUILD_GMOCK=OFF
make
sudo make install    # Install in /usr/local/lib by default
```

# Build instructions
```
git clone https://github.com/k0ushal/hotbackup.git
cd hotbackup
make        # Builds the main binary (hotbackup)
make tests  # Builds unit tests (tests)
```

# Run application
```
./hotbackup <hot-files-directory> <backup-directory>
```
The log file is stored at location: `./audit.log`
