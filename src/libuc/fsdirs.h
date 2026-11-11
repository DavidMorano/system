/* fsdirs HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* traverse directory tree */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	FSDIRS_INCLUDE
#define	FSDIRS_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<fsdir.h>		/* LIBUC */
#include	<fifostr.h>		/* LIBUC */
#include	<hdb.h>			/* LIBUC */


#define	FSDIRS			struct fsdirs_head
#define	FSDIRS_FL		struct fsdirs_flags
#define	FSDIRS_MAGIC		0x98653217

/* options */
enum fsdirsos {
    fsdirso_pipe,
    fsdirso_chr,
    fsdirso_dir,
    fsdirso_name,
    fsdirso_blk,
    fsdirso_reg,
    fsdirso_lnk,
    fsdirso_sock,
    fsdirso_door,
    fsdirso_wht,
    fsdirso_follow,
    fsdirso_noent,
    fsdirso_uniqfile,
    fsdirso_uniqdir,
    fsdirso_igndotfile,
    fsdirso_igndotdir,
    fsdirso_overlast
} ; /* end enum (fsdirsos) */

/* masks */
#ifdef	__cplusplus
struct fsdirsms {
    static constexpr int	pipe		= (1 << fsdirso_pipe) ;
    static constexpr int	chr		= (1 << fsdirso_chr) ;
    static constexpr int	dir		= (1 << fsdirso_dir) ;
    static constexpr int	name		= (1 << fsdirso_name) ;
    static constexpr int	blk		= (1 << fsdirso_blk) ;
    static constexpr int	reg		= (1 << fsdirso_reg) ;
    static constexpr int	lnk		= (1 << fsdirso_lnk) ;
    static constexpr int	sock		= (1 << fsdirso_sock) ;
    static constexpr int	door		= (1 << fsdirso_door) ;
    static constexpr int	wht		= (1 << fsdirso_wht) ;
    static constexpr int	follow		= (1 << fsdirso_follow) ;
    static constexpr int	noent		= (1 << fsdirso_noent) ;
    static constexpr int	uniqfile	= (1 << fsdirso_uniqfile) ;
    static constexpr int	uniqdir		= (1 << fsdirso_uniqdir) ;
    static constexpr int	igndotfile	= (1 << fsdirso_igndotfile) ;
    static constexpr int	igndotdir	= (1 << fsdirso_igndotdir) ;
} ; /* end struct (fsdirsms) */
#endif /* __cplusplus */

/* options */
#define	FSDIRS_MPIPE		(1<<	    fsdirso_pipe)
#define	FSDIRS_MCHR		(1<<	    fsdirso_chr)
#define	FSDIRS_MDIR		(1<<	    fsdirso_dir)
#define	FSDIRS_MNAME		(1<<	    fsdirso_name)
#define	FSDIRS_MBLK		(1<<	    fsdirso_blk)
#define	FSDIRS_MREG		(1<<	    fsdirso_reg)
#define	FSDIRS_MLNK		(1<<	    fsdirso_lnk)
#define	FSDIRS_MSOCK		(1<<	    fsdirso_sock)
#define	FSDIRS_MDOOR		(1<<	    fsdirso_door)
#define	FSDIRS_MWHT		(1<<	    fsdirso_wht)
#define	FSDIRS_MFOLLOW		(1<<	    fsdirso_follow)
#define	FSDIRS_MNOENT		(1<<	    fsdirso_noent)
#define	FSDIRS_MUNIQFILE	(1<<	    fsdirso_uniqfile)
#define	FSDIRS_MUNIQDIR		(1<<	    fsdirso_uniqdir)
#define	FSDIRS_MIGNDOTFILE	(1<<	    fsdirso_igndotfile)
#define	FSDIRS_MIGNDOTDIR	(1<<	    fsdirso_igndotdir)

#define	FSDIRS_MFIFO		FSDIRS_MPIPE
#define	FSDIRS_MCHAR		FSDIRS_MCHR
#define	FSDIRS_MBLOCK		FSDIRS_MBLK
#define	FSDIRS_MLINK		FSDIRS_MLNK

struct fsdirs_flags {
	uint		feof:1 ;
	uint		dir:1 ;
	uint		dirids:1 ;	/* dir-id tracking */
} ; /* end struct (fsdirs_flags) */

struct fsdirs_head {
	cchar		**prunearr ;
	char		*bnbuf ;
	char		*nbuf ;		/* "name" buffer */
	char		*lbuf ;		/* "link" buffer */
	void		*bsp ;		/* Bit-Set-Pointer */
	fifostr		*dqp ;		/* directory-queue-pointer */
	fsdir		*dirp ;		/* directory-pointer */
	hdb		*dip ;		/* directory-id-pointer */
	FSDIRS_FL	fl ;
	uint		magval ;
	int		opts ;
	int		bndlen ;
	int		cdnlen ;
	int		bnlen ;
	int		nlen ;		/* "name" length */
	int		llen ;		/* "link" length */
	ushort		selset ;	/* selection-set of DTs */
} ; /* end struct (fsdirs_head) */

#ifdef	__cplusplus
enum fsdirsmems {
	fsdirsmem_close,
	fsdirsmem_overlast
} ; /* end enum (fsdirsmems) */
struct fsdirs ;
struct fsdirs_co {
	fsdirs		*op = nullptr ;
	int		w = -1 ;
	void operator () (fsdirs *p,int m) noex {
	    op = p ;
	    w = m ;
	} ; /* end */
	operator int () noex ;
	int operator () () noex { 
	    return operator int () ;
	} ; /* end */
} ; /* end struct (fsdirs_co) */
struct fsdirs : fsdirs_head {
	fsdirs_co	close ;
	fsdirs() noex {
	    close	(this,fsdirsmem_close) ;
	    magval	= 0 ;
	} ; /* end ctor */
	fsdirs(const fsdirs &) = delete ;
	fsdirs &operator = (const fsdirs &) = delete ;
	int open	(cchar *,int = 0) noex ;
	int prune	(cchar **) noex ;
	int read	(ustat *,char *,int) noex ;
	void dtor	() noex ;
	destruct fsdirs() {
	    if (magval) dtor() ;
	} ; /* end */
} ; /* end struct (fsdirs) */
#else	/* __cplusplus */
typedef FSDIRS		fsdirs ;
#endif /* __cplusplus */

typedef	FSDIRS_FL	fsdirs_fl ;

EXTERNC_begin
extern int fsdirs_open	(fsdirs *,cchar *,int) noex ;
extern int fsdirs_prune	(fsdirs *,cchar **) noex ;
extern int fsdirs_read	(fsdirs *,ustat *,char *,int) noex ;
extern int fsdirs_close	(fsdirs *) noex ;
EXTERNC_end

#ifdef	__cplusplus
extern const fsdirsms	fsdirsm ;
#endif /* __cplusplus */


#endif /* FSDIRS_INCLUDE */


