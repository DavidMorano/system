/* builtin HEADER */
/* encoding=ISO8859-1 */
/* lang=C20 */


/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

#ifndef	BUILTIN_INCLUDE
#define	BUILTIN_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<time.h>
#include	<vecstr.h>
#include	<svcfile.h>
#include	<connection.h>
#include	<localmisc.h>

#include	"config.h"
#include	"defs.h"
#include	"clientinfo.h"
#include	"standing.h"


#define	BUILTIN_MAGIC	0x12345678
#define	BUILTIN		struct builtin
#define	BUILTIN_CACHE	struct builtin_cache


struct builtin_cache {
	time_t		boottime ;
} ;

struct builtin {
	unsigned long	magic ;
	BUILTIN_CACHE	c ;
	SVCFILE		*sfp ;
	PROGINFO	*pip ;
	uint		providerid ;
	int		nentries ;
	int		hostnamelen ;
} ;

EXTERNC_begin

extern int builtin_start(BUILTIN *,PROGINFO *) noex ;
extern int builtin_finish(BUILTIN *) noex ;
extern int builtin_match(BUILTIN *,const char *) noex ;
extern int builtin_enum(BUILTIN *,int,const char **) noex ;
extern int builtin_execute(BUILTIN *,STANDING *,CLIENTINFO *,int,
		cchar **) noex ;

EXTERNC_end


#endif /* BUILTIN_INCLUDE */


