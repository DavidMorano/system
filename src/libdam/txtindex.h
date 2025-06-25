/* txtindex HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* interface to TXTINDEX database management */
/* version %I% last-modified %G% */


/* revision history:

	= 2008-10-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

#ifndef	TXTINDEX_INCLUDE
#define	TXTINDEX_INCLUDE


#include	<envstandards.h>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<modload.h>

#include	<txtindexes.h>


#define	TXTINDEX_MAGIC		0x99447246
#define	TXTINDEX		struct txtindex_head
#define	TXTINDEX_FL		struct txtindex_flags
#define	TXTINDEX_CUR		struct txtindex_cursor
#define	TXTINDEX_TAG		TXTINDEXES_TAG
#define	TXTINDEX_INFO		TXTINDEXES_INFO


struct txtindex_flags {
        uint		modload:1 ;
} ;

struct txtindex_cursor {
	void		*scp ;		/* SO-cursor pointer */
	uint		magic ;
} ;

struct txtindex_head {
	modload		*mlp ;
	void		*callp ;
	void		*obj ;		/* object pointer */
	TXTINDEX_FL	fl ;
	uint		magic ;
	int		objsize ;	/* object size */
	int		cursize ;	/* cursor size */
} ;

typedef	TXTINDEX	txtindex ;
typedef	TXTINDEX_FL	txtindex_fl ;
typedef	TXTINDEX_CUR	txtindex_cur ;
typedef	TXTINDEX_TAG	txtindex_tag ;
typedef	TXTINDEX_INFO	txtindex_info ;

EXTERNC_begin

extern int txtindex_open(txtindex *,cchar *,cchar *) noex ;
extern int txtindex_count(txtindex *) noex ;
extern int txtindex_neigen(txtindex *) noex ;
extern int txtindex_getinfo(txtindex *,txtindex_info *) noex ;
extern int txtindex_getsdn(txtindex *,char *,int) noex ;
extern int txtindex_getsfn(txtindex *,char *,int) noex ;
extern int txtindex_iseigen(txtindex *,cchar *,int) noex ;
extern int txtindex_curbegin(txtindex *,txtindex_cur *) noex ;
extern int txtindex_curlook(txtindex *,txtindex_cur *,mainv) noex ;
extern int txtindex_curenum(txtindex *,txtindex_cur *,txtindex_tag *,
		char *,int) noex ;
extern int txtindex_curend(txtindex *,txtindex_cur *) noex ;
extern int txtindex_audit(txtindex *) noex ;
extern int txtindex_close(txtindex *) noex ;

EXTERNC_end


#endif /* TXTINDEX_INCLUDE */


