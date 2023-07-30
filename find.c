#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"

char*
fmtname(char *path)
{
  static char buf[DIRSIZ+1];
  char *p;

  // Find first character after last slash.
  for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  // Return name, ended with a null character
  if(strlen(p) >= DIRSIZ)
    return p;
  memmove(buf, p, strlen(p));
  memset(buf+strlen(p), '\0', DIRSIZ-strlen(p));
  return buf;
}

void
searchDir(char *path, char *buf, int fd, struct stat st, char *search, char *type, int size, int sizeCompare)
{ 
  char *p;
  struct dirent de;
  struct stat st2;

  if(strlen(path) + 1 + DIRSIZ + 1 > 512){
    printf(2, "find: path too long\n");
  }
  strcpy(buf, path);
  p = buf+strlen(buf);
  *p++ = '/';

  // Iterate through directory contents
  while(read(fd, &de, sizeof(de)) == sizeof(de)){
    if(de.inum == 0)
      continue;
    memmove(p, de.name, DIRSIZ);
    p[DIRSIZ] = 0;
    if(stat(buf, &st) < 0){
      printf(2, "find: cannot stat %s\n", buf);
      continue;
    }
    if (st.type == T_DIR) {
      if (strcmp(fmtname(buf), ".") != 0 && strcmp(fmtname(buf), "..") != 0) {
        // Get new metadata for directory file
        int fd2 = open(buf, 0);
        // Path must have stats
        if(fstat(fd, &st2) < 0){
          printf(1, "find: cannot stat %s\n", path);
          close(fd);
          return;
        }
        if ((strcmp(type,"d")==0 || strcmp(type, "x")==0) && strcmp(fmtname(buf), search) == 0) {
            if(size!=-1)
            {
                if(sizeCompare==1 && st.size >= size)
                    printf(1, "%s\n", buf);
                else if(sizeCompare==-1 && st.size <size)
                    printf(1, "%s\n", buf);
                else if(sizeCompare==0 && st.size == size)
                    printf(1, "%s\n", buf);
            }
            else
                printf(1, "%s\n", buf);
        }
        // Recursive search in found directory
        searchDir(buf, buf, fd2, st2, search, type, size, sizeCompare);
        close(fd2);
      }
    } else if (st.type == T_FILE) {
      if ((strcmp(type,"f")==0 || strcmp(type, "x")==0) && strcmp(fmtname(buf), search) == 0) {
          if(size!=-1)
            {
                if(sizeCompare==1 && st.size >= size)
                    printf(1, "%s\n", buf);
                else if(sizeCompare==-1 && st.size <size)
                    printf(1, "%s\n", buf);
                else if(sizeCompare==0 && st.size == size)
                    printf(1, "%s\n", buf);
            }
            else
                printf(1, "%s\n", buf);

      }
    }
  }
}

void
find(char *path, char *search, char *type, int size, int sizeCompare)
{
  char buf[512];
  int fd; // Fd of path passed in
  struct stat st; // Details about path

  // Path must be a directory
  if((fd = open(path, 0)) < 0){
    printf(2, "find: cannot open %s\n", path);
    return;
  }

  // Path must have stats
  if(fstat(fd, &st) < 0){
    printf(2, "find: cannot stat %s\n", path);
    close(fd);
    return;
  }

  if (st.type == T_DIR) {
    searchDir(path, buf, fd, st, search, type, size, sizeCompare);
  }
  close(fd);
}

int
main(int argc, char *argv[])
{
  int i;
  if(argc < 2){
    printf(1, "find: requires a file name to search for\n");
  } else if (argc == 2) { // If no starting-point specified, '.' is assumed
    find(".", argv[1], "x", -1, 0);
  } 
  else if(strcmp(argv[4], "-type")==0 || strcmp(argv[4], "-size")==0){
      if(strcmp(argv[4], "-type")==0){
        find(argv[1], argv[3], argv[5], -1, 0);
      }
      else if(strcmp(argv[4], "-size")==0){
        char *size = argv[5];
        if(size[0]=='+') 
        {
            size++;
            find(argv[1], argv[3], "x", atoi(size), 1);//todo
        }
        else if(size[0]=='-')
        {
            size++;
            find(argv[1], argv[3], "x", atoi(size), -1);//todo
        }
        else
            find(argv[1], argv[3], "x", atoi(argv[5]), 0);//todo
      }
  }else {
    for (i = 3; i < argc; i++) {
      find(argv[1], argv[i], "x", -1, 0);
    }
  }
  exit();
}
