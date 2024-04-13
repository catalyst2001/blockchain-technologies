#include <Windows.h>
#include <stdio.h>
#include <time.h>

volatile DWORD WINAPI thread_proc(LPVOID p_param)
{
  printf("thread started\n");
  while (1) {
    printf("hello world!\n");
  }
  return 0;
}

int main()
{
  HANDLE h_thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&thread_proc, NULL, 0, NULL);

  printf(
    "thread started and call function time()\n"
    "press any key to terminate thread:\n"
  );

  Sleep(100);
  printf("call next printf() for demonstrate deadlock\n");
  SuspendThread(h_thread);
  //TerminateThread(h_thread, 1);
  printf("*** this text will never come out ***\n");
  return 0;
}