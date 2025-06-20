/* sysdialer HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* system-dialer storage object */
/* version %I% last-modified %G% */


/* revision history:

	= 2003-02-01, David A­D­ Morano
	This subroutine was adopted for use from the DWD program.

*/

/* Copyright © 2003 David A­D­ Morano.  All rights reserved. */

#ifndef	SYSDIALER_INCLUDE
#define	SYSDIALER_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* system types */
#include	<usystem.h>		/* for |ino_t| */
#include	<vecobj.h>
#include	<vecstr.h>


/* striaght up */
#define	SYSDIALER_MAGIC	0x31815927
#define	SYSDIALER	struct sysdialer_head
#define	SYSDIALER_FL	struct sysdialer_flags
#define	SYSDIALER_ENT	struct sysdialer_entry
#define	SYSDIALER_CALLS	struct sysdialer_calls
#define	SYSDIALER_INFO	struct sysdialer_information
#define	SYSDIALER_ARGS	struct sysdialer_arguments
#define	SYSDIALER_PRC	struct sysdialer_prcache
#define	SYSDIALER_MOD	struct sysdialer_module
#define	SYSDIALER_LF	"sysdialer"
/* optional */
#ifdef	COMMENT
#define	INTERFACE	struct interface
#endif
/* option masks */
enum sysdialeros {
    	sysdialero_full,
    	sysdialero_halfout,
    	sysdialero_halfin,
    	sysdialero_cor,
    	sysdialero_co,
    	sysdialero_cl,
    	sysdialero_nargs,
    	sysdialero_overlast,
    	sysdialero_rdonly = sysdialero_halfin,
    	sysdialero_wronly = sysdialero_halfout
} ;

#ifdef	__cplusplus

struct sysdialerms {
	static cint	full ;
	static cint	halfout ;
	static cint	halfin ;
	static cint	cor ;
	static cint	co ;
	static cint	cl ;
	static cint	nargs ;
} ;

#endif /* __cplusplus */

#define	SYSDIALER_MFULL		(1 << sysdialero_full)
#define	SYSDIALER_MHALFOUT	(1 << sysdialero_halfout)
#define	SYSDIALER_MHALFIN	(1 << sysdialero_halfin)
#define	SYSDIALER_MCOR		(1 << sysdialero_cor)
#define	SYSDIALER_MCO		(1 << sysdialero_co)
#define	SYSDIALER_MCL		(1 << sysdialero_cl)
#define	SYSDIALER_MARGS		(1 << sysdialero_nargs)
/* option mash aliases */
#define	SYSDIALER_MRDONLY	SYSDIALER_MHALFIN
#define	SYSDIALER_MWRONLY	SYSDIALER_MHALFOUT


struct sysdialer_information {
	cchar		*name ;
	cchar		*version ;
	cchar		*itype ;
	int		osize ;
	int		flags ;
} ;

struct sysdialer_flags {
	uint		vsprs:1 ;
	uint		vsdirs:1 ;
	uint		voents:1 ;
	uint		prcache:1 ;
} ;

struct sysdialer_prcache {
	cchar		*domainname ;
	mainv		prs ;
} ;

struct sysdialer_head {
	cchar		*pr ;
	mainv		dirs ;
	vecobj		*elp ;		/* entry-list-pointer */
	vecstr		*plp ;		/* pr-list-pointer */
	vecstr		*dlp ;		/* directory-list-pointer */
	void		*callp ;	/* calls-structure pointer */
	SYSDIALER_PRC	pc ;
	SYSDIALER_FL	fl ;
	time_t		ti_lastcheck ;
	uint		magic ;
} ;

struct sysdialer_module {
	void		*dhp ;		/* SO-dlopen handle */
	dev_t		dev ;
	ino_t		ino ;
	int		count ;
} ;

struct sysdialer_entry {
	cchar		*name ;
	cchar		*itype ;
	SYSDIALER_MOD	*mp ;
	void		*callp ;
	int		osize ;		/* object size */
	int		flags ;
	int		count ;
} ;

struct sysdialer_arguments {
	cchar		*pr ;		/* program root */
	cchar		*prn ;		/* program root-name */
	cchar		*ip ;		/* interface */
	mainv		argv ;
	mainv		envv ;
	int		timeout ;
	int		options ;
} ;

#ifdef	COMMENT
struct interface {
	char		*name ;
	int		osize ;
} ;
#endif /* COMMENT */

typedef	SYSDIALER	sysdialer ;
typedef	SYSDIALER_FL	sysdialer_fl ;
typedef	SYSDIALER_ENT	sysdialer_ent ;
typedef	SYSDIALER_MOD	sysdialer_mod ;
typedef	SYSDIALER_INFO	sysdialer_info ;
typedef	SYSDIALER_ARGS	sysdialer_args ;
typedef	SYSDIALER_PRC	sysdialer_prc ;
typedef	SYSDIALER_MOD	sysdialer_mod ;

EXTERNC_begin

extern int sysdialer_start(sysdialer *,cchar *,mainv,mainv) noex ;
extern int sysdialer_loadin(sysdialer *,cchar *,sysdialer_ent **) noex ;
extern int sysdialer_loadout(sysdialer *,cchar *) noex ;
extern int sysdialer_check(sysdialer *,time_t) noex ;
extern int sysdialer_finish(sysdialer *) noex ;

EXTERNC_end

extern sysdialerms	sysdialerm ;


#endif /* SYSDIALER_INCLUDE */


