/* calyear HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* CALYEAR object loader */
/* version %I% last-modified %G% */


/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

#ifndef	CALYEAR_INCLUDE
#define	CALYEAR_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<modload.h>		/* LIBDAM */
#include	<calyears.h>		/* LIBDAM */


#define	CALYEAR		struct calyear_head
#define	CALYEAR_FL	struct calyear_flags
#define	CALYEAR_Q	struct calyear_query
#define	CALYEAR_QUERY	struct calyear_query
#define	CALYEAR_CITE	struct calyear_query
#define	CALYEAR_CUR	struct calyear_cursor
#define	CALYEAR_MAGIC	0x99447246


struct calyear_flags {
    	uint		modload:1 ;
} ; /* end struct */

struct calyear_query {
	ushort		y ;
	uchar		m, d ;
} ; /* end struct */

struct calyear_cursor {
	void		*scp ;
	uint		magval ;
} ; /* end struct */

struct calyear_head {
	modload		*mlp ;
	void		*callp ;
	void		*obj ;		/* object pointer */
	CALYEAR_FL	fl ;
	uint		magval ;
	int		objsz ;		/* object size */
	int		cursz ;		/* cursor size */
} ; /* end struct */

typedef	CALYEAR		calyear ;
typedef	CALYEAR_FL	calyear_fl ;
typedef	CALYEAR_Q	calyear_q ;
typedef	CALYEAR_CUR	calyear_cur ;

EXTERNC_begin

extern int calyear_open(calyear *,cchar *,con mainv,con mainv) noex ;
extern int calyear_count(calyear *) noex ;
extern int calyear_curbegin(calyear *,calyear_cur *) noex ;
extern int calyear_curlook(calyear *,calyear_cur *,con calyear_q *) noex ;
extern int calyear_curread(calyear *,calyear_cur *,mut calyear_q *,
			char *,int) noex ;
extern int calyear_curend(calyear *,calyear_cur *) noex ;
extern int calyear_check(calyear *,time_t) noex ;
extern int calyear_audit(calyear *) noex ;
extern int calyear_close(calyear *) noex ;

EXTERNC_end


#endif /* CALYEAR_INCLUDE */


