/* ebuf HEADER */
/* charset=ISO8859-1 */
/* lang=C20 (conformance reviewed) */

/* manage entry buffering of a file */
/* version %I% last-modified %G% */


/* revision history:

	= 2003-10-22, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2003 David A­D­ Morano.  All rights reserved. */

#ifndef	EBUF_INCLUDE
#define	EBUF_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


/* object defines */
#define	EBUF_MAGIC	1092847456
#define	EBUF_NENTS	4
#define	EBUF		struct ebuf_head
#define	EBUF_FL		struct ebuf_flags
#define	EBUF_WAY	struct ebuf_way


/* file buffer state */
struct ebuf_way {
	offset_t	woff ;		/* "way" offset to file entries */
	char		*wbuf ;		/* buffer */
	uint		utime ;		/* usage time */
	int		wlen ;
	int		nvalid ;	/* number of valid entries */
} ;

struct ebuf_flags {
	uint		init:1 ;	/* init'ed */
} ;

struct ebuf_head {
	uint		magic ;
	EBUF_FL		f ;
	EBUF_WAY	*ways ;
	uint		utimer ;	/* usage timer (fake time) */
	uint		soff ;		/* starting offset */
	int		esize ;		/* entry size */
	int		nways ;		/* maximum number of ways */
	int		iways ;		/* active number of ways */
	int		npw ;		/* number entries per way */
	int		nentries ;	/* number of total entries */
	int		fd ;
} ;

EXTERNC_begin

extern int ebuf_start(EBUF *,int,uint,int,int,int) noex ;
extern int ebuf_finish(EBUF *) noex ;
extern int ebuf_read(EBUF *,int,char **) noex ;
extern int ebuf_write(EBUF *,int,cvoid *) noex ;
extern int ebuf_count(EBUF *) noex ;
extern int ebuf_sync(EBUF *) noex ;
extern int ebuf_invalidate(EBUF *,int) noex ;

EXTERNC_end


#endif /* EBUF_INCLUDE */


