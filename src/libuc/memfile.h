/* memfile INCLUDE */
/* lang=C20 */

/* support low-overhead file bufferring requirements */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	MEMFILE_INCLUDE
#define	MEMFILE_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<localmisc.h>		/* extra types */


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

#ifdef	__cplusplus
extern "C" {
#endif

extern int	memfile_open(MEMFILE *,cchar *,int,mode_t) noex ;
extern int	memfile_write(MEMFILE *,cvoid *,int) noex ;
extern int	memfile_len(MEMFILE *) noex ;
extern int	memfile_allocation(MEMFILE *) noex ;
extern int	memfile_tell(MEMFILE *,off_t *) noex ;
extern int	memfile_buf(MEMFILE *,void *) noex ;
extern int	memfile_close(MEMFILE *) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* MEMFILE_INCLUDE */


