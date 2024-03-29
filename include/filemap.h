/* filemap HEADER */
/* lang=C20 */

/* support some buffered file operations */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-01, David A�D� Morano
	This subroutine was originally written.

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

        This little object supports some buffered file operations for
        low-overhead buffered I/O requirements.

*******************************************************************************/

#ifndef	FILEMAP_INCLUDE
#define	FILEMAP_INCLUDE 


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<limits.h>
#include	<usystem.h>		/* for |USTAT| */
#include	<localmisc.h>


#define	FILEMAP		struct filemap_head


struct filemap_head {
	cchar		*bp ;
	void		*mapdata ;
	USTAT		sb ;		/* requires 'usystem.h' */
	size_t		mapsize ;
	size_t		maxsize ;
} ;

typedef	FILEMAP		filemap ;

EXTERNC_begin

extern int	filemap_open(filemap *,cchar *,int,size_t) noex ;
extern int	filemap_stat(filemap *,USTAT *) noex ;
extern int	filemap_read(filemap *,int,void *) noex ;
extern int	filemap_getline(filemap *,cchar **) noex ;
extern int	filemap_seek(filemap *,off_t,int) noex ;
extern int	filemap_tell(filemap *,off_t *) noex ;
extern int	filemap_rewind(filemap *) noex ;
extern int	filemap_close(filemap *) noex ;

EXTERNC_end


#endif /* FILEMAP_INCLUDE */


