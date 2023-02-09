/* Internal commands are called from the the monitor code based on
the first token on the command line. To ease implementing help we shall
adopt the convention that internal commands called with argc = 0 will 
print their usage string and when called with argc = -1 they will print their
help string. 

Each routine should compare it's argv[0] with its own name
and return -1 if they are not the same. The monitor can then
compare the name with external command names and script names.

Don't forget to add entries to initialize_jumptable if you add internal
commands.

*/

#include "macros.h"
#include "internal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* functions implemented in monitor.c that we call here */

extern void release(char *);
extern char *grab(int);
extern void memory(void);
extern char *my_getenv(char *,char **);

int ninternals = 9;

char set_name[] = "set"; 
char set_usage[] = "set [ FOO [BAR] ]";
char set_help[] = "set FOO=BAR, unset FOO if BAR absent, list env if no args";
char mem_name[] = "mem";
char mem_usage[]="mem";
char mem_help[]="print information on memory usage";
char quit_name[] = "quit";
char quit_usage[] = "quit [n]";
char quit_help[] = "exit the monitor [return integer n to caller]";
char echo_name[] = "echo";
char echo_usage[] = "echo arg1 arg2 ...";
char echo_help[] = "prints arguments separated by spaces to stdout";
char help_name[] = "help";
char help_usage[]= "help [cmd]";
char help_help[] = "display available commands [ help on cmd ]";
char read_name[] = "read";
char read_usage[]= "read <var name>";
char read_help[] = "Prompt user for a line. set var name = her response";
char pop_name[] = "pop";
char pop_usage[]= "pop args ...";
char pop_help[] = "run args as command after processing";
char if_name[] = "if";
char if_usage[]= "if FOO BAR args ...";
char if_help[] = "run args as command if and only if FOO=BAR in environment";
char goto_name[] = "goto";
char goto_usage[]= "goto <label>";
char goto_help[] = "branch to line after :label - non-interactive only";

extern int envc;

/* Jump table for internal commands */

ICMD internals[] = {
	NULL, /*a*/
	NULL, /*b*/
	NULL, /*c*/
	NULL, /*d*/
	NULL, /*e*/
	NULL, /*f*/
	NULL, /*g*/
	NULL, /*h*/
	NULL, /*i*/
	NULL, /*j*/
	NULL, /*k*/
	NULL, /*l*/
	NULL,  /*m*/
	NULL, /*n*/
	NULL, /*o*/
	NULL,  /*p*/
	NULL, /*q*/
	NULL, /*r*/
	NULL,  /*s*/
	NULL, /*t*/
	NULL, /*u*/
	NULL, /*v*/
	NULL, /*w*/
	NULL, /*x*/
	NULL, /*y*/
	NULL /*z*/
};

void initialize_jumptable(void){

	internals['e'-'a'] = &Echo;
	internals['g'-'a'] = &Goto;
	internals['h'-'a'] = &Help;
	internals['i'-'a'] = &If;
	internals['m'-'a'] = &Mem;
	internals['p'-'a'] = &Pop;
	internals['q'-'a'] = &Quit;
	internals['r'-'a'] = &Read; 
	internals['s'-'a'] = &Set; 
}


/* Goto: branch to lable */

int Goto(int argc, char **argv, char **envp){

	if(argc == 0){printf("%s\n",goto_usage);return 0;}
	if(argc == -1){printf("%s\n",goto_help);return 0;}
	if(strcmp(goto_name,argv[0])) return -1; 

/* Don't do anything here because this has be handled in monitor */

	return 0;
}


/* If: run args as command if FOO = BAR*/

int If(int argc, char **argv, char **envp){

	if(argc == 0){printf("%s\n",if_usage);return 0;}
	if(argc == -1){printf("%s\n",if_help);return 0;}
	if(strcmp(if_name,argv[0])) return -1; 

/* Don't do anything here because this has be handled in monitor */

	return 0;
}


/* Pop: run args as command after processing */

