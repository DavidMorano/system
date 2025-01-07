/* mailaliashdr HEADER */
/* encoding=ISO8859-1 */
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

  	Object:
	mailaliashdr

	Description:
	This module is used to manage a MAILALIAS object.  We always
	check if your alias key is the string "Postmater" (in a
	case-insensitive way) and if it is, we convert it to LOWER
	case before doing the |mailalias_fetch()|.  Also, unlike
	other fetching (with other DBs), there is no concept of
	"prefix" match fetching!  We use TWO string tables in this
	DB (unlike some other similar DBs). We use one string table
	for keys and another for values. I sort of did this for fun
	(so far). This is actually potentially less space-efficient
	than using just one string table since strings that are
	both a key and a value are not combined in one table but
	rather appear separately in each of the two tables. However,
	the up side is that:

	a. making a record table of only keys is done by just taking
	advantage of the strtab method for that

	b. we easily can provide an interface to transverse or
	otherwise just query the keys if we want to later

	c. we can do more space-efficient building of the DB file
	since we only need to keep the smaller key string table
	around for the main indexing!

*******************************************************************************/

#ifndef	MAILALIASHDR_INCLUDE
#define	MAILALIASHDR_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>


enum mailaliashdrs {
	mailaliashdr_wtime,
	mailaliashdr_wcount,
	mailaliashdr_key,
	mailaliashdr_keylen,
	mailaliashdr_rec,
	mailaliashdr_reclen,
	mailaliashdr_ind,
	mailaliashdr_indlen,
	mailaliashdr_skey,
	mailaliashdr_skeysize,
	mailaliashdr_sval,
	mailaliashdr_svalsize,
	mailaliashdr_overlast
} ;


#endif /* MAILALIASHDR_INCLUDE */


