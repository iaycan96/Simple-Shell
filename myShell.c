

    #include <stdio.h>
    #include <unistd.h>
    #include <errno.h>
    #include <sys/wait.h>
    #include <stdlib.h>
    #include <string.h>
    #include <fcntl.h>
    #define MAX_LINE 80 /* 80 chars per line, per command, should be enough. */

    struct node
    {
        char str[50];
        char *args[30];
        struct node *next;
    };
    typedef struct node node;
    node* list;
    struct node_bg{
        int pid;
        struct node* next;
    };

    typedef struct node_bg node_bg;
    node_bg *bg_list;

    void setup(char inputBuffer[], char *args[],int *background);
    void bg_2_fg(node_bg**list);
    void print_bg(node_bg **list);
    void insert_bg(node_bg** head_ref, int new_data);
    int checknode(node**list,char*key);
    int count_command;
    void deleteNode(node **head_ref, char* key);
    int doalias(node ** list,char*command,char*path[20]);
    void print(node **list);
    char *substring(char *string, int position, int length);
    void insertLL (node**r,char str[50],char*args[]);
    int getlocation(char *path[20],char*command);
    int redirectCheck(char*args[]);
    int appendCheck(char* args[]);
    int stderrCheck(char*args[]);
    int revredirect(char*args[]);
    int inoutredirectCheck(char*args[]);
    void redirect(char * args[],char*path[20]);
    int search_bglist(node_bg** list,int key);


    int main(void){
        char inputBuffer[MAX_LINE]; /*buffer to hold command entered */
        int background; /* equals 1 if a command is followed by '&' */
        char *args[MAX_LINE/2 + 1]; /*command line arguments */
        pid_t childpid;
        char temp_path[128];
        char * found;
        char *pathArr[20];


        char* allPath=getenv("PATH");;      //finds all pathes
        int i,j,k;
        int controller;
        int checkbg;
        i=0;
        j=0;
        k=0;
        int abc;
        char s[1]=":";

        //divides path
        char*token;
        token=strtok(allPath,s);
        while( token != NULL ) {

            pathArr[i]=token;
            i++;
            token = strtok(NULL, s);
        }


        //-----------------------------------------------

        while (1) {

           // perror("error burda ");
            background = 0;
            printf("myshell: ");
            fflush(stdout);

            controller=0;


            /*setup() calls exit() when Control-D is entered */
            setup(inputBuffer, args, &background);


            if(args[0]==NULL){      //if there is no argumant
                continue;
            }
            if(!strcmp(args[count_command-1],"&")) {        //deletes &
                args[count_command - 1] = NULL;
            }

            controller= doalias(&list,args[0],pathArr);

            if(controller==1 || controller==-1){
                continue;
            }
///For clear operation
            if (!strcmp(args[0], "clr")&&args[1]==NULL) {
                childpid=fork();
                if (childpid>0) { ///parent function waits until child finishes
                    waitpid(childpid,NULL,0);
                } else {

                    strcpy(temp_path, "/usr/bin/clear");
                    execl(temp_path, "clear", NULL);
                }
            }

            else if (strcmp(args[0],"alias")==0){
                if(args[1]==NULL){
                    perror("Wrong command");

                    continue;
                }
                if(!strcmp(args[1],"-l")){
                    if(args[2]==NULL){
                        print(&list);
                    }
                    else {
                        perror("command not found");
                        continue;
                    }
                }
                else{
                    if(args[2]==NULL){
                        perror("Error:");
                        continue;
                    }
                    if(!strcmp(args[1],"\"\"")) {
                        perror("error");
                        continue;
                    }

                    for (i=0;args[i+1]!=NULL;i++){
                    }
                    if(args[1][0]!='"'||args[i-1][strlen(args[i-1])-1]!='"' || strlen(args[i-1])==1){
                        perror("command not found");

                        continue;
                    }

                    if(checknode(&list,args[i])){
                        printf("\n%s is already in the alias list, first you must unalias\n",args[i]);
                        continue;
                    }
                    insertLL(&list,args[i],args);
                }


            }
            else if(!strcmp(args[0],"unalias")){
                deleteNode(&list,args[1]);
            }


            //-----------------------------------------------------------
            else if (!strcmp(args[0], "exit")&& args[1]==NULL) {
                printf("\nThere may be background processes still running. \nTerminate those processes to exit.\n");
                while (wait(NULL) > 0); // Wait for all background processes

                printf("\nProgram terminated.\n\n");
                break;
            }

            else if(!strcmp(args[0],"fg")&&args[1]==NULL){//makes all background process tp background
                bg_2_fg(&bg_list);
            }


            else {          //if not exit,alias,unalias,clr ,fg

                //Checks if there is any redirection operation
                if(redirectCheck(args)==1||appendCheck(args)==1||stderrCheck(args)==1|inoutredirectCheck(args)==1||revredirect(args)==1){
                     redirect(args,pathArr);
                }
                else {// if there is not any redirection

                    abc=getlocation(pathArr,args[0]);
                    if(abc==-1){
                        perror("Wrong command");
                        continue;
                    }

                   childpid=fork();
                   if (childpid<0) {
                       perror("Failed to fork");
                   }
                   else if(childpid==0) {

                       strcpy(temp_path,pathArr[abc]);

                       strcat(temp_path,"/");
                       strcat(temp_path, args[0]);
                       execl(temp_path, args[0],args[1],args[2],args[3],args[4],args[5],args[6],args[7],args[8],args[9],args[10],args[11],args[12],args[13],args[14],args[15],args[16],args[17],args[18],args[19],args[20],args[21],args[22],args[23],args[24], NULL);
                       perror("\nWrong argument ");
                       break;

                   }

                    else{

                        if (!background)
                            waitpid(childpid,NULL,0);
                        else{
                            insert_bg(&bg_list,childpid);
                        }
           
                    }

                }
            }

        }
                            //(2) the child process will invoke execv()
                            //(3) if background == 0, the parent will wait,
                            //otherwise it will invoke the setup() function again. */
    }
    int getlocation(char *path[20],char*command){//finds the location of command in pathlist
        FILE *fptr;
        int i,j,k;
        char filepath[128];
        i=0;
        j=0;
        k=0;

        while(path[i]!=NULL)
        {
            while(path[i][j]!='\0'){
                filepath[j]=path[i][j]; //harf harf filepathe pathi kopyalıyo
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
    int redirectCheck(char*args[]){// ">" checs redirection
        int j=0;
        while(args[j]!=NULL){
            if (!strcmp(args[j],">")) {
                if(args[j+1]==NULL){ //check if there is a filename
                    perror("Wrong argument");
                    return -1;
                }
                else { // there cant be 2 files
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
    int appendCheck(char* args[]){//">>" checks append redirection
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

    int stderrCheck(char*args[]){// checks standart error redirection
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
    int revredirect(char*args[]){//checks reverse redirection
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
    int inoutredirectCheck(char*args[]){//"<" ">"  checks input and output redirection
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
    void redirect(char * args[],char*path[20]) {//makes redirection operations
        int i;
        pid_t childpid;
        char * command[100];
        char * inputf;
        char * outputf;
        int fd;
        int abc;
        char temp_path[128];

        int k =0;
        while (args[k]){
            if ((!strcmp(args[k],">"))||(!strcmp(args[k],",")) ||(!strcmp(args[k],">>")) || (!strcmp(args[k],"2>")) || (!strcmp(args[k],"<")) || (!strcmp(args[k],"&"))){

                break;
            }
            command[k] = args[k];
            //printf(":%s",command[k]);
            k++;
        }

        for(i=k;i<100;i++)
            command[k]=NULL;



        abc=getlocation(path,command[0]);
        if(abc!=-1)
            strcpy(temp_path,path[abc]);
        else { // if path is not found
            if(stderrCheck(args)){
                strcpy(temp_path, path[0]);//default olarak temp pathe 0. pathi ekliyo
            }
            else{
                perror("Path could not found");
                return;
            }

        }
        strcat(temp_path,"/");
        strcat(temp_path, command[0]);

       // perror("burdaa");

        //printf("%s",temp_path);



        childpid=fork();

        if (childpid==0) {
            if (inoutredirectCheck(args)) {//// sort < in.file > out.file
                inputf = args[k + 1];
                outputf = args[k + 3];


                fd = open(inputf, O_RDONLY, 0600);
                dup2(fd, STDIN_FILENO);
                close(fd);

                fd = open(outputf, O_CREAT | O_TRUNC | O_WRONLY, 0600);
                dup2(fd, STDOUT_FILENO);
                close(fd);

                //printf("%s,%s",inputf,outputf);
                //perror("inout");


            } else if (revredirect(args)) { //sort < in.file
                inputf=args[k+1];

                fd = open(inputf, O_RDONLY, 0600);
                dup2(fd, STDIN_FILENO);
                close(fd);

                //perror("revredir");
            } else if (redirectCheck(args)) {//ls > out.file


                outputf = args[k + 1];
                fd = open(outputf, O_CREAT | O_TRUNC | O_WRONLY, 0600);
                dup2(fd, STDOUT_FILENO);
                close(fd);

                //perror("redir");
            } else if (appendCheck(args)) {//>>
                outputf=args[k+1];

                fd = open(outputf, O_CREAT | O_APPEND | O_WRONLY, 0600);
                dup2(fd, STDOUT_FILENO);
                close(fd);
               // perror("append");
            } else if (stderrCheck(args)) {

                outputf=args[k+1];
                fd = open(outputf,O_CREAT | O_TRUNC | O_WRONLY, 0600);

                dup2(fd, STDERR_FILENO);
                close(fd);


                perror("stderr");
            }
           // printf("%s",command[0]);
           // execl("/bin/ls","ls",NULL);
            execl(temp_path,command[0],command[1],command[2],command[3],command[4], NULL);
            //perror("Wrong command");
            exit(0);
        }
        else{ //if command is not working correctly
            waitpid(childpid,NULL,0);
            kill(childpid,SIGKILL);// child  ölmediyse öldür
        }

    }
    void insertLL (node**r,char str[50],char*args[]){			//insert function
        node*iter;
        node* temp= *r;
        char tempstr[20];
        int k;
        int j;
        char*tempstr1;
        temp=(node*)malloc(sizeof(node));		//allocates memory insert new element
       /* for (j = 0; j <20 ;j++) {
            for (k=0;k<30;k++){
                temp->args[j][k]=NULL;
            }

        }*/

        strcpy(temp->str,str);					//copies str strings  to temps str


        int i=0;
        while(args[i+1]!=NULL){
            if(i==0){
                strcpy(tempstr,args[i+1]);
                if(args[i+3]==NULL){
                    tempstr1=substring(tempstr,2,strlen(tempstr)-2);
                }
                else{
                    tempstr1=substring(tempstr,2,strlen(tempstr)-1);

                }


                //printf("kesilmiş :%s\n,",tempstr1);
                //temp->args[i]="\0";
                temp->args[i]=malloc(sizeof(char *)*strlen(tempstr1));
                strcpy(temp->args[i],tempstr1);
                //tempstr1="";

                if(args[i+3]==NULL){
                    break;
                }
                i++;
                continue;

            }
            if(args[i+3]==NULL){
                    strcpy(tempstr,args[i+1]);
                    tempstr1=substring(tempstr,1,strlen(tempstr)-1);
                    temp->args[i]=malloc(sizeof(char *)*strlen(tempstr1));
                    strcpy(temp->args[i],tempstr1);
                    //temp->args[i+1]=;
                    //printf("kesilmiş:%s\n",tempstr1);

                    i++;
                    break;
            }
            temp->args[i]=malloc(sizeof(char *)*strlen(tempstr1));
        strcpy(temp->args[i],args[i+1]);
        i++;
        }



        temp->next=NULL;

        if(*r==NULL)							//if linked list is empty
        {
            *r=temp;
        }
        else									//if linkedlist is not empty
        {
            iter=*r;
            while(iter->next!=NULL)
            {
                iter=iter->next;
            }
            iter->next=temp;
        }
    }
    char *substring(char *string, int position, int length){//substring function
        char *pointer;
        int c;

        pointer = malloc(length+1);

        if (pointer == NULL)
        {
            perror("Unable to allocate memory.\n");
            exit(1);
        }

        for (c = 0 ; c < length ; c++)
        {
            *(pointer+c) = *(string+position-1);
            string++;
        }

        *(pointer+c) = '\0';

        return pointer;
    }
    void print(node **list){			//prints alias linkedlists all elements{
        node*p;
        p=*list;
        int i;
        while(p!=NULL)
        {
            i=0;
            printf("%s  ",p->str);//prints linkedlists elements
            while(p->args[i]!=NULL){
                printf("%s",p->args[i]);
                i++;
            }
            printf("\n");
            p=p->next;
        }
        printf("\n");		//prints the number of  common words adn 2-grams

    }
    int doalias(node ** list,char*command,char*path[20]){//runs alias command
        pid_t childpid;
        node*p;
        char temp_path[128];
        int abc;
      //  abc=getlocation(path,"ls");

        p=*list;
        while (p!=NULL){
            if(!strcmp(p->str,command)){
                abc=getlocation(path,p->args[0]);
                if(abc==-1){
                    perror("Wrong command");
                    return -1;

                }

                childpid=fork();
                if (childpid<0) {

                    perror("Failed to fork");
                }
                else if(childpid==0) {
                    strcpy(temp_path,path[abc]);

                    strcat(temp_path,"/");
                    strcat(temp_path, p->args[0]);
                    execl(temp_path, p->args[0],p->args[1],p->args[2],p->args[3],p->args[4],p->args[5],p->args[6],p->args[7],p->args[8],p->args[9],p->args[10],p->args[11],p->args[12],p->args[13],p->args[14],p->args[15],p->args[16],p->args[17],NULL);
                    perror("\nWrong command ");
                    break;

                }
                else {
                    waitpid(childpid,NULL,0);
                    return 1;
                }



            }
            p=p->next;

        }
        return 0;

    }
    void deleteNode(node **head_ref, char* key){ //Deletes a build in command in alias list
        // Store head node
        node *temp = *head_ref, *prev;

        // If head node itself holds the key to be deleted
        if (temp != NULL && !strcmp(temp->str,key))
        {
            *head_ref = temp->next;   // Changed head
            free(temp);               // free old head
            return;
        }

        // Search for the key to be deleted, keep track of the
        // previous node as we need to change 'prev->next'
        while (temp != NULL && strcmp(temp->str,key))
        {
            prev = temp;
            temp = temp->next;
        }

        // If key was not present in linked list
        if (temp == NULL) {
            perror("Key has not found");
            return;}

        // Unlink the node from linked list
        prev->next = temp->next;

        free(temp);  // Free memory
    }
    int checknode(node**list,char*key){//searc if key is in alias list
        node*temp=*list;
        while(temp!=NULL){
            if(!strcmp(temp->str,key))
                return 1;
            temp=temp->next;
        }
        return 0;
    }
    void insert_bg(node_bg** list, int data){//Inserts new element to background list
        node_bg* new_node = (node_bg*) malloc(sizeof(node_bg));

        node_bg *iter = *list;
        new_node->pid  =data;

        new_node->next = NULL;

        if (*list == NULL)
        {
            *list = new_node;
            return;
        }
        while (iter->next != NULL)
            iter = iter->next;
        iter->next = new_node;
        return;
    }
    void print_bg(node_bg **list){			//prints linkedlists all elements{
        node_bg *p;
        p=*list;
        while(p!=NULL)
        {
            printf("%d\n",p->pid);//prints linkedlists elements
            p=p->next;
        }
        printf("\n");		//prints the number of  common words adn 2-grams

    }
    void bg_2_fg(node_bg**list){  //turns background process into foreground
        node_bg*p;
        p=*list;
        while (p!=NULL){
            waitpid(p->pid,NULL,0);
            kill(p->pid,SIGKILL);
     
            p=p->next;
        }
    }
    void setup(char inputBuffer[], char *args[],int *background){
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

