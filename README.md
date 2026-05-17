# ntfstools - An experimental NTFS scanner

This software aims to scan a drive, search for any Windows partitions present, and makes a skeleton of the files and directories within it.

## Motivation

I made this so I can eventually recover any file if my drive gets to a state where data can still be read, but the Windows partition somehow got corrupted.
Certain types of corruption can majorly prevent this software from being able to perform any meaningful work, and so I'm also using this as a learning experience to investigate the different ways a Windows partition might somehow get corrupted.

## Compilation

Run `make`.

## Usage

Usage: `./ntfstools <drive name>`
