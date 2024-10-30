/* nodesearch HEADER */
/* encoding=ISO8859-1 */
/* lang=C20 */

/* search for a node name */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	nodesearch

	Description:
	This little object supports searching for a node name in
	the cluster node list file.

*******************************************************************************/

#ifndef	NODESEARCH_INCLUDE
#define	NODESEARCH_INCLUDE


#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<hdbstr.h>
#include	<nodesfile.h>


#define	NODESEARCH		struct nodesearch_head
#define	NODESEARCH_CUR		struct nodesearch_cursor
#define	NODESEARCH_FL		struct nodesearch_flags
#define	NODESEARCH_FI		struct nodesearch_file


struct nodesearch_flags {
	uint		sw:1 ;
	uint		loaded:1 ;
} ;

struct nodesearch_cursor {
	nodesfile_cur	c1 ;
	hdbstr_cur	c2 ;
} ;

struct nodesearch_file {
	cchar		*fname ;
	time_t		mtime ;
	ino_t		ino ;
	dev_t		dev ;
} ;

struct nodesearch_head {
	nodesfile	a ;		/* first choice */
	hddstr		b ;		/* second choice */
	NODESEARCH_FL	fl ;
	NODESEARCH_FI	fi ;
	time_t		ti_check ;	/* last check time */
	time_t		ti_load ;	/* last load time */
} ;

typedef NODESEARCH	nodesearch ;
typedef	NODESEARCH_FL	nodesearch_fl ;
typedef	NODESEARCH_FI	nodesearch_fi ;
typedef NODESEARCH_CUR	nodesearch_cur ;

EXTERNC_begin

extern int	nodesearch_open(nodesearch *,cchar *,int,int) noex ;
extern int	nodesearch_search(nodesearch *,cchar *,int) noex ;
extern int	nodesearch_curbegin(nodesearch *,nodesearch_cur *) noex ;
extern int	nodesearch_curend(nodesearch *,nodesearch_cur *) noex ;
extern int	nodesearch_enum(nodesearch *,nodesearch_cur *,char *,int) noex ;
extern int	nodesearch_check(nodesearch *,time_t) noex ;
extern int	nodesearch_close(nodesearch *) noex ;

EXTERNC_end


#endif /* NODESEARCH_INCLUDE */


