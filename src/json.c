#include "core.h"

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>

/*
 * Error logging toggle (compile-time).
 *
 * When false (default), parser failures print a diagnostic to stderr.
 * When true, parsing stays silent and the caller must check the
 * `bool` return of jsonParse().
 *
 * Override macros must be defined BEFORE "json.c" #include line
 * or via compiler flag:
 *   cc ... -DSUPPRESS_ERRORS=true ...
 */
#ifndef SUPPRESS_ERRORS
#define SUPPRESS_ERRORS false
#endif

typedef struct {
  u8 *data;
  u32 len;
  u32 cursor;
  u32 line;
  u32 line_offset;
} Parser;

static u8 psCurrent(Parser p) {
  if (p.cursor >= p.len) return '\0';
  u8 c = p.data[p.cursor];
  return c;
}

static u8 psPeek(Parser p) {
  u32 next = p.cursor + 1;
  if (next >= p.len) return '\0';
  u8 c = p.data[next];
  return c;
}

static u8 psConsume(Parser *p) {
  u8 c = psCurrent(*p);
  if (p->cursor < p->len) {
    p->cursor++;
    p->line_offset++;
  }
  if (c == '\n') {
    p->line_offset = 0;
    // skip consecutive newlines
    while (psPeek(*p) == '\n') {
      p->line++;
      p->cursor++;
    }
  }
  return c;
}

// static bool psExpectNext(Parser p, u8 exp) {
//   u8 next = psPeek(p);
//   if (exp == next) return true;
//   if (!SUPPRESS_ERRORS) {
//     fprintf(stderr, "ERROR: invalid object: expected '%c' got '%c' at %d:%d\n", exp, next, p.line, p.line_offset);
//   }
//   return false;
// }

static bool psEquals(u8 actual, u8 exp) {
  if (exp == actual) return true;
  if (!SUPPRESS_ERRORS) {
    fprintf(stderr, "ERROR: invalid object: expected '%c' got '%c'.\n", exp, actual);
  }
  return false;
}

static void psConsumeWhitespace(Parser *p) {
  while (isspace(psCurrent(*p))) {
    p->cursor++;
    p->line_offset++;
  }
}

static bool psConsumeString(Parser *p) {
  u8 current = psCurrent(*p);
  psEquals(current, '"');
  while ((current = psConsume(p)) != '\0' && current != '"') {
    if (isalpha(current)) {
      continue;
    }

    else if (iscntrl(current)) {
      if (!SUPPRESS_ERRORS) {
        fprintf(stderr, "ERROR: invalid string: control character '%c' at %d:%d.\n", current, p->line, p->line_offset);
      }
      return false;
    }

    else if (current == '\\') {
      switch (current = psConsume(p)) {
      case '\"':
      case '\\':
      case '/':
      case '\b':
      case '\f':
      case '\n':
      case '\r':
      case '\t':
        break;

      case 'u':
        for (u32 i = 0; i < 4; i++) {
          switch (current = psConsume(p)) {
          case 'A' ... 'F':
          case 'a' ... 'f':
          case '0' ... '9':
            continue;

          default:
            // if (!SUPPRESS_ERRORS) {
              fprintf(stderr, "ERROR: invalid string: invalid hex code '%c' at %d:%d.\n", current, p->line, p->line_offset);
            // }
            return false;
          }
        }
        break;

      default:
        if (!SUPPRESS_ERRORS) {
          fprintf(stderr, "ERROR: invalid string: unsupported escape '%c' at %d:%d.\n", current, p->line, p->line_offset);
        }
        break;
      }
    }
  }
  return true;
}

bool jsonParse(String json) {
  Parser p = {.data = (u8 *)json.data, .len = json.len, .cursor = 0};

  psConsumeWhitespace(&p);
  u8 initial = psConsume(&p);
  if (!psEquals(initial, '{')) return false;

  u8 current = initial;
  while (current != '\0' && current != '}') {
    psConsumeWhitespace(&p);
    current = psConsume(&p);
    switch (current) {
    // parse a key
    case '"':
      if (!psConsumeString(&p)) return false;
      psConsumeWhitespace(&p);
      current = psConsume(&p);
      if (!psEquals(current, ':')) return false;
      break;

    case ':':
      break;
    }
  }
  if (current != '}') return false;

  psConsumeWhitespace(&p);
  if ((current = psConsume(&p)) != '\0') return false;
  if (p.cursor != p.len) return false;

  return true;
}
