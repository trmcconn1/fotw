/* strdup: not always in stdlib. */

char *strdup( char *s)
{
        char *p;

        p = (char *) malloc(strlen(s)+1);
        if(p != (char *)0)
                strcpy(p,s);
        return p;
}