int Pop(int argc, char **argv, char **envp){

	if(argc == 0){printf("%s\n",pop_usage);return 0;}
	if(argc == -1){printf("%s\n",pop_help);return 0;}
	if(strcmp(pop_name,argv[0])) return -1; 

/* Don't do anything here because this has be handled in monitor */

	return 0;
}

/* List commands and help on individual commends */

extern int n_externals;
extern char *external_names[];
extern char *external_usage[];
extern char *external_help[];

int Help(int argc, char **argv, char **envp){

	int i;
	char c;
	char *p,*q;
	ICMD cmd;

	if(argc == 0){printf("%s\n",help_usage);return 0;}
	if(argc == -1){printf("%s\n",help_help);return 0;}
	if(strcmp(help_name,argv[0])) return -1; 

	if(argc == 1){
		printf("Internal commands\n\n");
		for(i=0;i<26;i++){
			cmd = internals[i];
			if(cmd)cmd(0,argv,NULL);
		}
		printf("---------- more ---------\n");
		getchar();
		printf("\nExternal commands\n\n");
		for(i=0;i<n_externals;i++)
			printf("%s\n",external_usage[i]);
		printf("\nScripts\n\n");
		for(i=0;i<envc;i++){
/* We think a variable whose value starts with # is a script name */
			if(strstr(envp[i],"=#")){
				p = strtok(envp[i],"=");
				printf("%s\n",p);
				*(p+strlen(p)) = '=';
			}
		}
		
	}
	if(argc == 2){

		/* see if it's an external command */

		for(i=0;i<n_externals;i++)
			if(strcmp(argv[1],external_names[i])==0){
				printf("\n%s\n",external_usage[i]);
				printf("%s\n\n",external_help[i]);
				return 0;
			}

		/* see it's a script */
		p = my_getenv(argv[1],envp);
		if(p){
			printf("%s: %s\n",argv[1],p);
			return 0;
		}
		/* so maybe it's an internal command */
		c = argv[1][0];
		if(('a'<=c)&&(c <= 'z')){
			cmd = internals[c-'a'];
			if(cmd){
				printf("\n");
				cmd(0,NULL,NULL);
				cmd(-1,NULL,NULL);
				printf("\n");
				 return 0;
			}
		return 1;
		}
		
	}
	return 1;
}

/* Print args to stdout */

int Echo(int argc, char **argv, char **envp){

	int i;


	if(argc == 0){printf("%s\n",echo_usage);return 0;}
	if(argc == -1){printf("%s\n",echo_help);return 0;}
	if(strcmp(echo_name,argv[0])) return -1; 

	for(i=1;i<argc;i++)
		printf("%s ",argv[i]);
	printf("\n");
	return 0;
}

/* Implement display environment strings or change environment */
/* returns 1 if memory is insufficient, 2 if there are too many
   environment strings. Otherwise returns 0 */ 

int Set(int argc, char **argv, char **envp){
	
	int i;
	char *foo, *bar, *p,*q;

	if(argc == 0){printf("%s\n",set_usage);return 0;}
	if(argc == -1){printf("%s\n",set_help);return 0;}
	if(strcmp(set_name,argv[0])) return -1; 

	switch(argc){

		case 1: /* no args: list environment strings */
			for(i=0;i<envc;i++)
				printf("%s\n",envp[i]);
			return 0;

		case 2: /* set FOO */
		case 3: /* set FOO BAR */

/* code in common to the two cases searches the environment
to see if FOO is already set */

			foo = argv[1];
			if(strchr(foo,'=')){
				fprintf(stderr,"set: No = in variable name\n");
				return 1;
			}
			for(i=0;i<envc;i++){
				p = envp[i];
				q = strchr(p,'='); /* isolate var name */
				*q = '\0';
				if(strcmp(p,foo)==0){
					*q = '='; /* repair */
					break;
				}
				*q = '=';
			}
			if(i == envc)p=NULL; /* not found */				
/* continuing for case 2 */
			if(argc == 2){
				if(p){
					/* remove the entry and close up */
					release(envp[i]);
					while(i<envc-1){
						envp[i]=envp[i+1];	
						i++;
					}
					envp[i]=NULL; /* unused enteries
                                              MUST be kept NULL */
					envc--;
					return 0;
				}
				return 1; 
			}	
/* continuing for case 3: */	
			bar = argv[2];
			/* ask for enough memory */
			q = grab(strlen(foo)+strlen(bar)+2);
			if(!q)return 2; /* Drat */
			sprintf(q,"%s=%s",foo,bar);
			if(!p){ /* new entry */
			  if(envc >= ENV_SIZ)return 3;
			  envp[envc++] = q;
			  return 0;
			}
			/* else change existing entry */
			envp[i] = q;
			release(p);
			return 0;
	}
	return 1;
}

