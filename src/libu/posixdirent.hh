/* posixdirent HEADER */
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
	posixdirent

	Description:
	See the support source file for more information (and
	commentary) on this object.

*******************************************************************************/

#ifndef	POSIXDIRENT_INCLUDE
#define	POSIXDIRENT_INCLUDE
#ifdef	__cplusplus /* everything is C++ only */


#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>		/* |off_t| */
#include	<clanguage.h>
#include	<usysbase.h>
#include	<dirent.h>		/* |DIR(3c)| + |dirent_t(3c)| */


#define	POSIXDIRENT_MAGIC	0x97634525


struct posixdirent ;

constexpr uint		posixdirent_magicval = POSIXDIRENT_MAGIC ;

typedef int (posixdirent::*posixdirent_m)() noex ;

enum posixdirentmems {
    	posixdirentmem_tell,
    	posixdirentmem_rewind,
	posixdirentmem_close,
	posixdirentmem_overlast
} ; /* end enum */

class posixdirent ;
struct posixdirent_te {
	posixdirent	*op = nullptr ;
	int		w = -1 ;
	void operator () (posixdirent *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	int operator () (off_t * = nullptr) noex ;
	operator int () noex {
	    return operator () () ;
	} ;
} ; /* end struct (posixdirent_te) */
struct posixdirent_ma {
        posixdirent	*op = nullptr ;
        void operator () (posixdirent *p,int) noex {
            op = p ;
        } ;
        template<typename ... Args> int operator () (Args ... ) noex ;
        operator int () noex ;
} ; /* end struct (posixdirent_ma) */
struct posixdirent_co {
	posixdirent	*op = nullptr ;
	int		w = -1 ;
	void operator () (posixdirent *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	operator int () noex ;
	int operator () () noex { 
	    return operator int () ;
	} ;
} ; /* end struct (posixdirent_co) */
class posixdirent {
    	friend		posixdirent_te ;
    	friend		posixdirent_ma ;
    	friend		posixdirent_co ;
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
	int diropen() noex ;
	int dirclose() noex ;
	int dirread() noex ;
	int dirtell() noex ;
	int dirseek() noex ;
	int dirrewind() noex ;
	int bufbegin(cchar *) noex ;
	int bufend() noex ;
	int callout(posixdirent_m) noex ;
	int itell(off_t *) noex ;
	int irewind() noex ;
	int iclose() noex ;
public:
	posixdirent_te	tell ;
	posixdirent_co	rewind ;
	posixdirent_co	close ;
	posixdirent_ma	magic ;
	posixdirent() noex {
	    tell	(this,posixdirentmem_tell) ;
	    rewind	(this,posixdirentmem_rewind) ;
	    close	(this,posixdirentmem_close) ;
	    magic	(this,0) ;
	    magval = 0 ;
	} ; /* end ctor */
	posixdirent(const posixdirent &) = delete ;
	posixdirent &operator = (const posixdirent &) = delete ;
	int open	(cchar *) noex ;
	int read	(dirent *,char *,int) noex ;
	int seek	(off_t) noex ;
	void dtor() noex ;
	destruct posixdirent() {
	    if (magval) dtor() ;
	}
} ; /* end struct (posixdirent) */

template<typename ... Args> 
int posixdirent_ma::operator () (Args ... args) noex {
        int             rs = SR_FAULT ;
        if ((... && args)) {
            rs = (op->magval == posixdirent_magicval) ? SR_OK : SR_NOTOPEN ;
        }
        return rs ;
} /* end method (posixdirent_ma::operator) */

inline posixdirent_ma::operator int () noex {
        return (op->magval == posixdirent_magicval) ? SR_OK : SR_NOTOPEN ;
} /* end method (posixdirent_ma::operator) */


#endif	/* __cplusplus */
#endif /* POSIXDIRENT_INCLUDE */


