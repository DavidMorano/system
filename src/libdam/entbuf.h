/* entbuf HEADER (Entry-Buffer) */
/* encoding=ISO8859-1 */
/* lang=C20 */

/* manage entry buffering of a file */
/* version %I% last-modified %G% */


/* revision history:

	= 2003-10-22, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2003 David A­D­ Morano.  All rights reserved. */

#ifndef	ENTBUF_INCLUDE
#define	ENTBUF_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* system types */
#include	<unistd.h>		/* |off_t| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


#define	ENTBUF_MAGIC	1092847456
#define	ENTBUF_NENTS	4
#define	ENTBUF		struct entbuf_head
#define	ENTBUF_FL	struct entbuf_flags
#define	ENTBUF_WAY	struct entbuf_wayer


struct entbuf_wayer {
	char		*wbuf ;		/* buffer */
	off_t		woff ;		/* "way" offset to file entries */
	uint		utime ;		/* usage time */
	int		wlen ;
	int		nvalid ;	/* number of valid entries */
} ;

struct entbuf_flags {
	uint		init:1 ;	/* init'ed */
} ;

struct entbuf_head {
	ENTBUF_WAY	*ways ;
	ENTBUF_FL	f ;
	uint		magic ;
	uint		utimer ;	/* usage timer (fake time) */
	uint		soff ;		/* starting offset */
	int		esize ;		/* entry size */
	int		nways ;		/* maximum number of ways */
	int		iways ;		/* active number of ways */
	int		npw ;		/* number entries per way */
	int		nentries ;	/* number of total entries */
	int		fd ;
} ;

typedef ENTBUF		entbuf ;
typedef	ENTBUF_FL	entbuf_fl ;
typedef	ENTBUF_WAY	entbuf_way ;

EXTERNC_begin

extern int entbuf_start(entbuf *,int,uint,int,int,int) noex ;
extern int entbuf_finish(entbuf *) noex ;
extern int entbuf_read(entbuf *,int,char **) noex ;
extern int entbuf_write(entbuf *,int,cvoid *) noex ;
extern int entbuf_count(entbuf *) noex ;
extern int entbuf_sync(entbuf *) noex ;
extern int entbuf_invalidate(entbuf *,int) noex ;

EXTERNC_end


#endif /* ENTBUF_INCLUDE */


