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

If you want `mdtfmt` to spit the prettified text to `stdout`, you can just:

```sh
./mdtfmt uglyFile.txt
```

...and redirect that where you want it to go. I find that `sponge` is probably
going to be helpful if you want to keybind this to your Emacs instance or whatever.

```sh
./mdtfmt uglyFile.txt | sponge > uglyFile.txt
```

The rules to determine what a table is and isn't are pretty loose (and they have
to be, to some degree, because the assumption is that the tables aren't formatted
well if you've resorted to this tool). But good rule of thumb is that the more
well-formatted your tables are, the more likely this tool's going to catch and
format the appropriate lines.

### Features

There are a few things this thing will do for you:

- Line up your columns.
- Add trailing pipes to cells of your last column if you forgot to slap a pipe at the end.
- Preserve your column alignments.
- Obey escaped pipes (and escaped slashes next to pipes, etc.)
- Pad your table with newlines above and below (if it doesn't have them already)

There are a bunch of things it doesn't do:
- Assume your text is a table if it's missing at least some semblence of an
  alignment row/divider.
- Assume your text is a table if it doesn't start with a pipe.
- Build a table out of mahogony.
- Ship custom tables.
- Spill coffee on a table.

## Copyright / License / Disclaimer

Copyright (c) 2024 Caleb L. Power.

Licensed under the MIT license. I would not trust my life on this tool... you
should save backups if you're gonna use it. Feel free to make PRs if you really
wish to do so.
