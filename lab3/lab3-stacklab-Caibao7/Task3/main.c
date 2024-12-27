#include "assert.h"
#include "context.h"
#include "stdio.h"
#include "stdlib.h"
#include <unistd.h>

extern __generator* __now_gen;

void test1(int x){
    static const char* str = "test1 PASSED\n";
    static const int len = 13;
    static __ctx ctx[14];
    static int p = 0;
    while (p < len)
    {
        if (x > len)
        {
            p++;
            __ctx_restore(&ctx[p], p);
        }
        int t = __ctx_save(&ctx[x]);
        if (t == 0)
        {
            test1(x + 1);
        }
        else
        {
            assert(t == x);
            printf("%c", str[x - 1]);
            fflush(stdout);
        }
    }
}

void test2(__ctx* last, int depth)
{
  static int cnt;
  __ctx ctx;
  while (++cnt)
  {
    if ((((rand() & 15) > 6) || depth < 20) && depth < 100)
    {
      int t = __ctx_save(&ctx);
      if (t == 0)
      {
        test2(&ctx, depth + 1);
      }
      else
      {
        assert(depth < t && t <= 100);
        if (t == 100)
        {
          assert(cnt > 200 && cnt < 2000);
          if (last)
            __ctx_restore(last, t);
          else
            printf("test2 PASSED\n");
          break;
        }
      }
    }
    else
    {
      __ctx_restore(last, depth);
      assert(0);
    }
  }
}

void test3(int x)
{
  static const char* str = "test3 PASSED\n";
  static const int len = 13;
  if (x == len)
  {
    throw(str[0]);
    assert(0);
  }
  try
  {
    test3(x + 1);
  }
  catch
  {
    printf("%c", __err_try);
    if (x > 0)
    {
      throw(str[len - x]);
      assert(0);
    }
    else
    {
      assert(__check_err_stk());
    }
  }
}

void test4(int x)
{
  static int cnt;
  while (++cnt < 10000)
  {
    if (rand() % 100 >= x)
    {
      try
      {
        test4(x + 1);
        if (cnt < 10000)
          continue;
      }
      catch
      {
        assert(__err_try == x);
      }
      assert(cnt <= 10000);
      if (cnt == 10000)
        break;
    }
    else if (rand() & 1) {
      throw(x - 1);
    }
    else
      break;
  }
  if (x == 0)
  {
    assert(__check_err_stk());
    printf("test4 PASSED\n");
  }
}

void test5()
{
  try{
    try{
      try{
        printf("test");
        throw(233);
        assert(0);
      }
      catch{
        assert(__err_try == 233);
        printf("5");
        throw(344);
        assert(0);
      }
    }
    catch{
      assert(__err_try == 344);
      printf(" ");
      throw(455);
      assert(0);
    }
  }
  catch{
    assert(__err_try == 455);
    printf("PASSED\n");
  }
}

void test6(int x)
{
  static const char* str = "test6 PASSED\n";
  static const int len = 13;
  if (x == 0)
  {
    __generator* g = generator(test6, 1);
    while (1)
    {
      try
      {
        printf("%c", send(g, 233));
      }
      catch
      {
        assert(__err_try == ERR_GENEND);
        break;
      }
    }
    generator_free(&g);
  }
  else
  {
    assert(x == 1);
    for (int i = 0; i < len; i++)
      assert(yield(str[i]) == 233);
  }
  assert(__check_err_stk());
}

void test7(int x)
{
  static int cnt;
  __generator* g = NULL;
  while (1)
  {
    if (rand() % 100 >= x)
    {
      try
      {
        assert(cnt == send(g, x + 1));
        assert(cnt <= 10000);
        if (cnt == 10000)
        {
          throw(ERR_GENEND);
          assert(0);
        }
        continue;
      }
      catch
      {
        if (__err_try == ERR_GENNIL)
        {
          g = generator(test7, x + 1);
        }
        else if (__err_try == ERR_GENEND)
        {
          break;
        }
        else
        {
          assert(__err_try == x * 2 + 1);
          generator_free(&g);
        }
      }
    }
    else if (rand() & 3)
    {
      assert(yield(++cnt) == x);
    }
    else
    {
      throw(x * 2 - 1);
      assert(0);
    }
  }
  generator_free(&g);
  assert(__check_err_stk());
  if (x == 0)
    printf("test7 PASSED\n");
}

void test8(int x) {
  __generator* g = 0; 
  if (x == 0) {
    try {
      g = generator(test8, x+1);
      assert(send(g, 1) == (x+1) / (9-x));
      send(g, 0);
    }
    catch {
      assert(__err_try == ERR_DIV0);
      printf("test8 PASSED\n");
      generator_free(&g);
      return;
    }
  }
  else {
    if (10 - x != 0) {
      yield(x / (10-x));
    }
    else{
      throw(ERR_DIV0);
    }
    g = generator(test8, x + 1);
    assert(send(g, 1) == (x+1) / (9-x));
    send(g, 0);
  }
}

void progress_bar(int init){
  int progress = init;
  while(1){
    printf("\r["); //开始绘制进度条

        for (int i = 0; i < progress; i++) {
            printf("\033[32m=\033[0m"); //绿色
        }
        printf("\033[32m>>\033[0m");
        for (int i = progress; i < 63; i++) {
            printf("#");
        }

        printf("] \033[33m%3d%%\033[0m ", (progress * 100) / 63);//黄色
        if (progress % 4 == 0 && progress != 63) {
            printf("       "); //动态waiting
        } else if(progress % 4 != 0 && progress != 63){
            printf("\033[31mwaiting \033[0m"); //红色
        } else if(progress == 63){
            printf("\033[32mfinished\033[0m");
        }
    fflush(stdout);
    progress = yield(progress);
    if (progress == 64) break;
  }
}

void test_progress_bar(int x){
  __generator *g = generator(progress_bar, x);
  try {
    int now = 0;
    while(1){
      now = send(g, now);
      usleep(40000);
      now += 1;
    }
  }
  catch {
    assert(__err_try == ERR_GENEND);
    printf("\nprogress bar SHOWN\n");
  }
}

int main(){
    test1(1);
    test2(NULL, 0);
    test3(0);
    test4(0);
    test5();
    test6(0);
    test7(0);
    test8(0);
    test_progress_bar(0);
    {
      try{
      }
      catch{
        printf("test FAILED on main\n");
      }
    }
    assert(__check_err_stk());
    return 0;
}