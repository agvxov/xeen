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
Ouput:
![documentation/example.png](documentation/example.png)

Following the UNIX philosphy however,
Xeen itself does not highlight code for you,
instead it assumes its input was intelligently formatted.

To highlight code, we recommend:
+ [xighlight]()
+ [bat](https://github.com/sharkdp/bat)

## CLI
```
xeen [options]
  -h        : print help and exit
  -v        : print version and exit
  -o <file> : specify output
  -t <n>    : set tab width
  -s <n>    : set font size
  -f <file> : set font
```
