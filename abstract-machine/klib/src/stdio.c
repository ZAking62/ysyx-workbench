#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)
static char NUM_CHAR[] = "0123456789ABCDEF";

int printf(const char *fmt, ...) {
	char buffer[2048];
	va_list arg;
	va_start(arg, fmt);

	int done = vsprintf(buffer, fmt, arg);
	putstr(buffer);

	va_end(arg);
	return done;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
	return vsnprintf(out, -1, fmt, ap);
}

int sprintf(char *out, const char *fmt, ...) {
	va_list arg;
	va_start(arg, fmt);

	int done = vsprintf(out, fmt, arg);

	va_end(arg);
	return done;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
	va_list arg;
	va_start(arg, fmt);

	int done = vsnprintf(out, n, fmt, arg);

	va_end(arg);
	return done;
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
	int len = 0;
	char buf[128];
	int buf_len = 0;
	while(*fmt != '\0' && len < n){
		switch(*fmt){
			case '%':
				fmt++;
				switch(*fmt){
					case 'd':
						int val = va_arg(ap, int);
						if(val == 0){
							out[len++] = '0';
							break;
						}
						if(val < 0){
							out[len++] = '-';
							val = -val;
						}
						while(val != 0){
							buf[buf_len++] = NUM_CHAR[val % 10];
							val /= 10;
						}
						while(buf_len != 0){
							out[len++] = buf[--buf_len];
						}
						break;
					case 's':
						char *s = va_arg(ap, char*);
            for(int i = 0; s[i] != '\0'; i++){
                out[len++] = s[i];
						}
						break;
					case 'c':
						char m_s = va_arg(ap, int);//1
						out[len++] = m_s;
						break;
				}
				break;
			case '\n':
				out[len++] = '\n';
				break;
			default:
				out[len++] = *fmt;
		}
		fmt++;
	}
	out[len] = '\0';
	return len;
}

#endif
