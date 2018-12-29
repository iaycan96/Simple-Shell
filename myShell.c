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

	printf(">>%s<<",inputBuffer);
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
    printf("arguman sayısı =%d\n",count_command);

	for (i = 0; i <= ct; i++){

         /* if(!(strcmp(args[i],"&") && args[i+1]==NULL) {
         printf("bu ampersant");


          }*/
            printf("args %d = %s\n",i,args[i]);
	}

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


    char* allPath=getenv("PATH");;
    int i,j,k;
    i=0;
    j=0;
    k=0;
 //   printf("%s\n",allPath);
    char s[1]=":";
    char*token;
    token=strtok(allPath,s);

    while( token != NULL ) {

      //  printf( " %s\n", token );
        pathArr[i]=token;
        i++;
        token = strtok(NULL, s);
    }

    int abc;
    abc=getlocation(pathArr,"sort");

    //-----------------------------------------------

    while (1) {

        int commandChecker = 0, redirectChecker = 0, appendChecker = 0, stderrChecker = 0, fileNumber = 0;
        background = 0;
        printf("myshell: ");
        fflush(stdout);

        /*setup() calls exit() when Control-D is entered */
        setup(inputBuffer, args, &background);

        printf("%d\n",background);

        if (!strcmp(args[0], "clr")) {
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
        if (strcmp(args[0],"alias")==0){
         //   strcpy(args[count_command])



        }










        //-----------------------------------------------------------
        else if (!strcmp(args[0], "exit")) {
            printf("\nThere may be background processes still running. \nTerminate those processes to exit.\n");
            while (wait(NULL) > 0); // Wait for background processes

            printf("\nProgram terminated.\n\n");
            break;
        }
        else {
            /*for (i = 0; args[i] != NULL; i++)
            {
                if (!strcmp(args[i], ">") || !strcmp(args[i], ">>") || !strcmp(args[i], "2>"))
                {
                    if (args[i + 1] == NULL)
                    {
                        fprintf(stderr, "\nWrong input!");
                        continue;
                    }
                    args[i + 2] = NULL;
                    redirectChecker = 1;
                    fileNumber = i + 1;
                    if (!strcmp(args[i], ">>"))
                    {
                        appendChecker = 1;
                    }
                    else if (!strcmp(args[i], "2>"))
                    {
                        stderrChecker = 1;
                    }
                }
            }
            printf("> var:%d,")*/










            // (1) fork a child process using fork()
            abc=getlocation(pathArr,args[0]);
           // printf("%s",pathArr[abc]);

           if (fork() != 0) {
           //    wait(NULL);
           } else {
               strcpy(temp_path,pathArr[abc]);
               strcat(temp_path,"/");
               strcat(temp_path, args[0]);
               execl(temp_path, args[0],args[1],args[2],args[3],args[4],args[5],args[6],args[7],args[8],args[9],args[10], NULL);
               perror("\nWrong argument ");
               exit(0);
           }


            if(background==0){
                wait(NULL);
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
        //printf("%d:%s",i,filepath);
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
