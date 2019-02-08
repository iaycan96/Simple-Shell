# Simple-Shell
I make a simple shell using in C using execl() function.
It will take the command as input and will execute that in a new process. When program
takes the command, it will create a new process using fork() system call, and the new
process (child) will execute the program. The child will use the execl() function 
to execute a new program.

 My shell support
*	Simple shell commands(ex: ls, ps, sort etc.)
*	Background processes  
ex:myshell:gedit &  

## Built-in commands
*	alias/unalias command()  
set/remove an alias for a command.   
Using -l (lowercase letter L) lists all the aliases added to myshell.   
  
  
ex:
myshell:alias "ls -l" list  
myshell:alias "ps -a" proc  
myshell:alias -l  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;	list "ls -l"  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;	proc "ps -a"  
myshell:proc  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;	PID TTY TIME CMD  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;	6052 pts/0 00:00:00 ps  
myshell:unalias list  
myshell:alias -l  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;	proc "ps -a"  
	
*	clr :clears the screen
*	^Z: (<CONTROL><Z>):It terminats running foreground process.
*	^D: (<CONTROL><D>):Exits the program.
*	fg: moves all background process to foreground
*	exit: if there is no background process,terminates the shell.If there is, waits until all background processes terminates.

## Redirection	
*	myshell: myprog [args] > file.out  
Writes the standard output of myprog  to the file file.out.  
file.out is created if it does not exist and truncated if it does.  
  
*	myshell: myprog [args] >> file.out   
Appends the standard output of myprog  to the file file.out.    
file.out is created if it does not exist and appended to if it does.  

*	myshell: myprog [args] < file.in  
Uses the contents of the file file.in as the standard input to program myprog.  
 
*	myshell:  myprog [args] 2> file.out   
Writes the standard error of myprog to the file file.out.  

*	myshell: myprog [args] < file.in > file.out  
Executes the command myprog which will read input from file.in  and stdout of
the command is directed to the file file.out

