QUEUE

This directory contains queue-type objects.


fifostr		FIFO for strings, stores data
fifoelem	FIFO for fixe size elements, relo, count, magic
fifoitem	FIFO for variable sized items, relo, count, magic
fsi		FIFO for strings, stores-data, thread
plainq		Q: self-rel, relo, double, count, magic
slq		Q: pointer, relo, single
slist		Q: pointer, relo, single, count, enumeration
piq		Q: pointer, relo, double, count, magic, thread
q		Q: self-rel, relo, double, count, thread
aiq		Q: self-rel, relo, double, count, magic, thread, async
pq		Q: pointer, relo, double, count, not-circular
ciq		container Q: circular, relo, double, count, magic, thread
cq		container Q: relo, count, magic
charq		character Q: relo, count
chariq		character Q: relo, count, thread
intiq		integer   Q: relo, count, magic, thread

= obsoleted
cpq		Q: circular pointer Q (huge in the old days w/ OS stuff!)


