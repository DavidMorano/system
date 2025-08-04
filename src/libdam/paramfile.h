/* paramfile HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* object to handle parameter files */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-02-17, David A­D­ Morano
	This code was started for Levo related work.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

#ifndef	PARAMFILE_INCLUDE
#define	PARAMFILE_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/param.h>
#include	<limits.h>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<vecobj.h>
#include	<vecstr.h>
#include	<varsub.h>


#define	PARAMFILE		struct paramfile_head
#define	PARAMFILE_FL		struct paramfile_flags
#define	PARAMFILE_CUR		struct paramfile_cursor
#define	PARAMFILE_ENT		struct paramfile_entry
#define	PARAMFILE_ERROR		struct paramfile_errline

#define	PARAMFILE_MAGIC		0x12349876
#define	PARAMFILE_INTCHECK	2	/* file check interval (seconds) */
#define	PARAMFILE_INTCHANGE	2	/* wait change interval (seconds) */
#define	PARAMFILE_VALLEN	60	/* starting value-length */


struct paramfile_flags {
	uint		envinit:1 ;
	uint		envload:1 ;
	uint		definit:1 ;
	uint		defload:1 ;
} ;

struct paramfile_head {
	mainv		envv ;		/* program startup environment */
	cchar		*a ;		/* memory allocation */
	char		*lbuf ;
	char		*fbuf ;
	vecobj		*filep ;	/* files */
	vecobj		*entsp ;	/* parameter entries */
	varsub		*defp ;		/* defines */
	varsub		*envp ;		/* environment-variables */
	time_t		ti_check ;	/* time last checked */
	PARAMFILE_FL	fl ;
	uint		magic ;
	int		llen ;
	int		flen ;
	int		intcheck ;
} ;

struct paramfile_entry {
	cchar		*key ;
	cchar		*oval ;
	cchar		*value ;	/* dynamic variable expansion */
	int		fi ;
	int		klen ;
	int		olen ;
	int		vlen ;
} ;

struct paramfile_cursor {
	int		i ;
} ;

typedef PARAMFILE_CUR		paramfile_cur ;
typedef PARAMFILE_ENT		paramfile_ent ;

#ifdef	__cplusplus
enum paramfilemems {
    	paramfilemem_checkint,
    	paramfilemem_close,
	paramfilemem_overlast
} ; /* end enum (paramfilemems) */
struct paramfile ;
struct paramfile_co {
	paramfile	*op = nullptr ;
	int		w = -1 ;
	void operator () (paramfile *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	int operator () (int = 1) noex ;
	operator int () noex {
	    return operator () (1) ;
	}
} ; /* end struct (paramfile_co) */
struct paramfile : paramfile_head {
    	paramfile_co	checkint ;
    	paramfile_co	close ;
	paramfile() noex {
	    checkint(this,paramfilemem_checkint) ;
	    close(this,paramfilemem_close) ;
	} ;
    	int open(mainv,cchar * = nullptr) noex ;
	int curbegin(paramfile_cur *) noex ;
	int curend(paramfile_cur *) noex ;
	int curenum(paramfile_cur *,paramfile_ent *,char *,int) noex ;
	int check(time_t = 0L) noex ;
	void dtor() noex ;
	destruct paramfile() {
	    if (magic) dtor() ;
	} ;
} ; /* end struct (paramfile) */
#else
typedef PARAMFILE		paramfile ;
#endif /* __cplusplus */

EXTERNC_begin

extern int paramfile_open(paramfile *,mainv,cchar *) noex ;
extern int paramfile_fileadd(paramfile *,cchar *) noex ;
extern int paramfile_setdefines(paramfile *,vecstr *) noex ;
extern int paramfile_curbegin(paramfile *,paramfile_cur *) noex ;
extern int paramfile_curend(paramfile *,paramfile_cur *) noex ;
extern int paramfile_fetch(paramfile *,cchar *,paramfile_cur *,
		char *,int) noex ;
extern int paramfile_curenum(paramfile *,paramfile_cur *,paramfile_ent *,
		char *,int) noex ;
extern int paramfile_checkint(paramfile *,int) noex ;
extern int paramfile_check(paramfile *,time_t) noex ;
extern int paramfile_close(paramfile *) noex ;

EXTERNC_end


#endif /* PARAMFILE_INCLUDE */


