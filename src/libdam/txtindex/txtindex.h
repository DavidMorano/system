/* txtindex HEADER */
/* lang=C20 */

/* interface to TXTINDEX database management */
/* version %I% last-modified %G% */


/* revision history:

	= 2008-10-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

#ifndef	TXTINDEX_INCLUDE
#define	TXTINDEX_INCLUDE


#include	<envstandards.h>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysrets.h>
#include	<modload.h>

#include	"txtindexes.h"


#define	TXTINDEX_MAGIC		0x99447246
#define	TXTINDEX		struct txtindex_head
#define	TXTINDEX_CUR		struct txtindex_cursor
#define	TXTINDEX_CALLS		struct txtindex_callsubs
#define	TXTINDEX_TAG		TXTINDEXES_TAG
#define	TXTINDEX_INFO		TXTINDEXES_INFO


struct txtindex_cursor {
	void		*scp ;		/* SO-cursor pointer */
	uint		magic ;
} ;

EXTERNC_begin
    struct txtindex_callsubs {
	int	(*open)(void *,cchar *) ;
	int	(*count)(void *) ;
	int	(*neigen)(void *) ;
	int	(*info)(void *,TXTINDEXES_INFO *) ;
	int	(*iseigen)(void *,cchar *,int) ;
	int	(*curbegin)(void *,void *) ;
	int	(*lookup)(void *,void *,cchar **) ;
	int	(*read)(void *,void *,TXTINDEXES_TAG *) ;
	int	(*curend)(void *,void *) ;
	int	(*audit)(void *) ;
	int	(*close)(void *) ;
    } ; /* end struct (txtindex_callsubs) */
EXTERNC_end

struct txtindex_head {
	modload		loader ;
	TXTINDEX_CALLS	call ;
	void		*obj ;		/* object pointer */
	uint		magic ;
	int		objsize ;	/* object size */
	int		cursize ;	/* cursor size */
} ;

typedef	TXTINDEX	txtindex ;
typedef	TXTINDEX_CUR	txtindex_cur ;
typedef	TXTINDEX_CALLS	txtindex_calls ;

EXTERNC_begin

extern int	txtindex_open(txtindex *,cchar *,cchar *) noex ;
extern int	txtindex_count(txtindex *) noex ;
extern int	txtindex_neigen(txtindex *) noex ;
extern int	txtindex_info(txtindex *,TXTINDEX_INFO *) noex ;
extern int	txtindex_iseigen(txtindex *,cchar *,int) noex ;
extern int	txtindex_curbegin(txtindex *,txtindex_cur *) noex ;
extern int	txtindex_lookup(txtindex *,txtindex_cur *,cchar **) noex ;
extern int	txtindex_read(txtindex *,txtindex_cur *,TXTINDEX_TAG *) noex ;
extern int	txtindex_curend(txtindex *,txtindex_cur *) noex ;
extern int	txtindex_audit(txtindex *) noex ;
extern int	txtindex_close(txtindex *) noex ;

EXTERNC_end


#endif /* TXTINDEX_INCLUDE */


