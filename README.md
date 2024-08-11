# Markdown Table Formatter (mdtfmt)

I was tired of manually formatting Markdown tables in documentation for a project
I was working on, and for whatever reason I couldn't find a simple command-line
Markdown table formatter. So I threw this one together.

It probably ain't pretty, so use it at your own risk.

## Building

If you're gonna use the Makefile, you should have `make` and `clang`. But it's
not a very complicated project, so you can probably use any modern C++ compiler.

```sh
make
```

This should drop `mdtfmt` in the working directory.

## Usage

I find that `sponge` is probably helpful, because this tool is just gonna spit
the output to `stdout`. So you can probably just:

```sh
./mdtfmt uglyFile.txt | sponge > prettyFile.txt
```

## Copyright / License / Disclaimer

Copyright (c) 2024 Caleb L. Power.

Licensed under the MIT license. I would not trust my life on this tool... you
should save backups if you're gonna use it. Feel free to make PRs if you really
wish to do so.
