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

void listDir(char* path);

int main(int argc, char *argv[])
{
	if(argc < 2)
	{
		printf("参数不够\n");
		exit(1);
	}

	char* path = argv[1];
	printf(".\n..\n");
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
			printf("%s/%s\n", path, mydirent->d_name);
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