/* Get a line from user and set variable = response */

static char linebuf[BUF_SIZ];

int Read(int argc, char **argv, char **envp){

	if(argc == 0){printf("%s\n",read_usage);return 0;}
	if(argc == -1){printf("%s\n",read_help);return 0;}
	if(strcmp(read_name,argv[0])) return -1; 

	if(argc != 2)return 1;

	printf("?");
	if(!fgets(linebuf,BUF_SIZ,stdin))return 0;
	strtok(linebuf,"\n");
	sprintf(argv[0],"set");
	argv[2]=linebuf;
	Set(3,argv,envp);
	return 0;
}
	

/* Display some information about memory usage for the user to worry about */

int Mem(int argc,char **argv, char **envp){
	
	if(argc == -1){printf("%s\n",mem_help);return 0;}
	if(argc == 0){printf("%s\n",mem_usage);return 0;}
	if(strcmp(mem_name,argv[0])) return -1; 
	memory();
	return 0;
}

/* Quit the program, but we can't return a value to main 
using exit. So this one basically does nothing */

int Quit(int argc, char **argv, char **envp){

	if(argc == -1){printf("%s\n",quit_help);return 0;}
	if(argc == 0){printf("%s\n",quit_usage);return 0;}
	if(strcmp(quit_name,argv[0])) return -1; 
	if(argc == 2) return atoi(argv[1]);
	return 0;
}


#if 0
; Internal.a: implementation of internal monitor commands.  The stack on
; entry to each command looks like so:
;
;    envp    32 bit pointer to array of string pointers
;    argv    32 bit pointer to array of string pointers
;    argc    32 bit integer
;    ret addr  <---- esp
;
;    each command returns 0 if everything ok, 1 if not, in eax

%include "macros.a"

SECTION .data
GLOBAL shift_name,shift_usage,shift_help,exit_name,exit_usage,exit_help,if_name
GLOBAL goto_name,exec_name,exec_usage,exec_help,cd_name,cd_usage,cd_help
if_name: db 'if',0
goto_name: db 'goto',0
shift_name: db 'shift',0
shift_usage: db 'shift [args ...]',10,0
shift_help: db 'run args as new command after processing',10,0
exec_name: db 'exec',0
exec_usage: db 'exec file [args ...]',0
exec_help: db 'runs the executable file under the OS, returns here',0
exit_name: db 'exit',0
exit_usage: db 'exit [n]',10,0
exit_help: db 'exit monitor with return value 0[n]',10,0
cd_name: db 'cd',0
cd_usage: db 'cd path_name',10,0
cd_help: db 'change working directory to path_name',10,0

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; set command                                                         ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


GLOBAL _set,set_name,set_usage,set_help
EXTERN my_strlen,newline,environment,my_strcpy,my_strncmp,mygetenv

set_data: dd 1  ; memory to keep record of number of args
set_mode: dd 1  ; memory to keep record of mode of operation

SECTION .text

