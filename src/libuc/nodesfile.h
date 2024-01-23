/* nodesfile SUPPORT */
/* lang=C20 */

/* UNIX "nodes" file support */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This little object supports "nodes" file management.


*******************************************************************************/

#ifndef	NODESFILE_INCLUDE
#define	NODESFILE_INCLUDE


#include	<envstandards.h>	/* first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/mman.h>
#include	<climits>
#include	<usystem.h>
#include	<hdb.h>
#include	<localmisc.h>


#define	NODESFILE	struct nodesfile_head
#define	nodesfile_cur	struct nodesfile_cursor
#define	NODESFILE_FINFO	struct nodesfile_finfo


struct nodesfile_cursor {
	hdb_cur		cur ;
} ;

struct nodesfile_finfo {
	cchar		*fname ;
	time_t		mtime ;
	uino_t		ino ;
	dev_t		dev ;
	int		oflags ;
} ;

struct nodesfile_head {
	char		*mapbuf ;
	hdb		nodes ;
	time_t		ti_check ;
	time_t		ti_load ;
	NODESFILE_FINFO	fi ;
	uint		pagesize ;
	uint		mapsize ;
	uint		filesize ;
	uint		maxsize ;
	uint		len ;
} ;

EXTERNC_begin

extern int	nodesfile_open(nodesfile *,cchar *,int,int) noex ;
extern int	nodesfile_search(nodesfile *,cchar *,int) noex ;
extern int	nodesfile_curbegin(nodesfile *,nodesfile_cur *) noex ;
extern int	nodesfile_curend(nodesfile *,nodesfile_cur *) noex ;
extern int	nodesfile_enum(nodesfile *,nodesfile_cur *,char *,int) noex ;
extern int	nodesfile_check(nodesfile *,time_t) noex ;
extern int	nodesfile_close(nodesfile *) noex ;

EXTERNC_end


#endif /* NODESFILE_INCLUDE */


