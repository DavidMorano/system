/* testuserattr (C89) */

#include	<envstandards.h>

#include	<sys/types.h>
#include	<user_attr.h>
#include	<cstdio>

#include	<usystem.h>
#include	<localmisc.h>

int main()
{
	userattr_t	*uap ;

	int	rs ;

	const char	*un = "junker" ;

	rs = uc_getusernam(un,&uap) ;

	fprintf(stderr,"main: uc_getusernam() rs=%d\n",rs) ;

	return 0 ;
}

