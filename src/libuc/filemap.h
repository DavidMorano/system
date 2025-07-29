/* filemap HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* read-file funtion through file-mapping */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	filemap

	Description:
        This little object supports some buffered file operations for
        low-overhead buffered I-O operations (read-only).

*******************************************************************************/

#ifndef	FILEMAP_INCLUDE
#define	FILEMAP_INCLUDE 


#include	<envstandards.h>	/* MUST be first to configure */
#include	<unistd.h>		/* |off_t| */
#include	<usystem.h>		/* for |ustat| */


#define	FILEMAP		struct filemap_head


struct filemap_head {
	cchar		*bp ;
	void		*mapdata ;
	ustat		*stbp ;		/* requires 'usystem.h' */
	size_t		mapsize ;
	size_t		maxsize ;
} ;

#ifdef	__cplusplus
enum filemapmems {
	filemapmem_rewind,
	filemapmem_close,
	filemapmem_overlast
} ;
struct filemap ;
struct filemap_co {
	filemap		*op = nullptr ;
	int		w = -1 ;
	void operator () (filemap *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	operator int () noex ;
	int operator () () noex { 
	    return operator int () ;
	} ;
} ; /* end struct (filemap_co) */
struct filemap_teller {
	filemap		*op = nullptr ;
	int		w = -1 ;
	void operator () (filemap *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	int operator () (off_t * = nullptr) noex ;
	operator int () noex {
	    return operator () (nullptr) ;
	} ;
} ; /* end struct (filemap_teller) */
struct filemap : filemap_head {
	filemap_co	rewind ;
	filemap_co	close ;
	filemap_teller	tell ;
	filemap() noex {
	    rewind(this,filemapmem_rewind) ;
	    close(this,filemapmem_close) ;
	    tell(this,0) ;
	    mapdata = nullptr ;
	} ;
	filemap(const filemap &) = delete ;
	filemap &operator = (const filemap &) = delete ;
	int open(cchar *,size_t = 0uz) noex ;
	int stat(ustat *) noex ;
	int read(int,void *) noex ;
	int getln(cchar **) noex ;
	int seek(off_t,int = 0) noex ;
	void dtor() noex ;
	destruct filemap() {
	    if (mapdata) dtor() ;
	} ;
} ; /* end struct (filemap) */
#else	/* __cplusplus */
typedef FILEMAP		filemap ;
#endif /* __cplusplus */

EXTERNC_begin

extern int	filemap_open(filemap *,cchar *,size_t) noex ;
extern int	filemap_stat(filemap *,ustat *) noex ;
extern int	filemap_read(filemap *,int,void *) noex ;
extern int	filemap_getln(filemap *,cchar **) noex ;
extern int	filemap_seek(filemap *,off_t,int) noex ;
extern int	filemap_tell(filemap *,off_t *) noex ;
extern int	filemap_rewind(filemap *) noex ;
extern int	filemap_close(filemap *) noex ;

EXTERNC_end


#endif /* FILEMAP_INCLUDE */


