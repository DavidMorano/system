/* fsdir HEADER (SunOS) */
/* lang=C20 */

/* UNIX® file system dirextory operations */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Traverse a directory and read out the entries.

*******************************************************************************/

#ifndef	FSDIR_INCLUDE
#define	FSDIR_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* system types |ino_t| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>


#define	FSDIR_MAGIC	0x31415926
#define	FSDIR		struct fsdir_head
#define	FSDIR_ENT	struct fsdir_entry
#define	FSDIR_FL	struct fsdir_flags


struct fsdir_entry {
	ino_t		ino ;		/* UNIX® "inode number" of entry */
	char		*name ;		/* name of file */
	ushort		nlen ;		/* length of 'name' c-string */
	ushort		type ;		/* only some systems support this */
} ;

struct fsdir_flags {
	uint		descname:1 ;	/* name was really a descriptor */
} ;

struct fsdir_head {
	uint		magic ;
	FSDIR_FL	f ;
	char		*bdata ;	/* buffer pointer (fixed) */
	int		bsize ;		/* buffer size (fixed) */
	int		blen ;		/* buffer length read */
	int		doff ;		/* we do not handle "huge" dirs! */
	int		eoff ;		/* entry offset with directory */
	int		dfd ;		/* file-descriptor */
	int		ei ;		/* entry index (into buffer) */
} ;

typedef FSDIR		fsdir ;
typedef FSDIR_ENT	fsdir_ent ;

#ifdef	__cplusplus
extern "C" {
#endif

extern int	fsdir_open(fsdir *,cchar *) noex ;
extern int	fsdir_read(fsdir *,fsdir_ent *,char *,int) noex ;
extern int	fsdir_tell(fsdir *,off_t *) noex ;
extern int	fsdir_seek(fsdir *,off_t) noex ;
extern int	fsdir_rewind(fsdir *) noex ;
extern int	fsdir_audit(fsdir *) noex ;
extern int	fsdir_close(fsdir *) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* FSDIR_INCLUDE */


