/* memfile HEADER */
/* lang=C20 */

/* support low-overhead file bufferring requirements */
/* version %I% last-modified %G% */


/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

#ifndef	MEMFILE_INCLUDE
#define	MEMFILE_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<localmisc.h>


#define	MEMFILE_MAGIC	0x54728822
#define	MEMFILE		struct memfile_head


struct memfile_head {
	uint		magic ;
	int		pagesize ;
	char		*dbuf ;
	void		*bp ;
	size_t		off ;
	size_t		dlen ;
	size_t		fsize ;
	int		fd ;
} ;

typedef MEMFILE		memfile ;

EXTERNC_begin

extern int	memfile_open(memfile *,cchar *,int,mode_t) noex ;
extern int	memfile_write(memfile *,cvoid *,int) noex ;
extern int	memfile_len(memfile *) noex ;
extern int	memfile_allocation(memfile *) noex ;
extern int	memfile_tell(memfile *,off_t *) noex ;
extern int	memfile_buf(memfile *,void *) noex ;
extern int	memfile_close(memfile *) noex ;

EXTERNC_end


#endif /* MEMFILE_INCLUDE */


