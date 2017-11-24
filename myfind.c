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
//分别是-name, -prune, -ctime, -mtime, -print
int state[5] = {0, 0, 0, 0, 0};
//存放-name指定的名字
char name[512];
//存放-prune指定的名字
char pname[512];
//存放-ctime指定的参数, 如-8
char ctim[32];
//存放-mtime指定的参数, 如-8
char mtim[32];

//递归搜寻目录
void listDir(char* path);
//通配符匹配
int isMatch(const char *s, const char *p);
//10的i次方
//因为math.h调用了几次都失败, 就自己写了
int p10(int i);

int main(int argc, char *argv[])
{
	if(argc < 2)
	{
		printf("参数不够\n");
		exit(1);
	}
	
	//存放要查询的目录
	char* path = argv[1];
	//遍历参数
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
			//将输入的name放入全局变量里
			strcpy(name, argv[i+1]);
			//状态值置1
			state[0] = 1;
		}
		else if(strcmp(argv[i], "-prune")==0)
		{
			//如果指定了-prune但没输入目录名
			if(i>=argc-1)
			{
				printf("请指定查询文件名\n");
				exit(1);
			}
			//排除目录按照通配符匹配来排除, 预先在头尾加上通配符*
			//输入di1, 会将dir1, dir2都排除
			strcpy(pname, "*");
			strcat(pname, argv[i+1]);
			strcat(pname, "*");
			state[1] = 1;
		}
		else if(strcmp(argv[i], "-ctime")==0)
		{
			//如果指定了-ctime但没输入时间
			if(i>=argc-1)
			{
				printf("请指定查询时间\n");
				exit(1);
			}
			strcpy(ctim, argv[i+1]);
			state[2] = 1;
		}
		else if(strcmp(argv[i], "-mtime")==0)
		{
			//如果指定了-mtime但没输入时间
			if(i>=argc-1)
			{
				printf("请指定查询时间\n");
				exit(1);
			}
			strcpy(mtim, argv[i+1]);
			state[3] = 1;
		}
		else if(strcmp(argv[i], "-print")==0)
		{
			state[4] = 1;
		}
	}
	
	//不晓得啥时候输出.和..目录, 直接设定指定-name, -ctime, -mtime时都不输出.和..
	if(state[0]!=1 && state[2]!=1 && state[3]!=1) printf(".\n..\n");
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
			//存放该文件的全路径名
			char fullName[1024];
			sprintf(fullName, "%s/%s", path, mydirent->d_name);

			//做菊花链控制
			int flag = 1;

			//如果是.和.. 不用进入里面遍历
			if(strcmp(mydirent->d_name, ".") == 0 || strcmp(mydirent->d_name, "..") == 0) continue;
			//如果指定了-prune且匹配文件名, 则flag==0
			//isMatch为通配符匹配
			if(state[1]==1 && isMatch(fullName, pname)) flag = 0;
			//如果指定了-name 但 该文件不符合指定的名字, flag==0
			if(flag && state[0]==1 && !isMatch(mydirent->d_name, name)) flag = 0;
			//-ctime
			if(flag && state[2]==1)
			{
				//查询文件详细信息
				struct stat buf;
				stat(fullName, &buf);

				//求出当前时间-文件ctime时间, 算出差值
				time_t t;
				time(&t);
				//一天86400秒, 算出差几天
				int diff = (t - buf.st_ctim.tv_sec)/86400;

				//ctim[]里存的是-12这种字符串, 这一步提取出后面的12字符串并转化为int存在num里
				int num = 0;
				int i = 0;
				while(ctim[i]!='\0') ++i;
				for(int j = 1; ctim[j]!='\0'; ++j, --i) num+=(ctim[j]-48)*p10(i-2);

				//如果是-则差值一旦大于要求的, flag==0
				if(ctim[0]=='-')
				{
					if(diff>num) flag = 0;
				}
				//如果是+则差值一旦小于要求的, flag==0
				else if(ctim[0]=='+')
				{
					if(diff<=num)
					{
						flag = 0;
					}
				}
			}
			//mtime
			if(flag && state[3]==1)
			{
				struct stat buf;
				stat(fullName, &buf);

				time_t t;
				time(&t);
				int diff = (t - buf.st_atim.tv_sec)/86400;

				int num = 0;
				int i = 0;
				while(mtim[i]!='\0') ++i;
				for(int j = 1; mtim[j]!='\0'; ++j, --i) num+=(mtim[j]-48)*p10(i-2);

				if(mtim[0]=='-')
				{
					if(diff>num) flag = 0;
				}
				else if(mtim[0]=='+')
				{
					if(diff<=num)
					{
						flag = 0;
					}
				}
			}
			//如果完整通过菊花链, 则输出全路径名
			if(flag) printf("%s\n", fullName);
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

int p10(int i)
{
    if(i == 0) return 1;
    if(i == 1) return 10;
    return 10*p10(i-1);
}
