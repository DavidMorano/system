/* builtin HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */


/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

#ifndef	BUILTIN_INCLUDE
#define	BUILTIN_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<time.h>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<vecstr.h>
#include	<svcfile.h>
#include	<connection.h>

#include	"config.h"
#include	"defs.h"
#include	"clientinfo.h"
#include	"standing.h"


#define	BUILTIN_MAGIC	0x12345678
#define	BUILTIN		struct builtin_head
#define	BUILTIN_CA	struct builtin_cache


struct builtin_cache {
	time_t		boottime ;
} ;

struct builtin_head {
	svcfile		*sfp ;
	proginfo	*pip ;
	BUILTIN_CACHE	c ;
	uint		magic ;
	uint		providerid ;
	int		nentries ;
	int		hostnamelen ;
} ;

typedef	BUILTIN		builtin ;
typedef	BUILTIN_CA	builtin_ca ;

EXTERNC_begin

extern int builtin_start(builtin *,proginfo *) noex ;
extern int builtin_finish(builtin *) noex ;
extern int builtin_match(builtin *,cchar *) noex ;
extern int builtin_enum(builtin *,int,cchar **) noex ;
extern int builtin_execute(builtin *,standing *,clientinfo *,int,cc **) noex ;

EXTERNC_end


#endif /* BUILTIN_INCLUDE */


