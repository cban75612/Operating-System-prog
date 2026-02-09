#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <random>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/file.h>
#include <sys/mman.h>
using namespace std;

void error_and_die(const char *msg)
{
  perror(msg);
  exit(EXIT_FAILURE);
}
int main(int argc, char *argv[])
{
  // 建立子行程
  bool hita=false;
  bool hitb=false;
  int wstatus;
  int p1, p2, advance;
  p1 = stoi(argv[1], nullptr, 0);
  p2 = stoi(argv[2], nullptr, 0);
  advance = stoi(argv[3], nullptr, 0);
  int location11X, location11Y, location12X, location12Y, location21X, location21Y, location22X, location22Y;
  int ParentBombX, ParentBombY, ChildBombX, ChildBombY;
  int BombCount = 0;
  int r;

  const char *memname = "sample";
  const size_t region_size = sysconf(_SC_PAGE_SIZE);

  int fd = shm_open(memname, O_CREAT | O_TRUNC | O_RDWR, 0666);
  if (fd == -1)
    error_and_die("shm_open");

  r = ftruncate(fd, region_size);
  if (r != 0)
    error_and_die("ftruncate");

  void *ptr = mmap(0, region_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (ptr == MAP_FAILED)
    error_and_die("mmap");
  close(fd);

  pid_t pid = fork();

  if (pid > 0)
  { // parent
    cout << "[" << getpid() << " Parent]: Random Seed " << p1 << endl;
    waitpid(pid, &wstatus, WUNTRACED);
    srand(p1);
    int temp = rand() % (1 - 0 + 1) + 0; //決定第一個座標的第二個位置
    if (temp == 0)
    { // X
      location11X = rand() % (2 - 0 + 1) + 0;
      location11Y = rand() % (3 - 0 + 1) + 0;
      location12X = location11X + 1;
      location12Y = location11Y;
    }
    else
    { // Y
      location11X = rand() % (3 - 0 + 1) + 0;
      location11Y = rand() % (2 - 0 + 1) + 0;
      location12X = location11X;
      location12Y = location11Y + 1;
    }
    cout << "[" << getpid() << " Parent]: The gunboat:"
         << "(" << location11X << "," << location11Y << ")"
         << "(" << location12X << "," << location12Y << ")" << endl;
    kill(getpid() + 1, SIGCONT);
    waitpid(pid, &wstatus, WUNTRACED);
    while (hita==false||hitb==false)
    {
      //炸射
      srand(ChildBombX + BombCount);
      ParentBombX = rand() % (3 - 0 + 1) + 0;
      ParentBombY = rand() % (3 - 0 + 1) + 0;

      int ParentBomb = ParentBombX * 10 + ParentBombY;
      cout << "[" << getpid() << " Parent]:"
           << " bombing:"
           << "(" << ParentBombX << "," << ParentBombY << ")" << endl;
      // shm
      u_long *d = (u_long *)ptr;
      *d = ParentBomb;
      kill(getpid() + 1, SIGCONT);
      waitpid(pid, &wstatus, WUNTRACED);
      if(WIFEXITED(wstatus)){//if child process exit normaly
    
        exit(0);
      }
      //判斷是否擊中
      ChildBombX = *(u_long *)ptr / 10;
      ChildBombY = *(u_long *)ptr % 10;
      if (ChildBombX == location11X && ChildBombY == location11Y)
      {
        hita = true;
        if (hita == true && hitb == true)
        {
          cout << "[" << getpid() << " Parent]: hit and sinking" << endl;
          BombCount += 1;
          cout << "[" << getpid()+1 << " Child]:"
               << " wins with " << BombCount << " bombs" << endl;

          exit(0);
        }
        else
        {
          cout << "[" << getpid() << " Parent]: hit" << endl;
          BombCount += 1;
        }
      }
      else if (ChildBombX == location12X && ChildBombY == location12Y)
      {
        hitb = true;
        if (hita == true && hita == true)
        {
          cout << "[" << getpid() << " Parent]: hit and sinking" << endl;
          BombCount += 1;
          cout << "[" << getpid()+1 << " Child]:"
               << " wins with " << BombCount << " bombs" << endl;

          exit(0);
        }
        else
        {
          cout << "[" << getpid() << " Parent]: hit" << endl;
          BombCount += 1;
        }
      }
      else
      {
        cout << "[" << getpid() << " Parent]: missed" << endl;
        BombCount += 1;
      }
      
    }
  }
  else if (pid == 0)
  { // child
    cout << "[" << getpid() << " Child]: Random Seed " << p2 << endl;
    raise(SIGSTOP);
    srand(p2);
    int temp = rand() % (1 - 0 + 1) + 0; //決定第二個座標的第二個位置
    if (temp == 0)
    { // X
      location21X = rand() % (2 - 0 + 1) + 0;
      location21Y = rand() % (3 - 0 + 1) + 0;
      location22X = location21X + 1;
      location22Y = location21Y;
    }
    else
    { // Y
      location21X = rand() % (3 - 0 + 1) + 0;
      location21Y = rand() % (2 - 0 + 1) + 0;
      location22X = location21X;
      location22Y = location21Y + 1;
    }

    cout << "[" << getpid() << " Child]:"
         << " The gunboat:"
         << "(" << location21X << "," << location21Y << ")"
         << "(" << location22X << "," << location22Y << ")" << endl;
    raise(SIGSTOP);
    hita=false;
    hitb=false;
    //判斷是否擊中
    while (hitb==false||hita==false)
    {
      ParentBombX = *(u_long *)ptr / 10;
      ParentBombY = *(u_long *)ptr % 10;
      if (ParentBombX == location21X && ParentBombY == location21Y)
      {
        hita = true;
        if (hita == true && hitb == true)
        {
          cout << "[" << getpid() << " Child]: hit and sinking" << endl;
          BombCount += 1;
          cout << "[" << getpid()-1 << " Parent]:"
               << " wins with " << BombCount << " bombs" << endl;
          exit(0);
        }
        else
        {
          cout << "[" << getpid() << " Child]: hit" << endl;
          BombCount += 1;
        }
      }
      else if (ParentBombX == location22X && ParentBombY == location22Y)
      {
        hitb = true;
        if (hita == true && hitb == true)
        {
          cout << "[" << getpid() << " Child]: hit and sinking" << endl;
          BombCount += 1;
          cout << "[" << getpid()-1 << " Parent]:"
               << " wins with " << BombCount << " bombs" << endl;
          exit(0);
        }
        else
        {
          cout << "[" << getpid() << " Child]: hit" << endl;
          BombCount += 1;
        }
      }
      else
      {
        cout << "[" << getpid() << " Child]: missed" << endl;
        BombCount += 1;
      }

      //炸射

      srand(ParentBombX + BombCount);
      ChildBombX = rand() % (3 - 0 + 1) + 0;
      ChildBombY = rand() % (3 - 0 + 1) + 0;

      int ChildBomb = ChildBombX * 10 + ChildBombY;
      // shm
      u_long *a = (u_long *)ptr;
      *a = ChildBomb;
      cout << "[" << getpid() << " Child]:"
           << " bombing:"
           << "(" << ChildBombX << "," << ChildBombY << ")" << endl;
      raise(SIGSTOP);
    }
  }
}
