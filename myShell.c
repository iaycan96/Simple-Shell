#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
 
#define MAX_LINE 80 /* 80 chars per line, per command, should be enough. */
 
/* The setup function below will not return any value, but it will just: read
in the next command line; separate it into distinct arguments (using blanks as
delimiters), and set the args array entries to point to the beginning of what
will become null-terminated, C-style strings. */




int count_command;

int getlocation(char *path[20],char*command);
int redirectCheck(char*args[]);
int appendCheck(char* args[]);
int stderrCheck(char*args[]);
int revredirect(char*args[]);
int inoutredirectCheck(char*args[]);
void redirect(char * args[],char*path[20]);




void setup(char inputBuffer[], char *args[],int *background)
{
    int length, /* # of characters in the command line */
        i,      /* loop index for accessing inputBuffer array */
        start,  /* index where beginning of next command parameter is */
        ct;     /* index of where to place the next parameter into args[] */
    
    ct = 0;
        
    /* read what the user enters on the command line */
    length = read(STDIN_FILENO,inputBuffer,MAX_LINE);  

    /* 0 is the system predefined file descriptor for stdin (standard input),
       which is the user's screen in this case. inputBuffer by itself is the
       same as &inputBuffer[0], i.e. the starting address of where to store
       the command that is read, and length holds the number of characters
       read in. inputBuffer is not a null terminated C-string. */

    start = -1;
    if (length == 0)
        exit(0);            /* ^d was entered, end of use r command stream */

/* the signal interrupted the read system call */
/* if the process is in the read() system call, read returns -1
  However, if this occurs, errno is set to EINTR. We can check this  value
  and disregard the -1 value */
    if ( (length < 0) && (errno != EINTR) ) {
        perror("error reading the command");
	exit(-1);           /* terminate with error code of -1 */
    }

	//
	// printf(">>%s<<",inputBuffer);
    for (i=0;i<length;i++){ /* examine every character in the inputBuffer */

        switch (inputBuffer[i]){
	    case ' ':
	    case '\t' :               /* argument separators */
		if(start != -1){
                    args[ct] = &inputBuffer[start];    /* set up pointer */
		    ct++;
		}
                inputBuffer[i] = '\0'; /* add a null char; make a C string */
		start = -1;
		break;

            case '\n':                 /* should be the final char examined */
		if (start != -1){
                    args[ct] = &inputBuffer[start];     
		    ct++;
		}
                inputBuffer[i] = '\0';
                args[ct] = NULL; /* no more arguments to this command */
		break;

	    default :             /* some other character */
		if (start == -1)
		    start = i;
                if (inputBuffer[i] == '&'){
		        *background  = 1;
		        inputBuffer[i-1] = '\0';
		}
	} /* end of switch */
    }    /* end of for */
    args[ct] = NULL; /* just in case the input line was > 80 */
    count_command=ct;
 //   printf("arguman sayısı =%d\n",count_command);

} /* end of setup routine */
 
int main(void)
{
    char inputBuffer[MAX_LINE]; /*buffer to hold command entered */
    int background; /* equals 1 if a command is followed by '&' */
    char *args[MAX_LINE/2 + 1]; /*command line arguments */
    pid_t childpid;
    char temp_path[128];
    char * found;
    char *pathArr[20];
    char*set_to;
    char*aliasholder[30];


    char* allPath=getenv("PATH");;
    int i,j,k;
    i=0;
    j=0;
    k=0;
    char s[1]=":";
    char*token;
    token=strtok(allPath,s);
    while( token != NULL ) {

        pathArr[i]=token;
        i++;
        token = strtok(NULL, s);
    }
    int abc;


    //-----------------------------------------------

    while (1) {

       // perror("error burda ");
        jump:
        background = 0;
        printf("myshell: ");
        fflush(stdout);


        /*setup() calls exit() when Control-D is entered */
        setup(inputBuffer, args, &background);

       // printf("%d,%d,%d,%d,%d",redirectCheck(args),appendCheck(args),revredirect(args),stderrCheck(args),inoutredirectCheck(args));


        if(args[0]==NULL){
            continue;
        }
        if(!strcmp(args[count_command-1],"&")) {
            args[count_command - 1] = NULL;
        }
        //----------------------------------------
        if (!strcmp(args[0], "clr")&&args[1]==NULL) {
            if (fork() != 0) {
                wait(NULL);

            } else {
                for(i=0;i<20;i++) {
                    strcpy(temp_path, "/usr/bin/clear");
                    execl(temp_path, "clear", NULL);
                }
            }
        }


        //------------------------ALIAS-------------------------------
        else if (strcmp(args[0],"alias")==0){




        }

        //-----------------------------------------------------------
        else if (!strcmp(args[0], "exit")) {
            printf("\nThere may be background processes still running. \nTerminate those processes to exit.\n");
            while (wait(NULL) > 0); // Wait for background processes

            printf("\nProgram terminated.\n\n");
            break;
        }
        else {
            if(redirectCheck(args)==1||appendCheck(args)==1||stderrCheck(args)==1|inoutredirectCheck(args)==1||revredirect(args)==1){
                 redirect(args,pathArr);
            }
            else {

                abc=getlocation(pathArr,args[0]);
               childpid=fork();
               if (childpid<0) {
                   perror("Failed to fork");
               }
               else if(childpid==0) {
                   strcpy(temp_path,pathArr[abc]);

                   strcat(temp_path,"/");
                   strcat(temp_path, args[0]);
                   execl(temp_path, args[0],args[1],args[2],args[3],args[4],args[5],args[6],args[7],args[8],args[9],args[10], NULL);
                   perror("\nWrong argument ");
                   break;
               }



                if(!background&&childpid>0){
                    while(wait(NULL)>0);
                }

            }
        }

    }
                        //(2) the child process will invoke execv()
						//(3) if background == 0, the parent will wait,
                        //otherwise it will invoke the setup() function again. */
}



