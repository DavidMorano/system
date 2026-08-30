/* randomvar HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* random-variable object */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-08-11, David A­D­ Morano
	This object module was originally written.

*/

/*******************************************************************************

  	Object:
	randomvar

	Description:
	This code is part of the UNIX® standard-library interface layer
	library ("libuc").

*******************************************************************************/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	RANDOMVAR_INCLUDE
#define	RANDOMVAR_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX® system types */
#include	<time.h>		/* CSTD |time_t| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<stdintx.h>		/* LIBU */


#define	RANDOMVAR		struct randomvar_head
#define	RANDOMVAR_FL		struct randomvar_flags
#define	RANDOMVAR_MAGIC		0x97857322
#define	RANDOMVAR_DEGREE	127
#define	RANDOMVAR_DIGIT		ulong
#define	RANDOMVAR_STATELEN	(RANDOMVAR_DEGREE+1)
#define	RANDOMVAR_STIRINT	(5 * 60)


struct randomvar_flags {
	uint		pseudo:1 ;
	uint		flipper:1 ;
} ; /* end struct */

struct randomvar_head {
	ulong		*state ;
	time_t		laststir ;
	RANDOMVAR_FL	fl ;
	uint		magval ;
	int		maintcount ;
	int		a, b, c ;
} ; /* end struct */

#ifdef	__cplusplus
enum randomvarmems {
	randomvarmem_finish,
	randomvarmem_overlast
} ; /* end enum */
struct randomvar ;
struct randomvar_st {
	randomvar	*op = nullptr ;
	int		w = -1 ;
	void operator () (randomvar *p,int m) noex {
	    op = p ;
	    w = m ;
	} ; /* end */
	int operator () (int = 0,uint = 0) noex ;
	operator int () noex {
	    return operator () () ;
	} ; /* end */
} ; /* end struct (randomvar_st) */
struct randomvar_co {
	randomvar	*op = nullptr ;
	int		w = -1 ;
	void operator () (randomvar *p,int m) noex {
	    op = p ;
	    w = m ;
	} ; /* end */
	operator int () noex ;
	int operator () () noex { 
	    return operator int () ;
	} ; /* end */
} ; /* end struct (randomvar_co) */
struct randomvar : randomvar_head {
	randomvar_st	start ;
	randomvar_co	finish ;
	randomvar() noex {
	    start	(this,0) ;
	    finish	(this,randomvarmem_finish) ;
	    magval	= 0 ;
	} ; /* end ctor */
	randomvar(const randomvar &) = delete ;
	randomvar &operator = (const randomvar &) = delete ;
	int stateload	(cchar *,int) noex ;
	int statesave	(char *,int) noex ;
	int setpoly	(int,int) noex ;
	int addnoise	(cvoid *,int) noex ;
	int getlong	(long *) noex ;
	int getulong	(ulong *) noex ;
	int getint	(int *) noex ;
	int getuint	(uint *) noex ;
	int get		(void *,int) noex ;
	int get		(long *) noex ;
	int get		(ulong *) noex ;
	int get		(int *) noex ;
	int get		(uint *) noex ;
	void dtor	() noex ;
	destruct randomvar() {
	    if (magval) dtor() ;
	} ; /* end destruct */
} ; /* end struct (randomvar) */
#else	/* __cplusplus */
typedef RANDOMVAR	randomvar ;
#endif /* __cplusplus */

typedef RANDOMVAR_FL	randomvar_fl ;

EXTERNC_begin

extern int randomvar_start	(randomvar *,int,uint) noex ;
extern int randomvar_stateload	(randomvar *,cchar *,int) noex ;
extern int randomvar_statesave	(randomvar *,char *,int) noex ;
extern int randomvar_setpoly	(randomvar *,int,int) noex ;
extern int randomvar_addnoise	(randomvar *,cvoid *,int) noex ;
extern int randomvar_getlong	(randomvar *,long *) noex ;
extern int randomvar_getulong	(randomvar *,ulong *) noex ;
extern int randomvar_getint	(randomvar *,int *) noex ;
extern int randomvar_getuint	(randomvar *,uint *) noex ;
extern int randomvar_get	(randomvar *,void *,int) noex ;
extern int randomvar_finish	(randomvar *) noex ;

EXTERNC_end

#ifdef	__cplusplus

inline int randomvar_get(randomvar *op,int *p) noex {
	return randomvar_getint(op,p) ;
} /* end */
inline int randomvar_get(randomvar *op,long *p) noex {
	return randomvar_getlong(op,p) ;
} /* end */
inline int randomvar_get(randomvar *op,uint *p) noex {
	return randomvar_getuint(op,p) ;
} /* end */
inline int randomvar_get(randomvar *op,ulong *p) noex {
	return randomvar_getulong(op,p) ;
} /* end */

#endif /* __cplusplus */


#endif /* RANDOMVAR_INCLUDE */


