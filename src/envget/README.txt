ENVGET

This little program prints out the current process environment in a way that can
be read back in by a POSIX compatible shell program. This program is almost
exactly the same as the ENV program.

Synopsis:
$ envget [-export] [-e <envfile>] [-tee <teefile>]

Notes:
Note that:
	envget -export

is equivalent to:
	env | while read LINE ; do
	  print $LINE
	  KEY=$( print $LINE | cut -d = -f 1 )
	  print "export ${KEY}"
	done

