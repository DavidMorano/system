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

#include	"txtindexmks.h"


#define	TXTINDEXMK_MAGIC	0x99447246
#define	TXTINDEXMK		struct txtindexmk_head
#define	TXTINDEXMK_ENTS		struct txtindexmk_calls
#define	TXTINDEXMK_PA		TXTINDEXMKS_PA
#define	TXTINDEXMK_TAG		TXTINDEXMKS_TAG
#define	TXTINDEXMK_KEY		TXTINDEXMKS_KEY


EXTERNC_begin
struct txtindexmk_calls {
	int	(*open)(void *,TXTINDEXMKS_PA *,cchar *,int,int) noex ;
	int	(*addeigens)(void *,TXTINDEXMKS_KEY *,int) noex ;
	int	(*addtags)(void *,TXTINDEXMKS_TAG *,int) noex ;
	int	(*noop)(void *) noex ;
	int	(*abort)(void *) noex ;
	int	(*close)(void *) noex ;
} ;
EXTERNC_end

struct txtindexmk_head {
	modload		*lop ;		/* load-object-pointer */
	void		*obj ;		/* object pointer */
	TXTINDEXMK_ENTS	call ;
	uint		magic ;
	int		objsize ;	/* object size */
	int		cursize ;	/* cursor size (not used here) */
	int		nfd ;
} ;

typedef	TXTINDEXMK	txtindexmk ;
typedef	TXTINDEXMK_ENTS	txtindexmk_ents ;
typedef	TXTINDEXMK_PA	txtindexmk_pa ;
typedef	TXTINDEXMK_TAG	txtindexmk_tag ;
typedef	TXTINDEXMK_KEY	txtindexmk_key ;

EXTERNC_begin

extern int txtindexmk_open(txtindexmk *,txtindexmk_pa *,cc *,int,mode_t) noex ;
extern int txtindexmk_addeigens(txtindexmk *,txtindexmk_key *,int) ;
extern int txtindexmk_addtags(txtindexmk *,txtindexmk_tag *,int) ;
extern int txtindexmk_noop(txtindexmk *) ;
extern int txtindexmk_abort(txtindexmk *) ;
extern int txtindexmk_close(txtindexmk *) ;

EXTERNC_end


#endif /* TXTINDEXMK_INCLUDE */