_set:

	push ebp       ; set up stack frame
	mov ebp,esp    ;

	push esi       ; store registers used on stack
	push edi
	push ebx
	push ecx
	push edx

	mov eax,[ebp+8]  ;  argc into eax
	cmp eax,3        ;  3 or more args is a usage error
	jae set_bad_ret
	cmp eax,0      ;  0 args: print listing of environment
	je set_zero_args;

	mov ebx, [ebp + 12]           ;save:  ebx has  argv
	mov [set_mode],eax            ;save:  value of argc

	; meaning of other values of argc:
        ; 1  set foo: remove setting for foo
        ; 2  set foo bar: copy foo=bar to data area

	cmp eax,1
	je set_one_arg                ; if only, one arg, skip copy of string

        ; stack up pointer to new env string - needed at end of 2 arg code  

	push DWORD [enddata]          ; it is in data area, at end

	; copy the new environment string, piece by piece, to the end
	; of the data area

	push DWORD [ebx]              ; pointer to string foo (*argv) 
	push DWORD [enddata]          ; where to copy to
	call my_strcpy                ; copy foo
	add esp,8
	dec eax                       ; point at null char at end of string
	add [enddata],eax             ; length of string copied in eax
	mov al,0x3D                   ; ascii code for =
	mov ebx,DWORD [enddata]
	mov BYTE [ebx],al
	inc DWORD [enddata]
	mov ebx, [ebp + 12]
	push DWORD [ebx + 4]         ; pointer to string bar
	push DWORD [enddata]
	call my_strcpy
	add esp,8
	add DWORD [enddata],eax   
		
set_one_arg:                         ; or two args - action here is the same

; search environment to see if foo= is already there

	push DWORD [ebx]  ; pointer to string foo
	call mygetenv     ; See if it is set in environment
	add esp,4         ; clean up from call
	cmp eax,0x00
	je set_search1_done    ; not found, skip remove_loop 
	mov ecx,[envc]         ; set up for remove loop
	imul ecx,4
	sub ecx,esi
	
remove_loop:

	; remove the foo= string from the environment by moving all
	; pointers below it up by one 
        ; BUG: should we also null out the part of data area it points to
        ; for possible re-use ? Over time, crud will build up.

	mov eax, [environment+esi+4] ; next pointer below to eax
	mov [environment+esi],eax    ; and replace current pointer with it
	cmp ecx,0                    ; done when ecx = 0
	je set_done_shift            ; so continue on ...
	add esi,4                    ; else increment index,
	sub ecx,4                      ; decrement counter, and
	jmp remove_loop              ; restart move up pointers loop

set_done_shift:
	dec DWORD [envc]             ; count reduced when env string removed
	                             ; may be increased again below
	sub esi,4                    ; N.B.: esi has been increased one time
                                     ; too many 
set_search1_done:             

	mov eax,[set_mode]           ; remember mode of operation: two args
	cmp eax,2                    ; or one, and branch accordingly
	je set_two_args
	jmp  set_good_ret            ; all done if there was only 1 arg

set_two_args:                        ; else two args 

	pop ebx                      ; recall saved pointer to new string area
	mov [environment + esi], ebx ; and install it
	inc DWORD [envc]
	jmp set_good_ret 
	
; 0 args: print a listing of current environment

set_zero_args:
	mov esi,0     ; initialize counter

set_prenv:
	mov ecx,[environment + esi]   ; point ecx at first env string
	cmp ecx,0                     ; zero pointer marks end of env
	je set_good_ret               ; done with no problems
	push ecx                      ; push str ptr for strlen call 
	call my_strlen                  ; how many bytes to write
	pop ecx                       ; clean up from call
	PRINT_STRING ecx              ; print string with address in ecx
	PRINT_CHAR 0xA                ; move output to new line
	add esi,4                     ; increase counter 
	jmp set_prenv                 ; restart print env loop

set_good_ret:
	mov eax,0                     ; good return value
	jmp set_ret

set_bad_ret:

	PRINT_STRING set_usage        ; usage error message
	PRINT_CHAR 0xA	
	mov eax,1                     ; bad return value
	
