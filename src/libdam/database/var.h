/* var HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* VAR management */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	VAR_INCLUDE
#define	VAR_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>		/* POSIX® system types */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<modload.h>		/* LIBUC */
#include	<vars.h>		/* LIBDAM */


#define	VAR		struct var_head
#define	VAR_FL		struct var_flags
#define	VAR_CUR		struct var_cursor
#define	VAR_INFO	struct var_information
#define	VAR_MAGIC	0x99447246


struct var_flags {
    	uint		modload:1 ;
} ; /* end struct (var_flags) */

struct var_information {
	time_t		wtime ;
	time_t		mtime ;
	uint		nvars ;
	uint		nskip ;
} ; /* end struct */

struct var_cursor {
	void		*scp ;		/* SO-cursor pointer */
	uint		magval ;
} ; /* end struct */

struct var_head {
	modload		*mlp ;		/* module-load-pointer */
	void		*callp ;	/* */
	void		*obj ;		/* object pointer */
	VAR_FL		fl ;
	uint		magval ;
	int		objsz ;		/* object size */
	int		cursz ;		/* cursor size */
} ; /* end struct */

typedef	VAR		var ;
typedef VAR_FL		var_fl ;
typedef VAR_CUR		var_cur ;
typedef VAR_INFO	var_info ;

EXTERNC_begin

extern int	var_open(var *,cchar *) noex ;
extern int	var_count(var *) noex ;
extern int	var_curbegin(var *,var_cur *) noex ;
extern int	var_fetch(var *,cchar *,int,var_cur *,char *,int) noex ;
extern int	var_curenum(var *,var_cur *,char *,int,char *,int) noex ;
extern int	var_curend(var *,var_cur *) noex ;
extern int	var_getinfo(var *,var_info *) noex ;
extern int	var_audit(var *) noex ;
extern int	var_close(var *) noex ;

#ifdef	COMMENT
extern int	varinfo_get(varinfo_dat *,cchar *,int) noex ;
extern int	varunlink(cchar *,int) noex ;
#endif

EXTERNC_end


#endif /* VAR_INCLUDE */