int getlocation(char *path[20],char*command){
    FILE *fptr;
    int i,j,k;
    char filepath[20];
    //filepath=path[0];
    //printf("%s\n",filepath);
    i=0;
    j=0;
    k=0;

    while(path[i]!=NULL)
    {
        while(path[i][j]!='\0'){
            filepath[j]=path[i][j];
            //printf("%c\n",filepath[i]);
            j++;

        }


        strcat(filepath,"/");
        strcat(filepath,command);
        fptr = fopen(filepath,"r");
        if(fptr != NULL)
        {
            return i;
        }
        while(filepath[k]!='\0'){
            filepath[k]='\0';
            k++;
        }
       // printf("%s\n",filepath);
        j=0;
        k=0;
        i++;
    }

    return -1;
}
int redirectCheck(char*args[]){
    int j=0;
    while(args[j]!=NULL){
        if (!strcmp(args[j],">")) {
            if(args[j+1]==NULL){
                perror("Wrong argument");
                return -1;
            }
            else {
                if(args[j+2]!=NULL){
                    perror("Wrong :D argument");
                    return -1;
                }
                else
                    return 1;

            }
        }
        j++;
    }

    return 0;

}
int appendCheck(char* args[]){
    int j=0;
    while(args[j]!=NULL){
        if (!strcmp(args[j],">>")) {
            if(args[j+1]==NULL){
                perror("Wrong argument");
                return -1;
            }
            else {
                if(args[j+2]!=NULL){
                    perror("Wrong :D argument");
                    return -1;
                }
                else
                    return 1;
            }
        }
        j++;
    }

    return 0;
}
int stderrCheck(char*args[]){
    int j=0;
    while(args[j]!=NULL){
        if (!strcmp(args[j],"2>")) {
            if(args[j+1]==NULL){
                perror("Wrong argument");
                return -1;
            }
            else {
                if(args[j+2]!=NULL){
                    perror("Wrong :D argument");
                    return -1;
                }
                else
                    return 1;
            }
        }
        j++;
    }

    return 0;

}
int revredirect(char*args[]){
    int j=0;
    //perror("error :D");

    while(args[j]!=NULL){
        if (!strcmp(args[j],"<")) {
            if(args[j+1]==NULL){
                perror("Wrong argument");
                return -1;
            }
            else {
                if(args[j+2]!=NULL){
                    perror("Wrong :D argument");
                    return -1;
                }
                else
                    return 1;
            }
        }
        j++;
    }
    return 0;
}
int inoutredirectCheck(char*args[]){
    int j=0;
    while(args[j]!=NULL){
        if (!strcmp(args[j],"<")) {
            if(args[j+2]==NULL){
                break;
            }

            if(args[j+1]!=NULL){
                if(!strcmp(args[j+2],">")){
                    if(args[j+3]!=NULL){
                        return 1;

                    }
                    perror("Wrong argument");
                    return -1;
                }
                else
                    return 0;
            }
            else {
                perror("Wrong argument");
                return -1;


            }
        }
        j++;
    }
    return 0;
}
void redirect(char * args[],char*path[20]) {

    pid_t childpid;
    char * command[100];
    char * inputf;
    char * outputf;
    int fd;
    int abc;
    char temp_path[128];

    int k =0;
    while (args[k]){
        if ((!strcmp(args[k],">")) ||(!strcmp(args[k],">>")) || (!strcmp(args[k],"2>")) || (!strcmp(args[k],"<")) || (!strcmp(args[k],"&"))){

            break;
        }
        command[k] = args[k];
        printf(":%s",command[k]);
        k++;

    }

    abc=getlocation(path,command[0]);

    strcpy(temp_path,path[abc]);
    strcat(temp_path,"/");
    strcat(temp_path, command[0]);



    printf("%s",temp_path);



    childpid=fork();

    if (childpid==0) {
        if (inoutredirectCheck(args)) {
            inputf = args[k + 1];
            outputf = args[k + 3];
            //printf("%s,%s",inputf,outputf);
            //perror("inout");


        } else if (revredirect(args)) {

            perror("revredir");
        } else if (redirectCheck(args)) {


            outputf = args[k + 1];
            fd = open(outputf, O_CREAT | O_TRUNC | O_WRONLY, 0600);
            dup2(fd, STDOUT_FILENO);
            close(fd);

            //perror("redir");
        } else if (appendCheck(args)) {
            perror("append");
        } else if (stderrCheck(args)) {
            perror("stderr");
        }
        printf("%s",command[0]);
        execl("/bin/ls","ls",NULL);
        execl(temp_path,command[0],command[1], NULL);

    }

}