set_ret:

	pop edx                       ; restore registers used
	pop ecx
	pop ebx
	pop edi
	pop esi
	pop ebp
	ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; read command                                                        ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

SECTION .data
GLOBAL _read,read_name,read_usage,read_help
EXTERN linebuf1

read_name: db 'read',0
read_usage: db 'read var',10,0
read_help: db 'Prompt user for input and store response in env var',10,0
read_prompt: db 'Enter your response: ',0
read_setargs: times 2 dd 0      ; char **argv for set call

SECTION .text

_read:

	push ebp                    ; set up stack frame
	mov ebp,esp

	PRINT_STRING read_prompt    ; present user with prompt for input 
	READ_LINE linebuf1          ; read her response into buffer

; The response buffer has a newline that we have to get rid of

	mov BYTE [linebuf1 + eax - 1], 0x00 ; kill the newline 
	
	mov eax, [ebp + 8]          ; pointer to name of env var passed in
	mov [read_setargs],eax 
	mov eax, linebuf1           ; pointer to value of env var
	mov [read_setargs+4],eax    ; finish building argv for set
	push read_setargs           ; push char **argv for set
	push DWORD 2                ; push argc (=2) for set call 
	call _set                   ; set var = response
	add esp,8                   ; clean up from set call
	pop ebp                     ; restore registers used
	ret 

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; help command                                                        ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

SECTION .data

GLOBAL _help,help_name,help_usage,help_help
EXTERN my_strcmp,my_strstr,n_externals,external_names,external_usage,external_help

help_name: db 'help',0
help_usage: db 'help [ command ]',10,0
help_help: db 'print cmd list or help for command',10,0
help_msg0: db `Available factory commands are:\n\n`,0
help_msg1: db `\nAvailable custom (external) commands\n\n`,0
help_msg2: db `\n\nAvailable scripts:\n\n`,0
help_ptr: dd 0
equalpound: db 0x3D,COMMENT_CHAR,0  ; the sequence =# that identifies scripts

SECTION .text

_help:

	push ebp       ; set up stack frame
	mov ebp,esp    ;

	push esi       ; store registers used on stack
	push ebx
	push ecx
	push edx

	mov eax,[ebp+8] ;  argc into eax
	cmp eax,2      ;  2 or more args is a usage error
	jae help_bad_ret
	cmp eax,0      ;   
	je help_no_args

; 1 arg: print detailed help for command
	
	mov ebx, [ebp + 12]   ; argv in ebx
	push DWORD [ebx]      ; push *argv

	push set_name
	call my_strcmp
	add esp,4
	cmp eax,0
	jne is_it_shift
	PRINT_STRING set_usage
	PRINT_STRING set_help
	jmp help_cleanup

is_it_shift:

	push shift_name
	call my_strcmp
	add esp,4
	cmp eax,0
	jne is_it_cd
	PRINT_STRING shift_usage
	PRINT_STRING shift_help
	jmp help_cleanup

is_it_cd:

	push cd_name
	call my_strcmp
	add esp,4
	cmp eax,0
	jne is_it_exec
	PRINT_STRING cd_usage
	PRINT_STRING cd_help
	jmp help_cleanup


is_it_exec:

	push exec_name
	call my_strcmp
	add esp,4
	cmp eax,0
	jne is_it_read
	PRINT_STRING exec_usage
	PRINT_STRING exec_help
	jmp help_cleanup

is_it_read:

	push read_name
	call my_strcmp
	add esp,4
	cmp eax,0
	jne is_it_help
	PRINT_STRING read_usage
	PRINT_STRING read_help
	jmp help_cleanup

is_it_help:

	push help_name
	call my_strcmp
	add esp,4
	cmp eax,0
	jne is_it_echo
	PRINT_STRING help_usage
	PRINT_STRING help_help
	jmp help_cleanup

is_it_echo:

	push echo_name
	call my_strcmp
	add esp,4
	cmp eax,0
	jne is_it_status
	PRINT_STRING echo_usage
	PRINT_STRING echo_help
	jmp help_cleanup

