
namespace configvars_obj {

    int configvars_parse(CV *cvp,int fi,vecitem *eep) noex {
	USTAT		sb ;
	CV_FILE	*fep ;
	CV_FILE	ve ;
	FIELD		fsb ;
	bfile		file, *fp = &file ;
	cint		llen = LINEBUFLEN ;
	int	rs = SR_OK ;
	int	i ;
	int	c, type ;
	int	sl, cl, len, line = 0 ;
	int	klen, vlen ;

	char	lbuf[LINEBUFLEN + 1] ;
	char	keybuf[KEYBUFLEN + 1], *bp ;
	char	*key, *value ;
	char	*cp ;

/* get the pointer to our own file structure */

	if ((rs = vecitem_get(cvp->fesp,fi,&fep)) < 0)
	    return rs ;

	if (fep == nullptr)
	    return SR_NOTFOUND ;

	if ((rs = bopen(fp,fep->filename,"r",0664)) < 0)
	    return rs ;

	if ((rs = bcontrol(fp,BC_STAT,&sb)) < 0)
	    goto done ;

/* have we already parsed this one? */

	rs = SR_OK ;
	if (fep->mtime >= sb.st_mtime)
	    goto done ;

	fep->mtime = sb.st_mtime ;

/* start processing the configuration file */

	while ((rs = breadlns(fp,lbuf,llen,-1,&line)) > 0) {
	    len = rs ;

	    if (len == 1) continue ;	/* blank line */

	    if (lbuf[len - 1] != '\n') {

#ifdef	COMMENT
	        f_trunc = TRUE ;
#endif
	        while ((c = bgetc(fp)) >= 0) {

	            if (c == '\n') 
			break ;

		}

	        continue ;

	    } else
		len -= 1 ;

/* pre-process this a little bit */

	    cp = lbuf ;
	    cl = len ;
	    while ((cl > 0) && CHAR_ISWHITE(*cp)) {
		cp += 1 ;
		cl -= 1 ;
	    }

	    if ((cl == 0) || (cp[0] == '#'))
		continue ;

/* do this line */

	    field_get(&fsb,fterms) ;

/* empty or comment only line */

	    if (fsb.flen <= 0) continue ;

/* convert key to lower case */

	    cl = MIN(fsb.flen,KEYBUFLEN) ;
	    strwcpylc(keybuf,fsb.fp,cl) ;

/* check if key is a built-in one */

	    i = matostr(configkeys,2,keybuf,cl) ;

	    if (i >= 0) {
	        type = -1 ;
	        switch (i) {

/* unsets */
	        case configkey_unset:
		    type = vartype_unset ;
	            field_get(&fsb,fterms) ;

	            if (fsb.flen > 0) {

	                type = 3 ;
	                rs = file_addvar(fep,type,fi,fsb.fp,fsb.flen,nullptr,0) ;

	                if (rs < 0)
	                    goto badalloc ;

	            }

	            break ;

/* export environment */
	        case configkey_set:
			if (type < 0)
				type = vartype_set ;

/* FALLTHROUGH */

	        case configkey_define:
			if (type < 0)
				type = vartype_define ;

/* FALLTHROUGH */

	        case configkey_export:
			if (type < 0)
				type = vartype_export ;

/* get first part */

	            field_get(&fsb,fterms) ;
	            if (fsb.flen <= 0) {
	                rs = SR_INVALID ;
	                if (eep != nullptr)
	                    badline(eep,fep->filename,line) ;

	                break ;
	            }

	            key = fsb.fp ;
	            klen = fsb.flen ;

/* get second part */

	            field_get(&fsb,fterms) ;

	            value = (fsb.flen >= 0) ? fsb.fp : nullptr ;
	            vlen = fsb.flen ;

	            if (i == configkey_set) {
	                rs = configvars_addvar(cvp,fi,CONFIGVARS_WSETS,
	                    key,klen,value,vlen) ;

	            } else {
	                rs = file_addvar(fep,type,fi,key,klen,value,vlen) ;
		    }

	            if (rs < 0)
	                goto badalloc ;

	            break ;

	        default:
	            rs = SR_NOTSUP ;
	            goto badprogram ;

	        } /* end switch */

	    } else {
		int	alen ;
		char	abuf[LINEBUFLEN + 1] ;

/* unknown keyword, it is just another variable ! */

	        key = fsb.fp ;
	        klen = fsb.flen ;

/* store the key along with the remainder of this "line" */

	        rs = configvars_addvar(cvp,fi,CONFIGVARS_WVARS,
	            key,klen,fsb.lp,fsb.rlen) ;

	        if (rs < 0)
	            goto badalloc ;

	    } /* end if */

	} /* end while (reading lines) */

/* we're out of here one way or another! */
badprogram:
badalloc:
badconfig:

/* done with configuration file processing */
done:

	bclose(fp) ;

	return rs ;
}
/* end subroutine (configvars_parse) */

} /* end namespace (configvars_obj) */


