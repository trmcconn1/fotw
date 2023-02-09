/* macro.c: routines to support the .define directive (handled in parse.y) 
  and macro expansion */

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "macro.h"


#define MAX_MACROS 512
struct macro_struct *macros[MAX_MACROS];
static char expand_buf[4*MAX_MACRO_TEXT];

int nmacros=0;

/* Add the macro passed to the global array of macros */

struct macro_struct *addmacro(struct macro_struct *amacro)
{
	if(!amacro)return NULL;
	macros[nmacros++]=amacro;
	return amacro;
}

/* Removes a macro from the array. Moves the rest downward so that
 * no gaps appear in the array. Returns 0 if operation was successful,
 * 0 otherwise. */

int rmmacro(char *name){

	int i=0,k;

	while(i < nmacros){
		if(strcmp(name,macros[i]->name)==0)
			break;
		i++;
	}

	/* if i < nmacros, then it is index of macro to be removed */
	if(i < nmacros){
		free(macros[i]->name);
		free(macros[i]->value);
		for(k=0;k<macros[i]->argc;k++)free(macros[i]->argv[k]);
		free(macros[i]->argv);
		free(macros[i]);
		for(k=i;k<nmacros-1;k++)
			macros[k]=macros[k+1];
		nmacros--;
		return 0;
	}
	return 1;
}
		

/* Return pointer to macro with given name, or NULL if it does not exist.
 * Obviously, this linear search will really slow things down, but
 * macros are indentifiable by a leading ., so the parser only needs
 * to call this on such tokens. */

struct macro_struct *getmacro(char *name){
	
	int i=0;
	char buf[MAX_MACRO_NAME],*p;

	strcpy(buf,name);
	p = strchr(buf,'(');
	if(p)*p='\0';

	while(i < nmacros)
		if(strcmp(buf,macros[i]->name)==0)break;
		else i++;

	/* if i < nmacros, then it is index of macro to be removed */
	if(i < nmacros)return macros[i];
	return NULL;
}

/* Return pointer to string that represents the value of the macro text
   to be substituted (in lex.l) for the macro name. This gets tricky when
   the macro has arguments */

char *expand_macro(struct macro_struct *amac,char *call_name){

	char *p,*q,*r,buf[MAX_MACRO_NAME];
	char *arg_vals[MAX_MACRO_ARGS];
	int n=0,m=0,i,j;

	/* search the call name for '(' */
	p = strchr(call_name,'(');
	if(p){ /* extract strings to substitute for args */
		q=strtok(p+1,",)");
		if(q)arg_vals[n++]=q;
		while((q=strtok(NULL,",)")))
			arg_vals[n++]=q;
	}

	if(n!=amac->argc)return NULL;

	r = amac->value;

	while(r<amac->value+strlen(amac->value)){
		p = amac->value+strlen(amac->value);
		j=-1;
		/* find next arg name in macro text */
		for(i=0;i<amac->argc;i++){ 
			q = strstr(r,amac->argv[i]);
			if(q &&(q<p)){p=q;j=i;}
		}
		/* p now points at next arg name to be expanded and j is
		 its index */
		/* Copy next unexpanded block */
		for(i=0;i<p-r;i++)
			expand_buf[m++]=r[i];
		expand_buf[m]='\0';

		if(j>=0){
			strcat(expand_buf,arg_vals[j]);
			m += strlen(arg_vals[j]);
			r = p + strlen(amac->argv[j]); 
		}
		else r = p;
	}
	return (char *)strdup(expand_buf);
}

/* Display information about currently defined macros */

void dump_macros(){

	int i,j;

	printf("%d macros currently defined:\n",nmacros);
	for(i=0;i<nmacros;i++){
			printf("%s",macros[i]->name);
			if(macros[i]->argc){
				printf("(%s",macros[i]->argv[0]);
				for(j=1;j<macros[i]->argc;j++)
					printf(",%s",macros[i]->argv[j]);
				printf(")");
			}
			printf("=%s\n",macros[i]->value);
	}
}

