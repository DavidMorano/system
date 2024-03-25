/* filebuf HEADER */
/* lang=C20 */

/* support some buffered file operations */
/* version %I% last-modified %G% */


/* revision history:

	= 2002-04-01, David A­D­ Morano
	This subroutine was originally written for Rightcore Network
	Services (RNS).

*/

/* Copyright © 2002 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

        This little object supports some buffered file operations for
        low-overhead buffered I-O requirements.

*******************************************************************************/

#ifndef	FILEBUF_INCLUDE
#define	FILEBUF_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<stdarg.h>		/* |va_list(3c)| */
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<clanguage.h>
#include	<localmisc.h>


#define	FILEBUF		struct filebuf_head
#define	FILEBUF_FL	struct filebuf_flags
#define	FILEBUF_ONET	O_NETWORK	/* specify network FD */
#define	FILEBUF_RCNET	3		/* read-count for network */


struct filebuf_flags {
	uint		net:1 ;		/* network FD */
	uint		write:1 ;	/* we are writing (otherwise reading) */
} ;

struct filebuf_head {
	char		*buf ;		/* constant */
	char		*bp ;		/* goes up with use */
	off_t		off ;		/* virtual file pointer */
	FILEBUF_FL	f ;
	int		fd ;		/* file-descriptor */
	int		of ;		/* open-flags */
	int		dt ;		/* file-type ("DT_XXX" values)*/
	int		bufsize ;	/* buffer size (constant) */
	int		len ;		/* length of valid-data in buffer */
} ;

typedef FILEBUF		filebuf ;

EXTERNC_begin

extern int	filebuf_start(filebuf *,int,off_t,int,int) noex ;
extern int	filebuf_read(filebuf *,void *,int,int) noex ;
extern int	filebuf_readp(filebuf *,void *,int,off_t,int) noex ;
extern int	filebuf_readln(filebuf *,char *,int,int) noex ;
extern int	filebuf_readlns(filebuf *,char *,int,int,int *) noex ;
extern int	filebuf_write(filebuf *,cvoid *,int) noex ;
extern int	filebuf_writeto(filebuf *,cvoid *,int,int) noex ;
extern int	filebuf_println(filebuf *,cchar *,int) noex ;
extern int	filebuf_vprintf(filebuf *,cchar *,va_list) noex ;
extern int	filebuf_printf(filebuf *,cchar *,...) noex ;
extern int	filebuf_reserve(filebuf *,int) noex ;
extern int	filebuf_update(filebuf *,off_t,cchar *,int) noex ;
extern int	filebuf_invalidate(filebuf *) noex ;
extern int	filebuf_flush(filebuf *) noex ;
extern int	filebuf_adv(filebuf *,int) noex ;
extern int	filebuf_seek(filebuf *,off_t,int) noex ;
extern int	filebuf_tell(filebuf *,off_t *) noex ;
extern int	filebuf_poll(filebuf *,int) noex ;
extern int	filebuf_finish(filebuf *) noex ;

extern int	filebuf_writeblanks(filebuf *,int) noex ;
extern int	filebuf_writefill(filebuf *,cchar *,int) noex ;
extern int	filebuf_writealign(filebuf *,int) noex ;
extern int	filebuf_writezero(filebuf *,int) noex ;
extern int	filebuf_writefd(filebuf *,char *,int,int,int) noex ;
extern int	filebuf_writehdr(filebuf *,cchar *,int) noex ;
extern int	filebuf_writehdrkey(filebuf *,cchar *) noex ;
extern int	filebuf_printlncont(filebuf *,int,cchar *,int) noex ;

EXTERNC_end


#endif /* FILEBUF_INCLUDE */


