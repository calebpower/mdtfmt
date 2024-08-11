all : clean mdtfmt
	printf -- '-----\ndone!\n'

mdtfmt:
	clang++ mdtfmt.cc -o mdtfmt

clean:
	rm -f ./mdtfmt ./mdtfmt.core
