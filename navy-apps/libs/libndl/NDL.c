#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <assert.h>
#include <fcntl.h>

static int evtdev = -1;
static int fbdev = -1;
static int screen_w = 0, screen_h = 0;
static int canvas_w = 0, canvas_h = 0;
static int canvas_x = 0, canvas_y = 0;

uint32_t NDL_GetTicks() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return tv.tv_usec / 1000 + tv.tv_sec * 1000;
}

// 读出一条事件信息到buf中, 最长len字节
int NDL_PollEvent(char *buf, int len) {
  int fd = open("/dev/events", 0, 0);
  int ret = read(fd, buf, len);
  close(fd);
  if(ret == 0){
    return 0;
  }
  return 1;
}

void NDL_OpenCanvas(int *w, int *h) {
  //printf("NDL get screen width = %d, height = %d\n", screen_w, screen_h);
  
  //如果*w和*h均为0, 则将系统全屏幕作为画布, 并将*w和*h分别设为系统屏幕的大小
  if (*w == 0 && *h == 0) {
    *w = screen_w;
    *h = screen_h;
  }
  // 打开一张(*w) X (*h)的画布
  canvas_w = *w;
  canvas_h = *h;
  //居中
  canvas_x= (screen_w - canvas_w) / 2;
  canvas_y= (screen_h - canvas_h) / 2;

  if (getenv("NWM_APP")) {
    int fbctl = 4;
    fbdev = 5;
    screen_w = *w; screen_h = *h;
    char buf[64];
    int len = sprintf(buf, "%d %d", screen_w, screen_h);
    // let NWM resize the window and create the frame buffer
    write(fbctl, buf, len);
    while (1) {
      // 3 = evtdev
      int nread = read(3, buf, sizeof(buf) - 1);
      if (nread <= 0) continue;
      buf[nread] = '\0';
      if (strcmp(buf, "mmap ok") == 0) break;
    }
    close(fbctl);
  }
}

// 向画布`(x, y)`坐标处绘制`w*h`的矩形，像素按行优先方式存储在`pixels`中
void NDL_DrawRect(uint32_t *pixels, int x, int y, int w, int h) {
  int fd = open("/dev/fb", 0, 0);
  for (int i = 0; i < h && y + i < canvas_h; i++) {
    // 调整到居中位置 canvas_x  canvas_y 居中   一个像素4byte
    lseek(fd, ((y + canvas_y + i) * screen_w + (x + canvas_x)) * 4, SEEK_SET);
    write(fd, pixels + i * w, 4 * (w < canvas_w - x ? w : canvas_w - x));
  }
  close(fd);
}

void NDL_OpenAudio(int freq, int channels, int samples) {
}

void NDL_CloseAudio() {
}

int NDL_PlayAudio(void *buf, int len) {
  return 0;
}

int NDL_QueryAudio() {
  return 0;
}

int NDL_Init(uint32_t flags) {

  //dispinfo read screen width and height
  int buf_size = 1024; 
  char *buf = (char *)malloc(buf_size * sizeof(char));
  int fd = open("/proc/dispinfo", 0, 0);
  int ret = read(fd, buf, buf_size);
  close(fd);

  sscanf(buf, "WIDTH:%d\nHEIGHT:%d\n", &screen_w, &screen_h);
  
  if (getenv("NWM_APP")) {
    evtdev = 3;
  }
  return 0;
}

void NDL_Quit() {
}
