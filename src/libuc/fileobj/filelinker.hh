/* filelinker HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* short-word queue */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This obejct module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	FILELINKER_INCLUDE
#define	FILELINKER_INCLUDE
#ifdef	__cplusplus /* C++ only */


#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<clanguage.h>
#include	<usysbase.h>


#define	FILELINKER		struct filelinker_head
#define	FILELINKER_MAGIC	0x87625374


enum filelinkermems {
	filelinkermem_count,
	filelinkermem_finish,
	filelinkermem_overlast
} ; /* end enum (filelinkermem) */
struct filelinker ;
struct filelinker_co {
	filelinker	*op = nullptr ;
	int		w = -1 ;
	void operator () (filelinker *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	int operator () (int = 0) noex ;
	operator int () noex {
	    return operator () (0) ;
	} ;
} ; /* end struct (filelinker_co) */
struct filelinker {
	filelinker_co	count ;
	filelinker_co	finish ;
    	void		*tdp ;		/* target-directory-pointer */
    	void		*frp ;		/* file-record-pointer */
	uint		magic ;
	int		tll ;
	filelinker() noex {
	    count	(this,filelinkermem_count) ;
	    finish	(this,filelinkermem_finish) ;
	    tlp = nullptr ;
	    frp = nullptr ;
	    magic = 0 ;
	    tlp = 0 ;
	} ; /* end ctor */
	filelinker(const filelinker &) = delete ;
	filelinker &operator = (const filelinker &) = delete ;
	int start	(mainv) noex ;
	int link	(ustat *,cchar *,int) noex ;
	void dtor() noex ;
	int operator [] (int) noex ;
	operator int () noex ;
	destruct filelinker() {
	    if (magic) dtor() ;
	} ;
} ; /* end struct (filelinker) */


#endif /* __cplusplus */
#endif /* FILELINKER_INCLUDE */


