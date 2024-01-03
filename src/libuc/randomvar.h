/* randomvar HEADER */
/* lang=C20 */

/* random-variable object */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	RANDOMVAR_INCLUDE
#define	RANDOMVAR_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<time.h>
#include	<usystem.h>
#include	<stdintx.h>


#define	RANDOMVAR_MAGIC		0x97857322
#define	RANDOMVAR_DEGREE	128
#define	RANDOMVAR_DIGIT		ulong
#define	RANDOMVAR_STATELEN	(RANDOMVAR_DEGREE * sizeof(RANDOMVAR_DIGIT))
#define	RANDOMVAR_STIRTIME	(5 * 60)

#define	RANDOMVAR		struct randomvar_head
#define	RANDOMVAR_FL		struct randomvar_flags
#define	RANDOMVAR_ST		union randomvar_state


struct randomvar_flags {
	uint		pseudo:1 ;
	uint		flipper:1 ;
} ;

union randomvar_state {
	ulong		ls[RANDOMVAR_DEGREE] ;
	uint		is[RANDOMVAR_DEGREE * 2] ;
} ;
	
struct randomvar_head {
	RANDOMVAR_FL	f ;
	RANDOMVAR_ST	state ;
	time_t		laststir ;
	uint		magic ;
	int		maintcount ;
	int		a, b, c ;
} ;

typedef RANDOMVAR	randomvar ;
typedef RANDOMVAR_FL	randomvar_fl ;
typedef RANDOMVAR_ST	randomvar_st ;

#ifdef	__cplusplus
extern "C" {
#endif

extern int randomvar_start(randomvar *,int,uint) noex ;
extern int randomvar_stateload(randomvar *,cchar *,int) noex ;
extern int randomvar_statesave(randomvar *,char *,int) noex ;
extern int randomvar_setpoly(randomvar *,int,int) noex ;
extern int randomvar_addnoise(randomvar *,cvoid *,int) noex ;
extern int randomvar_getlong(randomvar *,long *) noex ;
extern int randomvar_getulong(randomvar *,ulong *) noex ;
extern int randomvar_getint(randomvar *,int *) noex ;
extern int randomvar_getuint(randomvar *,uint *) noex ;
extern int randomvar_finish(randomvar *) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* RANDOMVAR_INCLUDE */


