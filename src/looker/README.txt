LOOKER

This little program is used to search for words (much in the way
the LOOK program does) but allows for the specification of successive
parts of a word rather than just the prefix of a word.

Example:
$ # search for the word "apple"
$ looker ap ple

Synopsis:
$ loooker -d -f -t - <arg(s)>
Arguments:
-d		use dictionary order
-f		fold mode
-t <termchar>	terminal character
-db <database>	word database
<arg(s)>	fragments of words to try to match (only up to 5 allowed)

