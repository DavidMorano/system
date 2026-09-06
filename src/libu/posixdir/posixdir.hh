/* posixdir HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* help interface to POSIX |dirent(3c)| facility */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-23, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	posixdir

	Description:
	See the support source file for more information (and
	commentary) on this object.

*******************************************************************************/

#ifndef	POSIXDIR_INCLUDE
#define	POSIXDIR_INCLUDE
#ifdef	__cplusplus /* everything is C++ only */


#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>		/* POSIX® |off_t| */
#include	<dirent.h>		/* POSIX® |DIR(3c)| + |dirent_t(3c)| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


#define	POSIXDIR_MAGIC	0x97634525


struct posixdir ;

typedef int (posixdir::*posixdir_m)() noex ;

enum posixdirmems {
    	posixdirmem_tell,
    	posixdirmem_rewind,
	posixdirmem_close,
	posixdirmem_overlast
} ; /* end enum */

class posixdir ;
struct posixdir_te {
	posixdir	*op = nullptr ;
	int		w = -1 ;
	void operator () (posixdir *p,int m) noex {
	    op = p ;
	    w = m ;
	} ; /* end */
	int operator () (mut off_t * = nullptr) noex ;
	operator int () noex {
	    return operator () () ;
	} ; /* end */
} ; /* end struct (posixdir_te) */
struct posixdir_ma {
        posixdir	*op = nullptr ;
        void operator () (posixdir *p,int) noex {
            op = p ;
        } ; /* end */
        template<typename ... Args> int operator () (Args ... ) noex ;
        operator int () noex ;
} ; /* end struct (posixdir_ma) */
struct posixdir_co {
	posixdir	*op = nullptr ;
	int		w = -1 ;
	void operator () (posixdir *p,int m) noex {
	    op = p ;
	    w = m ;
	} ; /* end */
	operator int () noex ;
	int operator () () noex { 
	    return operator int () ;
	} ; /* end */
} ; /* end struct (posixdir_co) */
class posixdir {
    	friend		posixdir_te ;
    	friend		posixdir_ma ;
    	friend		posixdir_co ;
	DIR		*dirp = nullptr ;
	dirent		*debuf = nullptr ;
	dirent		*dep ;
	cchar		*fname ;
	char		*nbuf ;
	off_t		*sop ;
	off_t		so ;
	uint		magval ;
	int		nlen ;
	int		dfd = -1 ;
	int		delen = 0 ;
	int diropen	() noex ;
	int dirclose	() noex ;
	int dirread	() noex ;
	int dirtell	() noex ;
	int dirseek	() noex ;
	int dirrewind	() noex ;
	int bufbegin	(cchar *) noex ;
	int bufend	() noex ;
	int callout	(posixdir_m) noex ;
	int itell	(mut off_t *) noex ;
	int irewind	() noex ;
	int iclose	() noex ;
public:
	posixdir_te	tell ;
	posixdir_co	rewind ;
	posixdir_co	close ;
	posixdir_ma	magic ;
	posixdir() noex {
	    tell	(this,posixdirmem_tell) ;
	    rewind	(this,posixdirmem_rewind) ;
	    close	(this,posixdirmem_close) ;
	    magic	(this,0) ;
	    magval = 0 ;
	} ; /* end ctor */
	posixdir(const posixdir &) = delete ;
	posixdir &operator = (const posixdir &) = delete ;
	int open	(cchar *) noex ;
	int read	(dirent *,char *,int) noex ;
	int seek	(off_t) noex ;
	void dtor() noex ;
	destruct posixdir() {
	    if (magval) dtor() ;
	} ; /* end destruct */
} ; /* end struct (posixdir) */


#endif	/* __cplusplus */
#endif /* POSIXDIR_INCLUDE */


