QUEUE

This directory contains queue-type objects.


fifostr		FIFO object for strings, stores data
fsi		FIFO object for strings, stores data, thread-safe
fifoitem	FIFO object for variable sized items
slq		Q: pointer, relo, single
piq		Q: pointer, relo, double, count, magic, thread
plainq		Q: self-rel, relo, double, count, magic
q		Q: self-rel, relo, double, count, thread
aiq		Q: self-rel, relo, double, count, magic, thread, async
pq		Q: pointer, relo, double, count, not-circular
cq		container Q, relo, count, magic
ciq		container Q, relo, count, magic, thread
charq		character Q, relo, count
chariq		character Q, relo, count, thread
intiq		integer Q, relocatable, count, thread-safe

= obsoleted
cpq		Q: circular pointer Q (huge in the old days w/ OS stuff!)


