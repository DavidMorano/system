/* biblebooks HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* BIBLEBOOKS object implementation */
/* version %I% last-modified %G% */


/* revision history:

	- 2008-10-01, David A­D­ Morano
	This module was originally written for hardware CAD support.

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

#ifndef	BIBLEBOOKS_INCLUDE
#define	BIBLEBOOKS_INCLUDE


#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<vecpstr.h>		/* LIBUC */


#define	BIBLEBOOKS		struct biblebooks_head
#define	BIBLEBOOKS_OBJ		struct biblebooks_object
#define	BIBLEBOOKS_MAGIC	0x99447243


struct biblebooks_object {
	cchar		*name ;
	uint		objsz ;
	uint		cursz ;
} ; /* end struct */

struct biblebooks_head {
	vecpstr		db ;
	uint		magval ;
} ; /* end struct */

typedef	BIBLEBOOKS		biblebooks ;
typedef	BIBLEBOOKS_OBJ		biblebooks_obj ;

EXTERNC_begin

extern int	biblebooks_open		(biblebooks *,cchar *,cchar *) noex ;
extern int	biblebooks_count	(biblebooks *) noex ;
extern int	biblebooks_max		(biblebooks *) noex ;
extern int	biblebooks_lookup	(biblebooks *,char *,int,int) noex ;
extern int	biblebooks_get		(biblebooks *,int,char *,int) noex ;
extern int	biblebooks_size		(biblebooks *) noex ;
extern int	biblebooks_audit	(biblebooks *) noex ;
extern int	biblebooks_close	(biblebooks *) noex ;

EXTERNC_end


#endif /* BIBLEBOOKS_INCLUDE */


