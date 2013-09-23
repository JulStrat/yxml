/* Copyright (c) 2013 Yoran Heling

  Permission is hereby granted, free of charge, to any person obtaining
  a copy of this software and associated documentation files (the
  "Software"), to deal in the Software without restriction, including
  without limitation the rights to use, copy, modify, merge, publish,
  distribute, sublicense, and/or sell copies of the Software, and to
  permit persons to whom the Software is furnished to do so, subject to
  the following conditions:

  The above copyright notice and this permission notice shall be included
  in all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
  CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
  TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
  SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <yxml.h>
#include <stdio.h>
#include <stdlib.h>


static char stack[8*1024];
static int verbose = 0;


static void y_printchar(char c) {
	if(c == '\x7F' || (c >= 0 && c < 0x20))
		printf("\\x%02x", c);
	else
		printf("%c", c);
}


static void y_printstring(const char *str) {
	while(*str) {
		y_printchar(*str);
		str++;
	}
}


static void y_printtoken(yxml_t *x, const char *str) {
	puts("");
	if(verbose)
		printf("t%03lu l%03u b%03lu: ", x->total, x->line, x->byte);
	printf("%s", str);
}


static void y_printres(yxml_t *x, yxml_ret_t r) {
	static int indata;
	switch(r) {
	case YXML_OK:
		if(verbose) {
			y_printtoken(x, "ok");
			indata = 0;
		}
		break;
	case YXML_ELEMSTCONT:
	case YXML_ELEMSTART:
	case YXML_CONTENT:
		if(r & YXML_ELEMSTART) {
			y_printtoken(x, "elemstart ");
			y_printstring(x->elem);
		}
		if(r & YXML_CONTENT)
			y_printtoken(x, "content");
		break;
	case YXML_ELEMEND:
		y_printtoken(x, "elemend");
		break;
	case YXML_ATTRSTART:
		y_printtoken(x, "attrstart ");
		y_printstring(x->attr);
		break;
	case YXML_ATTREND:
		y_printtoken(x, "attrend");
		break;
	case YXML_DATA:
		if(!indata) {
			y_printtoken(x, "data ");
			indata = 1;
		}
		y_printchar(x->data);
		break;
	case YXML_PISTART:
		y_printtoken(x, "pistart ");
		y_printstring(x->pi);
		break;
	case YXML_PIEND:
		y_printtoken(x, "piend");
		break;
	default:
		y_printtoken(x, "error\n");
		exit(0);
	}
	if(r != YXML_OK && r != YXML_DATA)
		indata = 0;
}


int main(int argc, char **argv) {
	int c;
	yxml_ret_t r;
	yxml_t x[1];
	yxml_init(x, stack, sizeof(stack));

	verbose = argc > 1;

	while((c = getc(stdin)) != EOF) {
		r = yxml_parse(x, c);
		y_printres(x, r);
	}

	y_printtoken(x, yxml_eof(x) < 0 ? "error\n" : "ok\n");
	return 0;
}

/* vim: set noet sw=4 ts=4: */
