# mysync: Directory Synchronization Utility

mysync is a command-line utility designed for the CITS2002 Project 2 in 2023. It enables the synchronization of the contents of two or more directories. For more details about the project, visit [CITS2002 Systems Programming - Project 2 2023](https://teaching.csse.uwa.edu.au/units/CITS2002/projects/project2.php).

## Project Overview

In this project, files are considered identical if they exist in the same relative location below a 'top-level' directory, have the same filename, and identical contents. mysync aims to facilitate the synchronization process efficiently.

## Options

- `-a`: By default, 'hidden' and configuration files (those beginning with a '.' character) are excluded from synchronization. This option includes all files, regardless of their naming convention.
  
- `-i <pattern>`: Ignores filenames matching the indicated pattern, considering all other filenames for synchronization. This option can be used multiple times.
  
- `-n`: Identifies files to be copied for synchronization but does not execute the synchronization process. This option also activates verbose mode (`-v`).

- `-o <pattern>`: Considers only filenames matching the indicated pattern for synchronization, ignoring others. This option is repeatable.

- `-p`: Preserves the modification time and file permissions of the original file for the newly copied file.

- `-r`: Recursively processes directories found within the named directories for synchronization.

- `-v`: Enables verbose mode, providing detailed output of mysync's actions on stdout.

## Usage

To utilize mysync, follow these steps:

1. **Compile the program with make:**
```bash
make
```

2. **Execute to synchronize directories providing the options you need:**
```bash
./mysync [options] directory1 directory2 [directory3 ...]
```