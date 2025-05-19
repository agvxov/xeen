# Xeen
> pseudo terminal renderer

Xeen renders its input into a PNG as if it was a terminal.
It understands what formatting related ANSI escape codes are.

## Usage
The intended use of Xeen is to create screenshot like pictures of code and such,
without monitor-size or shaky-hand interference.

```
    xighligth source/main.c | xeen
```
Output:
![documentation/example.png](documentation/example.png)

Following the UNIX philosophy however,
Xeen itself does not highlight code for you,
instead it assumes the input was intelligently annotated with ANSI escapes.

To highlight code, we recommend:
* [xighlight](https://bis64wqhh3louusbd45iyj76kmn4rzw5ysawyan5bkxwyzihj67c5lid.onion/xolatile/xolatilization.git) (onion)
* [nvcat](https://github.com/brianhuster/nvcat)
* [bat](https://github.com/sharkdp/bat)

## CLI
```
xeen [options]
  -h        : print help and exit
  -v        : print version and exit
  -o <file> : specify output
  -t <n>    : set tab width
  -s <n>    : set font size
  -f <file> : set font
  -F <file> : set font directory
```