is_it_status:

	push status_name
	call my_strcmp
	add esp,4
	cmp eax,0
	jne is_it_exit
	PRINT_STRING status_usage
	PRINT_STRING status_help
	jmp help_cleanup

is_it_exit:

	push exit_name
	call my_strcmp
	add esp,4
	cmp eax,0
	jne is_it_external
	PRINT_STRING exit_usage
	PRINT_STRING exit_help
	jmp help_cleanup

is_it_external:
	
	mov ecx,[n_externals] 
	xor esi,esi

help_external_search:

	cmp ecx,0
	jbe is_it_a_script
	push DWORD [external_names + esi]
	call my_strcmp
	add esp,4
	cmp eax,0 
	je help_found_external
	add esi,4
	dec ecx
 	jmp help_external_search

help_found_external:

	PRINT_STRING [external_usage + esi]
	PRINT_STRING [external_help + esi]
	jmp help_cleanup

is_it_a_script:

	call mygetenv
	cmp eax,0x00
	je help_not_found
	mov [help_ptr],eax
	PRINT_STRING [help_ptr]
	jmp help_cleanup
	
	; clean stack and print all commands since this arg not found

help_not_found:
	add esp,4
	jmp help_no_args

help_cleanup:
	add esp,4      ; pop pushed arg
	jmp help_good_ret

help_no_args:

; 0 args: print a listing of commands
; Print in columns depending on the value of HELP_COLS
; Has to be set up by hand for these internal commands
	
	PRINT_STRING  help_msg0
	PRINT_STRING  set_name
	PRINT_CHAR 0x9         ; TAB
	PRINT_STRING  help_name
	PRINT_CHAR 0x9
	PRINT_STRING  echo_name
	PRINT_CHAR 0xA	
	PRINT_STRING  status_name
	PRINT_CHAR 0x9	
	PRINT_STRING  shift_name
	PRINT_CHAR 0x9	
	PRINT_STRING exit_name
	PRINT_CHAR 0xA	
	PRINT_STRING read_name
	PRINT_CHAR 0x9	
	PRINT_STRING exec_name
	PRINT_CHAR 0x9	
	PRINT_STRING cd_name
	PRINT_CHAR 0xA	

; loop over external commands, printing their names as well

	PRINT_STRING  help_msg1  ; announce there are externals

	mov ecx,[n_externals]
	xor esi,esi
	xor bx,bx                ; reset column counter
	inc bx
	mov dl,HELP_COLS         ; divisor for column-ation

help_list_externals:

	cmp ecx,0
	je help_list_scripts
	PRINT_STRING [external_names + esi]	

	mov ax,bx                 ; decide whether to print newline or tab
	div dl
	cmp ah,00H
	je ext_print_newline
	PRINT_CHAR 0x9	
	jmp ext_do_not_print_newline
ext_print_newline:
	PRINT_CHAR 0xA
ext_do_not_print_newline:

	add esi,4
	dec ecx
	inc bx
	cmp ecx,0
	jmp help_list_externals

; loop over all scripts. We believe a script is the value of any environment
; variable that starts with #

help_list_scripts:

	PRINT_STRING  help_msg2 ; say we we are now listing scripts
	xor bx,bx               ; reset column counter
	inc bx

	mov ecx,[envc]    ; loop over all env variables
	xor esi,esi       ; initialize env pointer offset

help_list_scripts_loop:

	cmp ecx,0         ; if no env variable remain to be examined
	je help_good_ret  ; then begin the return process
	mov eax,[environment + esi]  ; else store address of current env string
	mov [help_ptr],eax           ; in a local holding area 
	push equalpound  ; determine whether the =# sequence occurs inside
	push DWORD [help_ptr] ; current env string. If so, we deem it the start
	call my_strstr    ; of a script. (BUG: it might not be.)
	add esp,8         ; clean up from call
	cmp eax,0x00      ; if strstr returned null then this var not a script
	je scripts_loop_continue ; so go on to the next env var
