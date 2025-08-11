/* hdb_obj SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* general-purpose in-core hashing */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-05-01, David A­D­ Morano
	I wrote this from stratch due to frustration with other
	types of these things.  Why I have to wrote something like
	this just goes to show how little strctured containers are
	used now-a-days.

	= 2003-04-19, David A­D­ Morano
	I grabbed the previous version and modified it to use a
	look-aside buffer for the entry-node structures.  This is
	used instead of |malloc(3c)| to get and release node
	structures.  The look-aside manager uses |malloc(3c)| to
	extend the look-aside entries.  Entries released back to the
	look-aside manager are not given back to |malloc(3c)| (via
	|free(3c)| at the present time -- not so easy to do).  Anyway,
	we get about an average of 27% to 29% speedup on a combination
	of allocating and freeing entries.

*/

/* Copyright © 1998,2003 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	hdb

	Description:
	This package provides a reasonably general-purpose hashing
	object for use in cases calling for in-core hashing.

	One of the features sorely missed in other packages (code
	objects) like this is that this package can store multiple
	key-data pairs with the same keys.

	The "walk" type method, which I never liked in other packages,
	is deprecated in favor of the "enumerate" (|hdb_curenum|) type
	method instead.  There are two types of "delete" methods.
	One deletes all entries with the same key (possibly useful
	for old timers who never knew what to do with muliple
	identical keys in the first place), and the other function
	deletes the entry under the current cursor.

	Oh, the "enumerate" and "fetch" functions require the concept
	of a CURSOR, which is, roughly, a pointer type of a thing
	to the current entry.

	In terms of implementation, these routines are just as
	inelegant, or worse, than the DBM, HDBM, or other type of
	implementations; namely, I implemented hashing with hash
	buckets (using a single table) with chained entries dangling.
	An additional dimension of chains may dangle from each of
	the previous described entries to facilitate muliple entries
	with the same key.

	Important user note:

	This package does not store the user supplied data into its
	own storage space.  Only pointers from within the user
	supplied data are stored.  For this reason, it is very
	important for the user to not free up any storage locations
	that are still linked inside of this database.  Havoc will
	result! Also, when freeing entries in this database, the
	user's data is NOT freed! Further, the DB storage location(s)
	that were used to store the user's data IS freed! This means
	that the user should make pointers to or copies of any data
	that they have (if they ever want to access it again) BEFORE
	they delete the corresponding entry from this database!  Got
	it?  Do it!

	Implementation note:

	Whew! There may be more hairy code that what is in this
	object, but I do not know where it is! What am I trying to
	say? There may be bugs in this mess! It has held up solidly
	against testing and use already but there could still be
	some corners that are not quite right.  Both the core of
	'enumerate' and 'fetch' are very hairy.  Fetching is especially
	hairy.  Some of the hairyness is due to some efforts (minor)
	to not repeat executing some code unnecessarily.  This appoach
	may have been a mistake given the complexity of this whole
	mess already.  Maybe there is something to be said for simple
	data strctures after all! Enjoy!

	Synopsis:
	int hdb_start(hdb *op,int n,int at,hsbhas_f h,hsbcmp_f c) noex

	Arguments:
	op		object pointer
	n		starting number of entries (estimated)
	at		allocation-type:
				0=regular
				1=lookaside
	h		the hash function
	c		the key-comparison function

	Returns:
	>=0		OK
	<0		error (system-return)

	Note - for storing an item into the DB:

	Note that addresses in the key and the value DATUMs should
	point at memory that is not changed for the life of the
	item in the DB -- if the user wants it to be there when
	they access it!

	A "value" structure MUST always be passed to this subroutine
	from the caller. But the buffer address of a "value" can
	be nullptr.

	Also, zero length values can be stored by specifying a value
	length of zero (buffer address of anything).

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* <- |strlen(3c)| + |memcmp(3c)| */
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>
#include	<lookaside.h>
#include	<strn.h>
#include	<hash.h>
#include	<localmisc.h>

#include	"hdb.h"


/* local defines */

#define	HDB_PRBUFLEN	20
#define	HDB_D		DAT

#define	DAT		hdb_datum
#define	ENT		hdb_ent
#define	CUR		hdb_cur


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int hdb::start(int n,int alt,hdbhash_f h,hdbcmp_f c) noex {
    	return hdb_start(this,n,alt,h,c) ;
}

int hdb::store(HDB_D key,DAT val) noex {
	return hdb_store(this,key,val) ;
}

int hdb::have(HDB_D key) noex {
	return hdb_have(this,key) ;
}

int hdb::curbegin(CUR *curp) noex {
	return hdb_curbegin(this,curp) ;
}

int hdb::curend(CUR *curp) noex {
	return hdb_curend(this,curp) ;
}

int hdb::curenum(CUR *curp,HDB_D *keyp,DAT *valp) noex {
	return hdb_curenum(this,curp,keyp,valp) ;
}

int hdb::curget(CUR *curp,DAT *keyp,DAT *valp) noex {
	return hdb_curget(this,curp,keyp,valp) ;
}

int hdb::curnext(CUR *curp) noex {
	return hdb_curnext(this,curp) ;
}

int hdb::curdel(CUR *curp,int fadv) noex {
	return hdb_curdel(this,curp,fadv) ;
}

int hdb::curdone(CUR *curp) noex {
	return hdb_curdone(this,curp) ;
}

int hdb::fetch(DAT key,CUR *curp,DAT *valp) noex {
	return hdb_fetch(this,key,curp,valp) ;
}

int hdb::fetchrec(DAT key,CUR *curp,DAT *keyp,DAT *valp) noex {
	return hdb_fetchrec(this,key,curp,keyp,valp) ;
}

int hdb::getkeyrec(DAT key,CUR *curp,DAT *keyp,DAT *valp) noex {
	return hdb_getkeyrec(this,key,curp,keyp,valp) ;
}

int hdb::nextrec(hdb_dat key,hdb_cur *curp) noex {
    	return hdb_nextrec(this,key,curp) ;
}

int hdb::delkey(hdb_dat key) noex {
    	return hdb_delkey(this,key) ;
}

void hdb::dtor() noex {
	if (cint rs = finish ; rs < 0) {
	    ulogerror("hdb",rs,"fini-finish") ;
	}
}

hdb_co::operator int () noex {
	int		rs = SR_BUGCHECK ;
	if (op) {
	    switch (w) {
	    case hdbmem_delall:
	        rs = hdb_delall(op) ;
	        break ;
	    case hdbmem_audit:
	        rs = hdb_audit(op) ;
	        break ;
	    case hdbmem_finish:
	        rs = hdb_finish(op) ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
}
/* end method (hdb_co::operator) */


