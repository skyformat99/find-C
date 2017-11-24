/*************************************************************************
 > File Name: myfind.c
 > Author: huangkaibo
 > Mail: huangkaibochn@gmail.com
 > Created Time: Fri 10 Nov 2017 03:25:39 PM CST
 ************************************************************************/

#include<stdio.h>
#include<dirent.h>
#include<regex.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<stdlib.h>
#include<time.h>
#include<string.h>

//存放选项情况, 0代表没指定, 1代表指定了
//分别是-name, -prune, -mtime, -ctime, -print
int state[5] = {0, 0, 0, 0, 0};
//存放-name指定的名字
char name[512];

//递归搜寻目录
void listDir(char* path);
//通配符匹配
int isMatch(const char *s, const char *p);

int main(int argc, char *argv[])
{
	if(argc < 2)
	{
		printf("参数不够\n");
		exit(1);
	}

	char* path = argv[1];

	for(int i = 0; i <= argc-1; ++i)
	{
		if(strcmp(argv[i], "-name")==0)
		{
			//如果指定了-name但没输入文件名
			if(i>=argc-1)
			{
				printf("请指定查询文件名\n");
				exit(1);
			}
			strcpy(name, argv[i+1]);
			state[0] = 1;
		}
	}

	if(state[0]==0) printf(".\n..\n");
	listDir(path);
}

void listDir(char* path)
{
	DIR* dir;
	struct dirent* mydirent;
	//存放子目录的全名, 用于递归
	char childPath[512];

	if((dir = opendir(path)) != NULL)
	{
		while((mydirent = readdir(dir)) != NULL)
		{
			//如果是.和.. 不用进入里面遍历
			if(strcmp(mydirent->d_name, ".") == 0 || strcmp(mydirent->d_name, "..") == 0) continue;
			//如果没有指定-name, 直接输出文件全名
			if(state[0]!=1) printf("%s/%s\n", path, mydirent->d_name);
			//如果指定了-name 且 该文件符合指定的名字, 才输出出来
			//isMatch为通配符匹配
			else if(state[0]==1 && isMatch(mydirent->d_name, name)) printf("%s/%s\n", path, mydirent->d_name);
			//如果是个子目录
			if(mydirent->d_type & DT_DIR)
			{
				//得到子目录全名递归进入里面遍历
				sprintf(childPath, "%s/%s", path, mydirent->d_name);
				listDir(childPath);
			}
		}
		closedir(dir);
	}
}

//通配符匹配函数
int isMatch(const char *s, const char *p)
{
    // write your code here
    if (s == NULL && p == NULL)
        return 1;
    int m = strlen(s);
    int n = strlen(p);
    if (m == 0 && n == 0)
        return 1;
    int si = 0, pi = 0;
    int xidx = -1, mtch = -1;
    while (si < m)
    {
        if (pi < n && (*(p + pi) == '*'))
        {
            xidx = pi++;
            mtch = si; // si对应xidx的位置
        }
        else if (pi < n && (*(s + si) == *(p + pi) || *(p + pi) == '?'))
        {
            ++si;
            ++pi;
        }
        else if (xidx > -1)
        { // 上一个 '*' 的位置
            pi = xidx + 1;
            si = ++mtch;
        }
        else
        {
            return 0;
        }
    }
    while (pi < n && (*(p + pi) == '*'))
        ++pi;
    return (pi == n);
}
