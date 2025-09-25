/* mfs-config HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

	= 2017-08-10, David A­D­ Morano
	This subroutine was borrowed to code MFSERVE.

*/

/* Copyright © 2000,2017 David A­D­ Morano.  All rights reserved. */

#ifndef	MFSCONFIG_INCLUDE
#define	MFSCONFIG_INCLUDE


#include	<envstandards.h>
#include	<sys/types.h>
#include	<msfile.h>
#include	<paramfile.h>
#include	<expcook.h>

#include	"mfsmain.h"
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
	time_t		ti_lastcheck ;
	CONFIG_FL	fl ;
	int		intcheck ;
} ;

typedef	CONFIG		config ;
typedef	CONFIG_FL	config_fl ;

EXTERNC_begin

extern int	config_start(CONFIG *,proginfo *,cchar *,int) noex ;
extern int	config_check(CONFIG *) noex ;
extern int	config_read(CONFIG *) noex ;
extern int	config_finish(CONFIG *) noex ;

EXTERNC_end


#endif /* MFSCONFIG_INCLUDE */


