# path

[![Build Status](https://api.travis-ci.org/kevincox/path.png?branch=master)](https://travis-ci.org/kevincox/path)

`path` is a simple command for manipulating paths from shell scripts.  It takes
paths as arguments and outputs them on stdout.  It is intended to be fast and
simple.

`path` does all of its work as string manipulation, this means that files do
not need to exist and results are deterministic.  If you want to access the
filesystem use readlink.  The only system access that is done is getting the
current directory when a directory to use is not provided.

All flags are documented in `--help`, you can also easily see the documentation
in the source if you don't have `path` installed.

`path` is distributed along the terms of the (permissive) zlib licence.  See the
`LICENCE` file for details.

## Examples
```sh
cd /home/kevincox/
# Basic, you don't need path really.
path foo/bar    #-> foo/bar
path -a foo/bar #-> /home/kevincox/foo/bar

# Get parts of a path.
path -E ../random_folder/archive.tar.gz  #-> tar.gz
path -B my.awesome.file.txt              #-> my.awesome.file
path -L /tmp/secret.socket               #-> secret.socket
# Or for "simple" extension handling use -s.
path -Es ../random_folder/archive.tar.gz #-> gz

# And the fancy stuff.
path -n /foo/bar/./.././///random/hi/mom/../dad/./ #-> /foo/random/hi/dad/
path -a /my/dir bar/../../foo                      #-> /my/foo
```

## Building

`path` uses the [Rub](https://github.com/kevincox/rub) build system.  `rub`
should do it.  Run the tests with `rub :test` and install with `rub :install`.
