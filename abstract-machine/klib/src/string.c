#include <klib.h>
#include <klib-macros.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
	if(s == NULL){
		return 0;
	}
	size_t n = 0;
	while(s[n] != '\0'){
		n++;
	}
	return n;
}

char *strcpy(char *dst, const char *src) {
	if(dst == NULL || src == NULL){
		return NULL;
	}
	char* res = dst;
	while(*src != '\0'){
		*dst = *src;
		dst++;
		src++;
	}
	*dst = *src;
	return res;
}

char *strncpy(char *dst, const char *src, size_t n) {
  size_t i;
	for(i = 0; i < n && src[i] != '\0'; i++){
		dst[i] = src[i];
	}
	for( ; i < n; i++){
		dst[i] = '\0';
	}
	return dst;
}

char *strcat(char *dst, const char *src) {
	char* res = dst;
	while(*dst != '\0'){
		dst++;
	}
	strcpy(dst, src);
	return res;
}

int strcmp(const char *s1, const char *s2) {
  size_t i = 0;
  while(s1[i] != '\0' && s2[i] != '\0'){
	  if(s1[i] > s2[i]){
		  return 1;
		}
	  if(s1[i] < s2[i]){
		  return -1;
		}
	  i++;
  }
  if(s1[i] != '\0' && s2[i] == '\0')
	  return 1;
  if(s1[i] == '\0' && s2[i] != '\0')
	  return -1;
  return 0;
}

int strncmp(const char *s1, const char *s2, size_t n) {
	size_t i = 0;
	while(i < n){
	  if(s1[i] > s2[i]){
		  return 1;
		}
	  if(s1[i] < s2[i]){
		  return -1;
		}
	  i++;
  }
  return 0;
}

void *memset(void *s, int c, size_t n) {
	char* ch = (char*) s;
	size_t i = 0;
	while(i < n){
		ch[i++] = c;
	}
	return s;
}

void *memmove(void *dst, const void *src, size_t n) {
	size_t i;
	char* d = (char*)dst;
	char* s = (char*)src;
	if(dst < src){
		i = 0;
		while(i < n){
			d[i] = s[i];
			i++;
		}
	}
	else{
		i = n - 1;
		while(i >= 0){
			d[i] = s[i];
			i--;
		}
	}
	return dst;
}

void *memcpy(void *out, const void *in, size_t n) {
	char* ch_out = (char*)out;
	char* ch_in = (char*)in;
	size_t i = 0;
	while(i < n){
		ch_out[i] = ch_in[i];
		i++;
	}
	return out;
}

int memcmp(const void *s1, const void *s2, size_t n) {
	if(n == 0)
		return 0;
	size_t i = 0;
	char* ch_s1 = (char*)s1;
	char* ch_s2 = (char*)s2;
	while(i < n){
		if(ch_s1[i] < ch_s2[i]){
			return -1;
		}
		if(ch_s1[i] > ch_s2[i]){
			return 1;
		}
		i++;
	}
	return 0;
}

#endif
