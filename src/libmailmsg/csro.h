/* csro HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* email ComSat Receive Offset (CSRO) processing */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	CSRO_INCLUDE
#define	CSRO_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* system types - |off_t| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<hdb.h>
#include	<vecstr.h>
#include	<vecobj.h>


#define	CSRO_MAGIC	0x99723317
#define	CSRO_DEFENTS	10
#define	CSRO		struct csro_head
#define	CSRO_VAL	struct csro_value
#define	CSRO_NCUR	int
#define	CSRO_VCUR	int


struct csro_head {
	vecstr		*nlp ;			/* name-list-pointer */
	vecobj		*elp ;			/* entry-list-pointer */
	uint		magic ;
} ;

struct csro_value {
	cchar		*mailname ;
	cchar		*fname ;
	off_t		moff ;
} ;

typedef	CSRO		csro ;
typedef	CSRO_VAL	csro_val ;
typedef	CSRO_NCUR	csro_ncur ;
typedef	CSRO_VCUR	csro_vcur ;

EXTERNC_begin

extern int csro_start(csro *,int) noex ;
extern int csro_finish(csro *) noex ;
extern int csro_add(csro *,cchar *,cchar *,off_t) noex ;
extern int csro_countnames(csro *) noex ;
extern int csro_count(csro *) noex ;
extern int csro_ncurbegin(csro *,csro_ncur *) noex ;
extern int csro_ncurend(csro *,csro_ncur *) noex ;
extern int csro_getname(csro *,csro_ncur *,cchar **) noex ;
extern int csro_vcurbegin(csro *,csro_vcur *) noex ;
extern int csro_vcurend(csro *,csro_vcur *) noex ;
extern int csro_getvalue(csro *,cchar *,csro_vcur *,csro_val **) noex ;
extern int csro_already(csro *,cchar *,cchar *,off_t) noex ;

EXTERNC_end


#endif /* CSRO_INCLUDE */


