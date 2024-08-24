/* randomvar HEADER*/
/* lang=C20 */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	RANDOMVAR_INCLUDE
#define	RANDOMVAR_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<time.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<clanguage.h>
#include	<localmisc.h>


#define	RANDOMVAR_MAGIC		0x97857322
#define	RANDOMVAR_DEGREE	128
#define	RANDOMVAR_STATELEN	(RANDOMVAR_DEGREE * sizeof(long))
#define	RANDOMVAR_STIRTIME	(5 * 60)

#define	RANDOMVAR		struct randomvar_head
#define	RANDOMVAR_FL		struct randomvar_flags
#define	RANDOMVAR_ST		union randomvar_state


struct randomvar_flags {
	uint		pseudo:1 ;
	uint		flipper:1 ;
} ;

union randomvar_state {
	long		ls[RANDOMVAR_DEGREE] ;
	int		is[RANDOMVAR_DEGREE * 2] ;
} ;
	
struct randomvar_head {
	RANDOMVAR_ST	state ;
	time_t		laststir ;
	RANDOMVAR_FL	f ;
	uint		magic ;
	int		maintcount ;
	int		a, b, c ;
} ;

typedef RANDOMVAR		randomvar ;

EXTERNC_begin

extern int randomvar_start(RANDOMVAR *,int,uint) noex ;
extern int randomvar_stateload(RANDOMVAR *,cchar *,int) noex ;
extern int randomvar_statesave(RANDOMVAR *,char *,int) noex ;
extern int randomvar_setpoly(RANDOMVAR *,int,int) noex ;
extern int randomvar_addnoise(RANDOMVAR *,cvoid *,int) noex ;
extern int randomvar_getlong(RANDOMVAR *,long *) noex ;
extern int randomvar_getulong(RANDOMVAR *,ulong *) noex ;
extern int randomvar_getint(RANDOMVAR *,int *) noex ;
extern int randomvar_getuint(RANDOMVAR *,uint *) noex ;
extern int randomvar_finish(RANDOMVAR *) noex ;

EXTERNC_end


#endif /* RANDOMVAR_INCLUDE */


