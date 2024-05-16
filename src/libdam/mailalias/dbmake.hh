/* dbmake HEADER */
/* lang=C++20 */

/* database-make */
/* version %I% last-modified %G% */


/* revision history:

	= 2003-06-11, David A­D­ Morano
	I snarfed this file from some hardware research I was doing
	since it seemed be a pretty good match for the present
	algorithm needs.  We will see how it works out!

*/

/* Copyright © 2003 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This module is used to manage a MAILALIAS object.

	We always check if your alias key is the string "Postmater"
	(in a case-insensitive way) and if it is, we convert it to
	LOWER case before doing the |mailalias_fetch()|.  Also,
	unlike other fetching (with other DBs), there is no concept
	of "prefix" match fetching!

	We use TWO string tables in this DB (unlike some other
	similar DBs). We use one string table for keys and another
	for values. I sort of did this for fun (so far). This is
	actually potentially less space-efficient than using just
	one string table since strings that are both a key and a
	value are not combined in one table but rather appear
	separately in each of the two tables. However, the up side
	is that:

	a. making a record table of only keys is done by just taking
	advantage of the strtab method for that

	b. we easily can provide an interface to transverse or
	otherwise just query the keys if we want to later

	c. we can do more space-efficient building of the DB file
	since we only need to keep the smaller key string table
	around for the main indexing!

*******************************************************************************/

#ifndef	DBMAKE_INCLUDE
#define	DBMAKE_INCLUDE

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<vecobj.h>
#include	<strtab.h>
#include	<localmisc.h>		/* |MAILADDRLEN| */


#define	DBMAKE_IDLEN		64
#define	DBMAKE_ALIASNAMELEN	64
#define	DBMAKE_FILEMAGIC	"mailalias"
#define	DBMAKE_FILEMAGICSIZE	strlen(DBMAKE_FILEMAGIC)
#define	DBMAKE_FILEVERSION	0
#define	DBMAKE_OSEC		(1<<0)
#define	DBMAKE_ORANDLC		(1<<1)
#define	DBMAKE_NSHIFT		8


namespace mailutils {
    typedef int (*rt_t)[2] ;
    class dbmake {
    public:
	vecobj		*rlp ;
	strtab		*klp ;
	strtab		*vlp ;
	int		(*rectab)[2] ;
	uint		nrecs ;
	uint		nkeys ;
	int		fd ;
	int		ikey{} ;
	int		count{} ;
	int		ktlen ;
	int		ktsize ;
	int		reclen ;
	int		recsize ;
	int		ropts ;
	int		rilen ;
	int		risize ;
	int		sksize ;
	int		svsize ;
	int		f_havekey ;
	dbmake(vecobj *arlp,strtab *aklp,strtab *avlp,int afd) noex {
	    rlp = arlp ;
	    klp = aklp ;
	    vlp = avlp ;
	    fd = afd ;
	    f_havekey = false ;
	} ;
	int wrfileproc(cchar *) noex ;
	int wrfileline(cchar *,int) noex ;
	int wrfile(time_t) noex ;
	int wrfiler(time_t) noex ;
	int wrfilekeytab() noex ;
	int wrfilerec() noex ;
	int wrfilekeys() noex ;
	int wrfilevals() noex ;
	int mkind(vecobj *,cc *,rt_t,int) noex ;
    } ; /* end class (dbmake) */
    class record {
    public:
	uint		key ;
	uint		val ;
	record() noex : key(0), val(0) { } ;
	record(uint k,uint v) noex : key(k), val(v) { } ;
    } ; /* end class (record) */
    typedef record *	recordp ;
}


#endif /* DBMAKE_INCLUDE */


