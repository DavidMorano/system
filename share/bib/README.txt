TOOLS/SHARE/BIB


This is the Starbase project bibliographic database.  This database is
used by the 'referm' program found in 'prtfmt' (really 'referm'
underneath) as well as 'refer' for MS macro formatted AT&T DWB
documents.  

This sort of database is standard with most UNIX systems and many
of the programs used to access and maintenance the database are on all
UNIX systems.  Some of the programs are :

lookbib
roffbib
sortbib
addbib

The "real" program to use for maintenance of the database are :

lookbibl
indexbibl
addbibl

These program not only "work" (unlike the ones supplied with UNIX 
sometimes) but they are also customized to our local environment
somewhat making them easier to use (like when you forget where
the default databases are).

To recreate the index for the bibliographic references in this
directory, type :

	cd <to_this_directory>
	make

You can recreate an individual refernce by typing :

	cd <to_this_directory>
	indexbibl <reference>




Dave Morano, 96/04/17



The REFER database keyletters are as follows:

keyletter	description
-----------------------------------------
A		author
T		title
J		journal
L		label
C		address
M		month
Y		year
V		volume
N		number
P		pages
K		keywords
X		comments
Q		citation query key
B		ISBM
I		publisher
R		Library of Congress catalogue number



