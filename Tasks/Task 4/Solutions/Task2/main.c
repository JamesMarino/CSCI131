#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include <hiredis/hiredis.h>

static char* hostname="127.0.0.1";
static redisContext *ctx;
static int port=6379;

static void setupConnection(){
    struct timeval timeout = {1, 500000};
    ctx=redisConnectWithTimeout(hostname,port,timeout);
    if (ctx==NULL ||ctx->err) {
        if (ctx) {
            printf("Connection error: %s\n",ctx->errstr);
            redisFree(ctx);
        } else {
            printf("Connection error: can't allocate redis context\n");
        }
        exit(1);
    }
    printf("Connected to redis server\n");
}

static void closeConnection(){
    redisFree(ctx);
    printf("Disconnected from redis server\n");
}

static void getstr(char* str, int maxlen){
    memset(str,0,maxlen);
    fgets(str, maxlen, stdin);
    int len=strlen(str);
    len--;
    if(str[len]=='\n') str[len]='\0';
}

static void addProject(){
    printf("Project title : ");
    char title[128];
    getstr(title, 128);
    
    printf("Supervisor  : ");
    char supervisor[128];
    getstr(supervisor, 128);
    
    redisReply *reply;
    reply=redisCommand(ctx, "INCR projectcount");
    int acount=(int) reply->integer;
    freeReplyObject(reply);
    
    char projid[128];
    memset(projid, 0, 128);
    sprintf(projid, "Project:%d",acount);
    
    reply=redisCommand(ctx, "SADD Projects %s", projid);
    freeReplyObject(reply);
    
    reply=redisCommand(ctx, "HMSET %s Title %s Supervisor %s",projid,title,supervisor);
    freeReplyObject(reply);
}

static void deleteProject(){
    char pname[128];
    
    printf("Delete project with id : ");
    getstr(pname, 128);
    
    redisReply *reply;
    reply=redisCommand(ctx, "SISMEMBER Projects %s", pname);
    int res=(0==reply->integer);
    
    if(res){
        printf("There was no project with that id\n");
    } else {
        reply = redisCommand(ctx, "SREM Projects %s", pname);
        
        freeReplyObject(reply);
        reply=redisCommand(ctx,"DEL %s",pname);
        freeReplyObject(reply);
    }
}

static void listProjects(){
    redisReply *reply;
    reply=redisCommand(ctx, "SCARD Projects");
    
    int count=(int) reply->integer;
    freeReplyObject(reply);
    printf("There are %d projects currently in the database\n",count);
    char ** projnames=malloc(count*sizeof(char*));
    reply=redisCommand(ctx,"SMEMBERS Projects");
    int j;
    for (j=0; j<reply->elements; j++){
        projnames[j]=strdup(reply->element[j]->str);
    }
    freeReplyObject(reply);
    
    for (j=0; j<count; j++){
        reply=redisCommand(ctx, "HGETALL %s",projnames[j]);
        printf("Project %s\n",projnames[j]);
        int k;
        for(k=0; k<reply->elements; k+=2){
            printf("%s: %s\n",reply->element[k]->str,reply->element[k+1]->str);
        }
        freeReplyObject(reply);
    }
    
    for(j=0;j<count;j++)
        free(projnames[j]);
    free(projnames);
}

static void handleProjectCommands(){
    for(;;){
        printf("Project=>");
        char command[128];
        getstr(command, 128);
        if (command[0]=='q') break;
        if (command[0]=='?'){
            printf("Project commands:\n");
            printf("\t? : print this command list\n");
            printf("\tq : finish project commands\n");
            printf("\tAdd : Add a project\n");
            printf("\tDelete : Delete a project\n");
            printf("\tList : List projects\n");
        }
        if (0==strcmp(command, "Add")) addProject();
        else
            if (0==strcmp(command, "Delete")) deleteProject();
        else
            if (0==strcmp(command, "List")) listProjects();
    }
}

static void addGroup(){
    redisReply *reply;
    reply=redisCommand(ctx,"INCR groupcount");
    int acount=(int)reply->integer;
    freeReplyObject(reply);
    
    char projid[128];
    memset(projid, 0, 128);
    sprintf(projid, "Group:$d",acount);
    
    reply=redisCommand(ctx,"SADD Groups %s",projid);
    freeReplyObject(reply);
}

