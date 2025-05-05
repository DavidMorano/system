/* fsdirtree HEADER */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* traverse directory tree */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	FSDIRTREE_INCLUDE
#define	FSDIRTREE_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<usystem.h>
#include	<fsdir.h>
#include	<fifostr.h>
#include	<hdb.h>


#define	FSDIRTREE		struct fsdirtree_head
#define	FSDIRTREE_FL		struct fsdirtree_flags
#define	FSDIRTREE_MAGIC		0x98653217
/* options */
enum fsdirtreeos {
    fsdirtreeo_follow,
    fsdirtreeo_lnk,
    fsdirtreeo_reg,
    fsdirtreeo_blk,
    fsdirtreeo_chr,
    fsdirtreeo_pipe,
    fsdirtreeo_sock,
    fsdirtreeo_dir,
    fsdirtreeo_noent,
    fsdirtreeo_uniqfile,
    fsdirtreeo_uniqdir,
    fsdirtreeo_igndotfile,
    fsdirtreeo_igndotdir,
    fsdirtreeo_overlast
} ; /* end enum (fsdirtreeos) */
/* masks */
struct fsdirtreems {
    static int	follow ;
    static int	lnk ;
    static int	reg ;
    static int	blk ;
    static int	chr ;
    static int	pipe ;
    static int	sock ;
    static int	dir ;
    static int	noent ;
    static int	uniqfile ;
    static int	uniqdir ;
    static int	igndotfile ;
    static int	igndotdir ;
} ; /* end struct (fsdirtreems) */
/* options */
#define	FSDIRTREE_MFOLLOW	(1<<	    fsdirtreeo_follow)
#define	FSDIRTREE_MLINK		(1<<	    fsdirtreeo_lnk)
#define	FSDIRTREE_MREG		(1<<	    fsdirtreeo_reg)
#define	FSDIRTREE_MBLOCK	(1<<	    fsdirtreeo_blk)
#define	FSDIRTREE_MCHAR		(1<<	    fsdirtreeo_chr)
#define	FSDIRTREE_MPIPE		(1<<	    fsdirtreeo_pipe)
#define	FSDIRTREE_MSOCK		(1<<	    fsdirtreeo_sock)
#define	FSDIRTREE_MDIR		(1<<	    fsdirtreeo_dir)
#define	FSDIRTREE_MNOENT	(1<<	    fsdirtreeo_noent)
#define	FSDIRTREE_MUNIQFILE	(1<<	    fsdirtreeo_uniqfile)
#define	FSDIRTREE_MUNIQDIR	(1<<	    fsdirtreeo_uniqdir)
#define	FSDIRTREE_MIGNDOTFILE	(1<<	    fsdirtreeo_igndotfile)
#define	FSDIRTREE_MIGNDOTDIR	(1<<	    fsdirtreeo_igndotdir)


struct fsdirtree_flags {
	uint		feof:1 ;
	uint		dir:1 ;
	uint		dirids:1 ;	/* dir-id tracking */
} ;

struct fsdirtree_head {
	cchar		**prune ;
	char		*bnbuf ;
	char		*nbuf ;
	char		*lbuf ;
	void		*bsp ;		/* Bit-Set-Pointer */
	fifostr		*dqp ;		/* directory-queue-pointer */
	fsdir		*dirp ;		/* directory-pointer */
	hdb		*dip ;		/* directory-id-pointer */
	FSDIRTREE_FL	fl ;
	uint		magic ;
	int		opts ;
	int		bndlen ;
	int		cdnlen ;
	int		bnlen ;
	int		nlen ;
	int		llen ;
} ;

typedef FSDIRTREE	fsdirtree ;
typedef	FSDIRTREE_FL	fsdirtree_fl ;

EXTERNC_begin

extern int fsdirtree_open(fsdirtree *,cchar *,int) noex ;
extern int fsdirtree_prune(fsdirtree *,cchar **) noex ;
extern int fsdirtree_read(fsdirtree *,USTAT *,char *,int) noex ;
extern int fsdirtree_close(fsdirtree *) noex ;

EXTERNC_end


#endif /* FSDIRTREE_INCLUDE */


