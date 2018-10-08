#include "klib.h"
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

static unsigned int out_d(char *out, int value){
  unsigned char stack[10];
	unsigned char length=0;
	char * tmp = out;
	if (value<0){
			*(out++)='-';
			value *= -1;
	}
	do {
		stack[length++] = '0' + (value%10);
		value/=10;
	} while (value);

	while (length){
		*out++ = stack[length-1];
		length--;
	}

	return out - tmp;
}

int printf(const char *fmt, ...) {
  char buf[1024];
	va_list args;
	int ret;

	va_start(args, fmt);
  ret = sprintf(buf, fmt, args);
	va_end(args);

	int len = strlen(buf);
  for (int i=0;i<len;++i)
			_putc(buf[i]);

  return ret;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  return 0;
}

int sprintf(char *out, const char *fmt, ...) {
  va_list ap;
	char c,ch;
	int v;
	char *p;

	va_start(ap, fmt);
	while ((c=*fmt++)){
			if (c!='%') *(out++)=c;
			else {
					c=*fmt++;
					switch(c){
							case 'c':
									ch = va_arg(ap, int);
								  *(out++)=ch;
									break;
							case 's':
									p = va_arg(ap, char *);
									strcpy(out,p);
									out += strlen(p);
									break;
							case 'd':
									v = va_arg(ap, int);
									out += out_d(out, v);
								  break;
							default:
									assert(0);
					}
			}
	}
	*out = '\0';
	va_end(ap);
  return 0;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  return 0;
}

#endif
