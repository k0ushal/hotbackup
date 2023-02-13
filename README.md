
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

# Work status
## Implemented features
- file changes tracking and backup
- nonblocking implementation
- delete files having `delete_` prefix
- delete files with `delete_ISOtimestamp_` prefix
- Add audit logs for all operations to the log file

## Pending features
- view/filter log file through console options

When the application is executed, it will start monitoring file changes and take action on created/modified files. There are no console options to view/filter log file yet. The logs will have to be viewed through the log file `audit.log` which is created in the current directory.

There's about 4 more hours worth of work left to finish the remaining features.

# Code design
The application has been implemented for Linux but it has been designed keeping in mind the support for multiple OS in the future. All functionality is accessed exclusively through interfaces allowing to change implementation as per the intended OS.

The code is organized as follows:

`${ROOT}/interfaces`: headers declaring interface classes

`${ROOT}/linux`: linux-specific code

`${ROOT}/unit-tests`: Unit tests

`${ROOT}`: common sources (common to all platforms)


While the original plan was to support multiple OS, I've taken the liberty to use hardcoded linux style file paths (in the interest of time) and other little things which will prevent us from building the code cross platform as it is. However, the main design is fully portable as described further.

## High level overview
The application has 3 main parts:


```
┌─────────────┐    ┌─────────────┐    ┌─────────────┐
│  Observers  │ -> │    Queue    │ -> │   Workers   │
└─────────────┘    └─────────────┘    └─────────────┘
```

The Observers watch for changes to the selected hot target(s) and add the changed file info to the queue. The backup workers pick items from the queue and process them.

## Interfaces
`IFileNotifier - `Interface exposing `inotify` functionality that allows watching for changes to target files/directories. By itself it is incapable of knowing when new changes are available and has to keep calling the non-blocking function `get_changed_files_list()` and inspect its return value to see if new data was made available. To overcome this limitation, the `IFileObserver` interface was introduced.
&nbsp;

`IFileObserver - `This interface introduces the capability to poll filesystem objects (viz., `ISubject`) for changes. It then notifies the user and returns the respective `ISubject` that was signaled. 
&nbsp;

`IFileObserverSubject - ` This interface combines the `ISubject` and `IFileNotifier` properties. The user can pass the `ISubject` part to `IFileObserver` and when they get notified of changes to the `ISubject`, they can use the `IFileNotifier` interface to read all of the changes.
&nbsp;

`IFileBackupQueue - `an interface representing the file backup queue.
&nbsp;

`IBackupManager -`The Backup manager supports extensions `(IFileBackupPlugin)` that allow pre-processing a queued item before it is backed up by the Backup Manager. The `delete_` and `delete_ISOtimestamp_` functionality is implemented in such an extension `(FileDeleterPlugin)`

```
   ┌──────────────────────────┐
   │      Backup Manager      │
   ├──────────────────────────┤
   │                          │
   │                          │
   ├──────────────────────────┤
O────      Extension 1        │
   ├──────────────────────────┤
O────      Extension 2        │
   ├──────────────────────────┤
   │                          │
   │                          │
   │                          │
   └──────────────────────────┘
```

## Implementation
`FileObserverSubject - `implements the `FileObserverSubject` interface allowing it to be watched for changes by the `IFileObserver` and then use the `IFileNotifier` properties to fetch those changes.
&nbsp;

`FileObserver - `implements the `IFileObserver` interface

