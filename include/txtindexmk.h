/* txtindexmk HEADER */
/* lang=C20 */

/* interface to the TXTINDEXMKS loadable object */
/* version %I% last-modified %G% */


/* revision history:

	= 2008-02-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

#ifndef	TXTINDEXMK_INCLUDE
#define	TXTINDEXMK_INCLUDE


#include	<envstandards.h>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<modload.h>

#include	<txtindexmks.h>


#define	TXTINDEXMK_MAGIC	0x99447246
#define	TXTINDEXMK		struct txtindexmk_head
#define	TXTINDEXMK_PA		TXTINDEXMKS_PA
#define	TXTINDEXMK_TAG		TXTINDEXMKS_TAG
#define	TXTINDEXMK_KEY		TXTINDEXMKS_KEY


struct txtindexmk_head {
	modload		*mlp ;		/* load-object-pointer */
	void		*callp ;
	void		*obj ;		/* object pointer */
	uint		magic ;
	int		objsize ;	/* object size */
	int		cursize ;	/* cursor size (not used here) */
	int		nfd ;
} ;

typedef	TXTINDEXMK	txtindexmk ;
typedef	TXTINDEXMK_PA	txtindexmk_pa ;
typedef	TXTINDEXMK_TAG	txtindexmk_tag ;
typedef	TXTINDEXMK_KEY	txtindexmk_key ;

EXTERNC_begin

extern int txtindexmk_open(txtindexmk *,txtindexmk_pa *,cc *,int,mode_t) noex ;
extern int txtindexmk_addeigens(txtindexmk *,txtindexmk_key *,int) noex ;
extern int txtindexmk_addtags(txtindexmk *,txtindexmk_tag *,int) noex ;
extern int txtindexmk_noop(txtindexmk *) noex ;
extern int txtindexmk_abort(txtindexmk *) noex ;
extern int txtindexmk_close(txtindexmk *) noex ;

EXTERNC_end


#endif /* TXTINDEXMK_INCLUDE */


