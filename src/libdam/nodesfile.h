/* nodesfile HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* UNIX "nodes" file support */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	nodesfile

	Description:
	This little object supports "nodes" file management.

*******************************************************************************/

#ifndef	NODESFILE_INCLUDE
#define	NODESFILE_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<time.h>		/* |time_t| */
#include	<usystem.h>
#include	<vechand.h>
#include	<hdb.h>


#define	NODESFILE		struct nodesfile_head
#define	NODESFILE_CUR		struct nodesfile_cursor
#define	NODESFILE_MAGIC 	0x17464148
#define	NODESFILE_MAXSZ		(500 * 1024)


struct nodesfile_cursor {
	hdb_cur		*hcp ;
} ;

struct nodesfile_head {
    	vechand		*flp ;			/* file-list-pointer */
	hdb		*elp ;			/* entry-list-pointer */
	time_t		ti_check ;
	time_t		ti_load ;
	uint		magic ;
	int		maxsz ;
} ;

typedef NODESFILE	nodesfile ;
typedef NODESFILE_CUR	nodesfile_cur ;

EXTERNC_begin

extern int	nodesfile_open(nodesfile *,cchar *,int) noex ;
extern int	nodesfile_add(nodesfile *,cchar *) noex ;
extern int	nodesfile_have(nodesfile *,cchar *,int) noex ;
extern int	nodesfile_curbegin(nodesfile *,nodesfile_cur *) noex ;
extern int	nodesfile_curend(nodesfile *,nodesfile_cur *) noex ;
extern int	nodesfile_curenum(nodesfile *,nodesfile_cur *,char *,int) noex ;
extern int	nodesfile_check(nodesfile *,time_t) noex ;
extern int	nodesfile_close(nodesfile *) noex ;

EXTERNC_end


#endif /* NODESFILE_INCLUDE */


