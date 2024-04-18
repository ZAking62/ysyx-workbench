#include <NDL.h>
#include <SDL.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

#define keyname(k) #k,

static const char *keyname[] = {
  "NONE",
  _KEYS(keyname)
};

static uint8_t key_state[sizeof(keyname) / sizeof(keyname[0])] = {0};
static char key_buf[64], *key_action, *key_key;
static int inline read_keyinfo(uint8_t *type, uint8_t *sym){
  int ret = NDL_PollEvent(key_buf, sizeof(key_buf));
  if (!ret){
    return 0;
  }
  //printf("%s\n", key_buf);
  key_action = key_buf;
  int i;
  for (i = 0; key_buf[i] != ' '; i++){}
  key_buf[i] = '\0';
  key_key = &key_buf[i + 1]; 
  
  //截断\n
  for (i = 0;  key_key[i] != '\0' && key_key[i] != '\n'; i++){}
  if (key_key[i] == '\n'){
    key_key[i] = '\0';
  }
  
  //strcmp("kd", key_action) == 0
  if (key_action[1] == 'd'){//加速！！
    *type = SDL_KEYDOWN;
  }else{
    *type = SDL_KEYUP;
  }

  for (i = 0; i < sizeof(keyname) / sizeof(char *); ++i){
    //剪枝掉很多
    if (key_key[0] == keyname[i][0] && strcmp(key_key, keyname[i]) == 0){
      *sym = i;
      //printf("%d %d\n", *type, *sym);
      return ret;
    }
  }
}

int SDL_PushEvent(SDL_Event *ev) {
  return 0;
}

int SDL_PollEvent(SDL_Event *ev) {
  uint8_t type = 0, sym = 0;
  // SDL_PumpEvents();


  // if (pop(&type, &sym)){
  //printf("SDL_PollEvent\n");
  if (read_keyinfo(&type, &sym)){
    ev->type = type;
    ev->key.keysym.sym = sym;

    switch(type){
    case SDL_KEYDOWN:
      key_state[sym] = 1;
      //printf("%d Down\n", (int)sym);
      break;
    
    case SDL_KEYUP:
      key_state[sym] = 0;
      //printf("%d Up\n", (int)sym);
      break;
    }
  }else {
    return 0;
  }

  return 1;
}

int SDL_WaitEvent(SDL_Event *event) {
  uint8_t type = 0, sym = 0;
  //SDL_PumpEvents();

  //while (!pop(&type, &sym)){
  while (!read_keyinfo(&type, &sym)){
    //SDL_PumpEvents();
  }
  
  event->type = type;
  event->key.keysym.sym = sym;

  switch(type){
    case SDL_KEYDOWN:
      key_state[sym] = 1;
      break;
    
    case SDL_KEYUP:
      key_state[sym] = 0;
      break;
  }
  return 1;
}

int SDL_PeepEvents(SDL_Event *ev, int numevents, int action, uint32_t mask) {
  return 0;
}

uint8_t* SDL_GetKeyState(int *numkeys) {
  return NULL;
}
