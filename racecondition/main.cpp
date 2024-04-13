#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <stdio.h>

#define NUM_THREADS MAXIMUM_WAIT_OBJECTS

long global_var;

#define THREAD_SAFE

DWORD WINAPI thread_proc(LPVOID p_param)
{
#ifdef THREAD_SAFE
  InterlockedIncrement(&global_var);
#else
  global_var++; // no-atomic increment for demonstrate race condition
#endif
  //printf("value incremented=%d  from thread %zd\n", global_var, (size_t)p_param);
  return 0;
}

#define CNT(x) (sizeof(x) / sizeof(x[0]))

const size_t num_threads = NUM_THREADS;

int main()
{
  SYSTEM_INFO sysinfo;
  size_t      i, num_tests;
  HANDLE      h_threads[num_threads];

  GetSystemInfo(&sysinfo);

  while (1) {
    printf("type num tests: ");
    scanf_s("%zd", &num_tests);
    for (i = 0; i < num_tests; i++) {
      InterlockedExchange(&global_var, 0); //reset var value
      for (size_t j = 0; j < num_threads; j++) {
        h_threads[j] = CreateThread(NULL, 0, &thread_proc, (LPVOID)j, 0, NULL);
        SetThreadAffinityMask(h_threads[j], (DWORD_PTR)((1 << (j % sysinfo.dwNumberOfProcessors))));
      }

      printf("%zd threads created (pass %zd)\n", num_threads, i);
      if (WaitForMultipleObjects((DWORD)num_threads, h_threads, TRUE, INFINITE) == WAIT_FAILED) {
        printf("error: %d\n", GetLastError());
        return 0;
      }

      if (NUM_THREADS != global_var) {
        printf("race condition!!  executed %d threads but the value is %d\n", NUM_THREADS, global_var);
        break;
      }
    }
    printf("[ %s ] tests passed %zd/%zd ---\n", (i == num_tests) ? "PASSED" : "NOT PASSED", i, num_tests);
  }
  return 0;
}