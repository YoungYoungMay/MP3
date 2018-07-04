//一个简单的mp3播放器
//功能有：播放、暂停、停止、退出、下一首、上一首

#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>

typedef struct node_ node_t;
struct node_
{
    char* name;//歌曲名
    node_t* prev;
    node_t* next;
};
node_t* head = NULL;

int first = 1;//是否第一次播放
node_t* cur = NULL;//指向当前播放的歌曲

enum//播放器的几种状态 
{
    PLAY,
    PAUSE,
    STOP
};
int status = STOP;

void list_init()//初始化一个带头结点的双向链表
{
    head = malloc(sizeof(node_t));
    //初始化
    memset(head, 0x00, sizeof(node_t));
    head->next = head->prev = head;
}

void list_insert(const char* name)//将歌曲插入到链表中保存
{
    node_t* p = malloc(sizeof(node_t));
    memset(p, 0x00, sizeof(node_t));
    
    p->name =  malloc(strlen(name) + 1);
    strcpy(p->name, name);
    
    //进行插入操作
    p->next = head->next;
    p->prev = head;
    head->next->prev = p;
    head->next = p;
}

void list_show()
{
    node_t* p = head->next;
    //依次将链表中的歌曲信息打印出来
    while(p != head)
    {
        printf("%s", p->name);
        //记录当前播放信息
        if(p == cur)
            printf(" ----------> cur");
        printf("\n");
        p = p->next;
    }
}

void load_music(const char* path)
{
    //打开path路径
    DIR* pdir = opendir(path);
    if(pdir == NULL)
    {
        perror("opendir");
        exit(1);
    }

    struct dirent* p = NULL;
    //将path路径下的歌曲插入到链表中
    while((p = readdir(pdir)) != NULL)
    {
        if(p->d_name[0] == '.')//去掉隐藏文件
            continue;
        //printf("%s\n", p->d_name);
        list_insert(p->d_name);
    }
    //关闭path路径
    closedir(pdir);
}

int menu()
{
    printf("+------------------------------------+\n");
    printf("1.play/pause\n");
    printf("2.next\n");
    printf("3.prev\n");
    printf("4.stop\n");
    printf("0.exit\n");
    printf("+------------------------------------+\n");
    list_show();

    int choose = 4;
    do
    {
        printf(" > ");
        scanf("%d", &choose);
        if(choose>=0 && choose<=4)
            break;
        printf("choose invalid\n");
        //刷新输入缓冲区
        //输入缓冲区不刷新，里面一直有内容，就不会等到scanf,一直输出内容
        while(getchar() != '\n');
    }while(1);
    return choose;
}

void playPause()//播放与暂停
{
    if(first == 1)//第一次使用该功能
    {
        char buf[1024] = {0};
        //将播放命令放入buf中
        sprintf(buf, "madplay -o wav:- /home/may/Music/music/%s 2> /dev/null| aplay 2> /dev/null &", cur->name);
        //用system执行buf中命令
        system(buf);
        first = 0;
        //将当前播放器状态修改为play
        status = PLAY;
    }
    else
    {
        if(status == PLAY)//当前播放器正在播放
        {
            system("killall -SIGSTOP aplay");
            status = PAUSE;
        }
        else if(status == PAUSE)//当前播放器暂停
        {
            system("killall -SIGCONT aplay");
            status = PLAY;
        }
    }
}

void stop()//停止->杀死当前的播放器进程
{
   system("killall -SIGKILL aplay");
   first = 1;
}

void next()//下一首
{//先杀掉当前正在播放的，然后指针后移，再播放
    stop();
    cur = cur->next;
    if(cur == head)
        cur = cur->next;
    playPause();
}

void prev()//上一首
{//先杀死当前播放的，然后指针前移，再播放
    stop();
    cur = cur->prev;
    if(cur == head)
        cur = cur->prev;
    playPause();
}

int main(int argc, char* argv[])
{
    list_init();
    load_music("/home/may/Music/music");
    //将cur指针指向第一个有效位置
    if(head->next != head)
        cur = head->next;
    do
    { 
        int choose = menu();
        switch(choose)
        {
            case 1:
                playPause();
                break;
            case 2:
                next();
                break;
            case 3:
                prev();
                break;
            case 4:
                stop();
                break;
            case 0:
                printf("谢谢使用\n");
                system("killall -SIGKILL aplay");
                exit(0);
                break;
            default:
                //do nothing
                break;
        } 
    }while(1);
    //list_show();
    return 0;
}