static void deleteGroup(){
    char gname[128];
    printf("Delete group with id : ");
    getstr(gname, 128);
    
    redisReply *reply;
    reply=redisCommand(ctx, "SISMEMBER Groups %s",gname);
    int res=(0==reply->integer);
    
    if(res){
        printf("There was no group with that id\n");
    } else {
        reply=redisCommand(ctx,"SREM Groups %s",gname);
        
        freeReplyObject(reply);
        reply=redisCommand(ctx, "DEL %s",gname);
        freeReplyObject(reply);
    }
}

static void listGroups(){
    redisReply *reply;
    reply=redisCommand(ctx,"SCARD Groups");
    
    int count=(int)reply->integer;
    freeReplyObject(reply);
    printf("There are %d groups currently in the database\n",count);
    char** groupnames = malloc(count*sizeof(char*));
    
    reply=redisCommand(ctx,"SMEMBERS Groups");
    int j;
    for (j=0; j<reply->elements; j++) {
        groupnames[j]=strdup(reply->element[j]->str);
    }
    freeReplyObject(reply);
    
    for (j=0; j<count; j++) {
        reply=redisCommand(ctx,"SMEMBERS %s",groupnames[j]);
        printf("Group %s\n\tMembers: ",groupnames[j]);
        int k;
        for (k=0; k<reply->elements; k++) {
            printf("%s ",reply->element[k]->str);
        }
        printf("\n");
        freeReplyObject(reply);
    }
    
    for (j=0; j<count; j++) {
        free(groupnames[j]);
    }
    free(groupnames);
    
}

static void addMember(){
    char groupname[128];
    char membername[128];
    printf("Adding student to group\n");
    printf("Group   : ");
    getstr(groupname, 128);
    printf("Student id : ");
    getstr(membername, 128);
    redisReply *reply;
    
    reply=redisCommand(ctx, "SISMEMBER Groups %s",groupname);
    int res=(0==reply->integer);
    freeReplyObject(reply);
    
    if(res){
        printf("There was no group with that id\n");
    }else {
        reply=redisCommand(ctx, "SADD %s %s",groupname,membername);
        freeReplyObject(reply);
    }
}

static void removeMember(){
    char groupname[128];
    char membername[128];
    printf("Removing a student from group\n");
    printf("Group   : ");
    getstr(groupname, 128);
    printf("Student id : ");
    getstr(membername, 128);
    redisReply *reply;
    reply=redisCommand(ctx, "SISMEMBER Groups %s", groupname);
    int res=(0==reply->integer);
    freeReplyObject(reply);
    
    if (res) {
        printf("There was no group with that id\n");
    } else {
        reply=redisCommand(ctx, "SREM %s %s",groupname, membername);
        freeReplyObject(reply);
    }
}

static void assignProject(){
    
}

static void handleGroupCommands(){
    for(;;){
        printf("Groups=>");
        char command[128];
        getstr(command, 128);
        if(command[0]=='q') break;
        if (command[0]=='?'){
            printf("Group commands:\n");
            printf("\t? :   print this command list\n");
            printf("\tq :   finish group commands\n");
            printf("\tAdd: Create new empty group\n");
            printf("\tDelete: Delete a group\n");
            printf("\tList: List groups\n");
            printf("\tMember: Add member to group\n");
            printf("\tRemove: Remove member from group\n");
            printf("\tAssign: Assign project to group");
        }
        
        if (0==strcmp(command, "Add")) addGroup();
        else
            if (0==strcmp(command, "Delete")) deleteGroup();
        else
            if (0==strcmp(command, "List")) listGroups();
		
        else
            if (0==strcmp(command, "Member")) addMember();
        else
            if (0==strcmp(command, "Remove")) removeMember();
        else
            if (0==strcmp(command, "Assign")) assignProject();
    }
}

static void menu_select_loop(){
    for (; ;) {
        printf("=>");
        char command[128];
        getstr(command, 128);
        if (command[0]=='q') break;
        if (command[0]=='?') {
            printf("Commands:\n");
            printf("\t? :   print this command list\n");
            printf("\tq :   quit\n");
            printf("\tRestart: re-initialise the database\n");
            printf("\tProject: sub-commands for project\n");
            printf("\tGroup: sub-commands for group\n");
        }
        if (0==strcmp(command, "Restart")){
            redisReply *reply;
            reply=redisCommand(ctx,"FLUSHDB");
            freeReplyObject(reply);
        } else
            if (0==strcmp(command, "Project"))
                handleProjectCommands();
        else
            if (0==strcmp(command, "Group"))
                handleGroupCommands();
    }
}

int main(int argc, const char * argv[]) {
    setupConnection();
    menu_select_loop();
    closeConnection();
    
    return 0;
}
