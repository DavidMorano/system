/* biblebooks HEADER */
/* encoding=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */


/* revision history:

	- 2008-10-01, David A­D­ Morano
	This module was originally written for hardware CAD support.

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

#ifndef	BIBLEBOOKS_INCLUDE
#define	BIBLEBOOKS_INCLUDE


#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<vecpstr.h>


#define	BIBLEBOOKS_MAGIC	0x99447243
#define	BIBLEBOOKS		struct biblebooks_head
#define	BIBLEBOOKS_OBJ		struct biblebooks_object


struct biblebooks_object {
	cchar		*name ;
	uint		objsize ;
	uint		cursize ;
} ;

struct biblebooks_head {
	uint		magic ;
	vecpstr		db ;
} ;

typedef	BIBLEBOOKS		biblebooks ;
typedef	BIBLEBOOKS_OBJ		biblebooks_obj ;

EXTERNC_begin

extern int	biblebooks_open(BIBLEBOOKS *,cchar *,cchar *) noex ;
extern int	biblebooks_count(BIBLEBOOKS *) noex ;
extern int	biblebooks_max(BIBLEBOOKS *) noex ;
extern int	biblebooks_lookup(BIBLEBOOKS *,char *,int,int) noex ;
extern int	biblebooks_get(BIBLEBOOKS *,int,char *,int) noex ;
extern int	biblebooks_size(BIBLEBOOKS *) noex ;
extern int	biblebooks_audit(BIBLEBOOKS *) noex ;
extern int	biblebooks_close(BIBLEBOOKS *) noex ;

EXTERNC_end


#endif /* BIBLEBOOKS_INCLUDE */


