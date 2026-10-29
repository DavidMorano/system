/* memo HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* memory allocation object */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	MEMO_INCLUDE
#define	MEMO_INCLUDE
#ifdef	__cplusplus /* C++ only! */


#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<stringview>		/* LIBU */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


class memo ;
struct memo_len {
    	memo		*op ;
	void operator () (memo *p) noex {
	    op = p ;
	} ; /* end method */
	operator int	() const noex ;
	int operator ()	() const noex {
	    return operator int () ;
	} ; /* end method */
} ; /* end struct (memo_len) */
class memo {
    	friend		memo_len ;
	voidp		rp ;
	int		rl ;		/* return-length */
	void dtor	() noex ;
	int allocstr	(charpp,int) noex ;
	int allocbuf	(voidp,	int) noex ;
	int getlen	() const noex ;
    public:
	memo		(const memo &) = delete ;
	void operator = (const memo &) = delete ;
	memo_len	len ;
	int free	(void *) noex ;
	int mall	(voidp,int) noex ;
	int vall	(voidp,int) noex ;
	int call	(voidp,int,int) noex ;
	int rall	(void *,int,void *) noex ;
	int sysbuf	(void *,int) noex ;
	int strw	(ccharpp,ccharp,int) noex ;
	int strw	(ccharpp,con stringview &) noex ;
	int ml		(char **) noex ;	/* max-line */
	int mn		(char **) noex ;	/* max-name */
	int mp		(char **) noex ;	/* max-path */
	int nn		(char **) noex ;	/* name system node */
	int un		(char **) noex ;	/* name system user */
	int gn		(char **) noex ;	/* name system group */
	int pn		(char **) noex ;	/* name system project */
	int protname	(char **) noex ;	/* name network prot-ocol */
	int netwname	(char **) noex ;	/* name network netw-ork */
	int hostname	(char **) noex ;	/* name network host */
	int servname	(char **) noex ;	/* name network serv-ice */
	int mailaddr	(char **) noex ;	/* buffer (mail-address) */
	int fs		(char **) noex ;	/* name system file-system */
	int sn		(char **) noex ;	/* name system symbol */
	int zn		(char **) noex ;	/* name zone (abbreviation) */
	int zi		(char **) noex ;	/* name zone-info */
	int ad		(char **) noex ;	/* buffer (mail-address) */
	int ps		(void *) noex ;
	int ma		(void *) noex ;
	int ut		(void *) noex ;		/* entry */
	int pw		(void *) noex ;		/* entry */
	int sp		(void *) noex ;		/* entry */
	int ua		(void *) noex ;		/* entry */
	int gr		(void *) noex ;		/* entry */
	int pj		(void *) noex ;		/* entry */
	int pr		(void *) noex ;		/* entry */
	int nw		(void *) noex ;		/* entry */
	int ho		(void *) noex ;		/* entry */
	int sv		(void *) noex ;		/* entry */
	memo() noex {
	    len		(this) ;
	    rp		= nullptr ;
	    rl		= -1 ;
	} ; /* end ctor */
	operator int	() const noex ;
	operator bool	() const noex {
	    return bool(rp) ;
	} ; /* end */
	destruct memo() {
	    if (rp) dtor() ;
	} ; /* end dtor */
} ; /* end struct (memo) */


#endif /* __cplusplus (C++ only) */
#endif /* MEMO_INCLUDE */


