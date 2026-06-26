/* txtindexmk HEADER */
/* charset=ISO8859-1 */
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


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<modload.h>		/* LIBUC */
#include	<txtindexmks.h>		/* LIBDAM */


#define	TXTINDEXMK		struct txtindexmk_head
#define	TXTINDEXMK_FL		struct txtindexmk_flags
#define	TXTINDEXMK_PA		txtindexmks_pa
#define	TXTINDEXMK_TAG		txtindexmks_tag
#define	TXTINDEXMK_KEY		txtindexmks_key
#define	TXTINDEXMK_MAGIC	0x99447246


struct txtindexmk_flags {
    	uint		modload:1 ;
} ; /* end struct */

struct txtindexmk_head {
	modload		*mlp ;		/* load-object-pointer */
	void		*callp ;
	void		*obj ;		/* object pointer */
	TXTINDEXMK_FL	fl ;
	uint		magval ;
	int		objsz ;		/* object size */
	int		cursz ;		/* cursor size (not used here) */
	int		nfd ;
} ; /* end struct */

typedef	TXTINDEXMK	txtindexmk ;
typedef	TXTINDEXMK_FL	txtindexmk_fl ;
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


