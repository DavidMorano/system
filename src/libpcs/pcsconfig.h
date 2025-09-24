/* pcsconfig HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

#ifndef	PCSPCSCONFIG_INCLUDE
#define	PCSPCSCONFIG_INCLUDE


#include	<envstandards.h>
#include	<sys/types.h>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<msfile.h>
#include	<paramfile.h>
#include	<expcook.h>

#include	"pcsmain.h"
#include	"defs.h"		/* for PROGINFO */


#define	PCSCONFIG	struct pcsconfig_head
#define	PCSCONFIG_FL	struct pcsconfig_flags


struct pcsconfig_flags {
	uint		p:1 ;
	uint		lockinfo:1 ;
} ;

struct pcsconfig_head {
	proginfo	*pip ;
	paramfile	p ;
	expcook		cooks ;
	time_t		ti_lastcheck ;
	PCSCONFIG_FL	fl ;
	int		intcheck ;
} ;

typedef	PCSCONFIG	pcsconfig ;
typedef	PCSCONFIG_FL	pcsconfig_fl ;

EXTERNC_begin

extern int	pcsconfig_start(pcsconfig *,proginfo *,cchar *,int) noex ;
extern int	pcsconfig_check(pcsconfig *) noex ;
extern int	pcsconfig_read(pcsconfig *) noex ;
extern int	pcsconfig_finish(pcsconfig *) noex ;

EXTERNC_end


#endif /* PCSPCSCONFIG_INCLUDE */


