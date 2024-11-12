# mysync

`mysync` is a command-line utility that enables efficient synchronisation of the contents of two or more directories. This tool was developed for the CITS2002 Project 2 in 2023. For detailed project information, visit the [CITS2002 Systems Programming - Project 2 2023](https://teaching.csse.uwa.edu.au/units/CITS2002/past-projects/p2023-2/summary.php) page.

## Overview

`mysync` ensures that files are synchronised across directories by examining the contents of each provided directory and copying files that are missing or have been modified. The tool can be used to synchronise directories on different filesystems, such as local directories, USB drives, etc.

## Features and Options

- `-a, --hidden`: Include all files in synchronisation, including hidden and configuration files (those starting with a '.').
- `-i, --ignore <pattern>`: Ignore filenames matching the specified pattern. This option can be repeated to exclude multiple patterns.
- `-n, --nothing`: Perform a dry run, identifying files that would be copied for synchronisation without actually performing the copy. This also enables verbose mode (`-v`).
- `-o, --only <pattern>`: synchronise only filenames matching the specified pattern, ignoring all others. This option can be used multiple times.
- `-p, --preserve`: Preserve the modification time and file permissions of the original file when copying.
- `-r, --recursive`: Recursively process and synchronise directories within the specified directories.
- `-v, --verbose`: Enable verbose mode to provide detailed output of `mysync`'s actions.

### Note

- If the `-i` and `-o` options are used together, the `-i` option takes precedence over the `-o` option. This means that files matching the `-i` pattern will be excluded from synchronisation, even if they match the `-o` pattern.
- If the glob star `*` is used exclusively in the `-i` or `-o` pattern, this will take precedence over any other pattern matching regular expressions provided.

## Usage

### Compilation

To compile `mysync`, use the provided Makefile:

```bash
make
```

### Execution

Run `mysync` with the desired options to synchronise directories:

```bash
./mysync [options] directory1 directory2 [directory3 ...]
```

Here's the revised list of example commands:

## Example Commands

1. **Synchronize two directories, including hidden files:**

    ```bash
    ./mysync -a dir1 dir2
    ```

1. **Synchronize directories while ignoring files matching a pattern:**

    ```bash
    ./mysync -i '*.tmp' dir1 dir2
    ```

1. **Perform a dry run with verbose output:**

    ```bash
    ./mysync -n dir1 dir2
    ```

1. **Synchronize only files matching a specific pattern:**

    ```bash
    ./mysync -o '*.txt' dir1 dir2
    ```

1. **Preserve file attributes during synchronization:**

    ```bash
    ./mysync -p dir1 dir2
    ```
