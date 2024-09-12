/* pcs-config HEADER */
/* lang=C20 */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

#ifndef	PCSCONFIG_INCLUDE
#define	PCSCONFIG_INCLUDE


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


#define	CONFIG		struct config_head
#define	CONFIG_FL	struct config_flags


struct config_flags {
	uint		p:1 ;
	uint		lockinfo:1 ;
} ;

struct config_head {
	PROGINFO	*pip ;
	PARAMFILE	p ;
	expcook		cooks ;
	time_t		ti_lastcheck ;
	CONFIG_FL	f ;
	int		intcheck ;
} ;

typedef	CONFIG		config ;
typedef	CONFIG_FL	config_fl ;

EXTERNC_begin

extern int	config_start(CONFIG *,PROGINFO *,cchar *,int) noex ;
extern int	config_check(CONFIG *) noex ;
extern int	config_read(CONFIG *) noex ;
extern int	config_finish(CONFIG *) noex ;

EXTERNC_end


#endif /* PCSCONFIG_INCLUDE */


