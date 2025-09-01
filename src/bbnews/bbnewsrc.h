/* bbnewsrc HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */


#ifndef	BBNEWSRC_INCLUDE
#define	BBNEWSRC_INCLUDE


#include	<envstandards.h>	/* must be ordered first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<bfile.h>
#include	<dater.h>


#define	BBNEWSRC	struct bbnewsrc_head
#define	BBNEWSRC_ENT	struct bbnewsrc_ent
#define	BBNEWSRC_FL	struct bbnewsrc_flags


struct bbnewsrc_flags {
	uint		readtime:1 ;
	uint		wroteheader:1 ;
	uint		fileopen:1 ;
} ;

struct bbnewsrc_head {
	cchar		*ufname ;
	bfile		nf ;
	dater		tmpdate ;
	BBNEWSRC_FL	fl ;
	uint		magic ;
	int		line ;
} ;

struct bbnewsrc_ent {
	time_t		mtime ;
	uint		f_subscribed:1 ;
	int		ln ;		/* line-number */
	char		ngname[MAXPATHLEN+1] ;
} ;

EXTERNC_end

extern int	bbnewsrc_open(BBNEWSRC *,cchar *,int) noex ;
extern int	bbnewsrc_close(BBNEWSRC *) noex ;
extern int	bbnewsrc_rewind(BBNEWSRC *) noex ;
extern int	bbnewsrc_read(BBNEWSRC *,BBNEWSRC_ENT *) noex ;
extern int	bbnewsrc_write(BBNEWSRC *,cchar *,int,time_t) noex ;

EXTERNC_end


#endif /* BBNEWSRC_INCLUDE */


