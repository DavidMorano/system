/* msu-config HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* handle MSU configuration functions */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

#ifndef	MSUCONFIG_INCLUDE
#define	MSUCONFIG_INCLUDE


#include	<envstandards.h>	/* must be ordered first to configure */
#include	<sys/types.h>
#include	<msfile.h>
#include	<paramfile.h>
#include	<expcook.h>

#include	"msumain.h"
#include	"defs.h"		/* for PROGINFO */


#define	CONFIG		struct config
#define	CONFIG_FL	struct config_flags


struct config_flags {
	uint		p:1 ;
	uint		lockinfo:1 ;
} ;

struct config {
	proginfo	*pip ;
	paramfile	p ;
	expcool		cooks ;
	time_t		ti_lastcheck ;
	CONFIG_FL	fl ;
	int		intcheck ;
} ;

EXTERNC_begin

extern int	config_start(CONFIG *,proginfo *,cchar *,int) noex ;
extern int	config_check(CONFIG *) noex ;
extern int	config_read(CONFIG *) noex ;
extern int	config_finish(CONFIG *) noex ;

EXTERNC_end


#endif /* MSUCONFIG_INCLUDE */


