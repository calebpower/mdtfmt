# Markdown Table Formatter (mdtfmt)

I was tired of manually formatting Markdown tables in documentation for a project
I was working on, and for whatever reason I couldn't find a simple command-line
Markdown table formatter. So I threw this one together.

It probably ain't pretty, so use it at your own risk.

## Features

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

## Building

If you're gonna use the Makefile, you should have `make` and `clang`. But it's
not a very complicated project, so you can probably use any modern C++ compiler.

```bash
make
```

This should drop `mdtfmt` in the working directory.

## Usage

The rules to determine what a table is and isn't are pretty loose (and they have
to be, to some degree, because the assumption is that the tables aren't formatted
well if you've resorted to this tool). But good rule of thumb is that the more
well-formatted your tables are, the more likely this tool's going to catch and
format the appropriate lines.

If you want `mdtfmt` to spit the prettified text to `stdout`, you can just:

```bash
./mdtfmt uglyFile.txt
```

...and redirect that where you want it to go. I find that `sponge` is probably
going to be helpful if you want to keybind this to your Emacs instance or whatever.

```bash
./mdtfmt uglyFile.txt | sponge uglyFile.txt
```

I personally use this tool as a pre-commit
[hook](https://www.atlassian.com/git/tutorials/git-hooks) for my documentation
repos (for when I'm using something like [mdBook](https://rust-lang.github.io/mdBook)
for static documentation sites or something). On a FreeBSD machine (which I
frequently use), I have the following script at `.git/hooks/pre-commit` (ensuring
that the execution bit is set):

```bash
#!/usr/local/bin/bash

if [ "" == "$(which mdtfmt)" ]; then
  printf 'missing calebpower/mdtfmt\n'
  exit 1
fi

if [ "" == "$(which sponge)" ]; then
  printf 'missing moreutils/sponge\n'
  exit 1
fi

for chg in $(git diff --cached --name-only); do
  printf 'mdtfmt: formatting %s\n' "${chg}"
  mdtfmt $chg | sponge $chg
done
```

Obviously, if you're going to copypasta that script you should make sure that
you've set the shebang correctly if you're using a different shell or bash is
somewhere else on your OS (Linux distros tend to have it at `/bin/bash` whereas
FreeBSD has it at `/usr/local/bin/bash`). The script essentially just ensures
that the modified Markdown files are formatted before they're committed.

## Copyright / License / Disclaimer

Copyright (c) 2024 Caleb L. Power.

Licensed under the MIT license. I would not trust my life on this tool... you
should save backups if you're gonna use it. Feel free to make PRs if you really
wish to do so.