; else print out the name part of the current env var as the name of a script
	mov BYTE [eax], 0x00 ; termporarily null terminate the name part of var
	push eax          ; save address of null byte for later restore
	PRINT_STRING [help_ptr]

; print tab unless bx divisible by number of columns to display
	mov ax,bx         ; prepare to divide
	div dl            ; divide column counter by HELP_COLS 
	cmp ah,00H        ; was it divisible?
	je script_print_newline ; new line if so
	PRINT_CHAR 0x9	  ; else tab over one
	jmp script_do_not_print_newline
script_print_newline:
	PRINT_CHAR 0xA
script_do_not_print_newline:
	inc bx            ; increment column counter

	pop eax           ; restored saved address of byte where = removed
	mov BYTE [eax],0x3D  ; put = sign back in env string

scripts_loop_continue:
	
	dec ecx
	add esi,4
	jmp help_list_scripts_loop

help_good_ret:
	mov eax,0                     ; good return value
	jmp help_ret

help_bad_ret:
	PRINT_STRING help_usage        ; usage error message
	PRINT_CHAR 0xA	
	mov eax,1                     ; bad return value
	
help_ret:

	pop edx                       ; restore registers used
	pop ecx
	pop ebx
	pop esi
	pop ebp
	ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; echo command                                                        ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

SECTION .data

GLOBAL _echo,echo_name,echo_usage,echo_help

echo_name: db 'echo',0
echo_usage: db 'echo [ anything ]',10,0
echo_help: db 'print tail of command line',10,0
echo_arg: dd 0

SECTION .text

_echo:

	push ebp       ; set up stack frame
	mov ebp,esp    ;
	push esi       ; save registers used
	push ebx
	push ecx


;  print everthing that second arg points to 

	mov ecx,[ebp + 8] ; load argc in ecx = number of words to echo
	mov ebx,[ebp + 12]; load argv in ebx
	xor esi,esi       ; initialize index to args

echo_print_loop:
	cmp ecx,0x00      ; Any more args to echo ?
	je  echo_ret      ; No, finish up and return
	mov eax,[ebx+esi]
	mov [echo_arg],eax
	PRINT_STRING [echo_arg] ; print the arg
	PRINT_CHAR 0x20
	add esi,4         ; point at next arg
	dec ecx           ; decrement counter
	jmp echo_print_loop

echo_ret:

	PRINT_CHAR 0xA	
	mov eax,0                     ; good return value
	pop ecx                       ; restore registers used
	pop ebx
	pop esi
	pop ebp
	ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; status command                                                       ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

SECTION .data

GLOBAL _status,status_name,status_usage,status_help
EXTERN _print_i,envc,enddata,data

status_name: db 'status',0
status_usage: db 'status',10,0
status_help: db 'Print data on current program state',10,0
status_str0: db 10,'Break address =  ',0
status_str2: db 10,'Number of Environment Strings = ',0 
status_str3: db 10,'Dynamic Memory Used (bytes) = ',0

SECTION .text

_status: 

; The break address is the top of memory available to the program for
; dynamic memory allocation. We put all new environment strings in this
; area. Since we never bother to reclaim any of this memory it will get
; used up over time. Obtaining the break address is OS dependent
; and we implement it in SBRK in macros.a

	PRINT_STRING status_str0             	
	SBRK                       ; return break address in eax
	push eax
	call _print_i              ; print break address to screen
	pop eax                    ; clean up from call 
	PRINT_STRING status_str2
	mov eax, [envc]            ; number of environment strings
	push eax
	call _print_i              ; print it and
	pop eax                    ; and clean up from call
	PRINT_STRING status_str3
	mov eax, [enddata]         ; end of data we have written to data
	sub eax,data               ; subtract from start of data area
	push eax                   ; and print the difference = bytes
	call _print_i              ; of dynamic memory used
	pop eax                    ; clean up from call
	ret
	
#endif
