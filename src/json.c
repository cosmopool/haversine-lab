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

static void psError(const char *fmt, ...) {
  if (SUPPRESS_ERRORS) return;
  va_list args;
  va_start(args, fmt);
  fprintf(stderr, "%s", "[ERROR] ");
  vfprintf(stderr, fmt, args);
  fprintf(stderr, "\n");
  fflush(stderr);
  va_end(args);
}

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
  if (p->cursor < p->len) {
    p->cursor++;
    p->line_offset++;
  }
  u8 c = psCurrent(*p);
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
//   psError("invalid object: expected '%c' got '%c' at %d:%d\n", exp, next, p.line, p.line_offset);
//   return false;
// }

static bool psEquals(u8 actual, u8 exp) {
  if (exp == actual) return true;
  psError("invalid object: expected '%c' got '%c'.\n", exp, actual);
  return false;
}

static void psConsumeWhitespace(Parser *p) {
  while (p->cursor < p->len) {
    u8 current = psCurrent(*p);
    bool is_space = current == ' ' ||
                    current == '\n' ||
                    current == '\r' ||
                    current == '\t';
    if (!is_space) break;
    p->cursor++;
    p->line_offset++;
  }
}

static bool psConsumeString(Parser *p) {
  u8 current = psCurrent(*p);
  ASSERT(current == '"', "string must start with open \"");
  while ((current = psConsume(p)) != '\0' && current != '"') {
    if (isalpha(current) || isdigit(current)) {
      continue;
    }

    else if (iscntrl(current)) {
      psError("invalid string: control character '%c' at %d:%d.\n", current, p->line, p->line_offset);
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
            psError("invalid string: invalid hex code '%c' at %d:%d.\n", current, p->line, p->line_offset);
            return false;
          }
        }
        break;

      default:
        psError("invalid string: unsupported escape '%c' at %d:%d.\n", current, p->line, p->line_offset);
        break;
      }
    }

    else {
      psError("invalid string: missing closing '\"' at %d:%d", p->line, p->line_offset);
      return false;
    }
  }
  if (current != '"') {
    psError("invalid string: missing closing '\"' at %d:%d", p->line, p->line_offset);
    return false;
  }
  psConsume(p);
  return true;
}

static bool psConsumeExponent(Parser *p, u32 beginning) {
  (void)beginning;
  u32 start = p->cursor;
  u32 end = start;
  u8 current = psConsume(p);
  while (current != '}' && current != ',' && !isspace(current)) {
    if (current == 'e' || current == 'E') {
      psError("%d:%d:invalid number: exponent cannot have more then one 'e' character", p->line, p->line_offset);
      return false;
    }

    else if (isdigit(current)) {
      // ignore and go to next iteration
    }

    else if (current == '+' || current == '-') {
      u8 next = psPeek(*p);
      if (!isdigit(next)) {
        psError("%d:%d:invalid number: exponent cannot end with sign '%c'", p->line, p->line_offset, current);
        return false;
      }
    }

    else {
      psError("%d:%d:invalid number: forbidden character in exponent '%c'", p->line, p->line_offset, current);
      return false;
    }

    current = psConsume(p);
    end = p->cursor;
  }
  if (end == start) {
    psError("%d:%d:invalid number: exponent must have one or more digits after 'e' character", p->line, p->line_offset);
    return false;
  }
  return true;
}

static bool psConsumeFraction(Parser *p, u32 beginning) {
  (void)beginning;
  u32 start = p->cursor;
  u32 end = start;
  u8 current = psConsume(p);
  while (current != '}' && current != ',' && !isspace(current)) {
    if (current == '.') {
      psError("%d:%d:invalid number: fraction cannot have more then one '.'", p->line, p->line_offset);
      return false;
    }

    else if (isdigit(current)) {
      // ignore and go to next iteration
    }

    else if (current == 'e' || current == 'E') {
      return psConsumeExponent(p, beginning);
    }

    else {
      psError("%d:%d:invalid number: forbidden character in fraction '%c'", p->line, p->line_offset, current);
      return false;
    }

    current = psConsume(p);
    end = p->cursor;
  }
  if (end == start) {
    psError("%d:%d:invalid number: fraction must have one or more digits after '.'", p->line, p->line_offset);
    return false;
  }
  return true;
}

static bool psConsumeNumber(Parser *p) {
  u32 start = p->cursor;
  u32 end = start;
  u8 current = psCurrent(*p);
  while (current != '}' && current != ',' && !isspace(current)) {
    if (current == '0') {
      u8 next = psPeek(*p);
      if (isdigit(next)) return false;
    }

    // integer case
    else if (isdigit(current)) {
    }

    // fraction case
    else if (current == '.') {
      return psConsumeFraction(p, start);
    }

    // exponent case
    else if (current == 'e' || current == 'E') {
      return psConsumeExponent(p, start);
    }

    else if (current == '-') {
      if (end != start) {
        psError("%d:%d:invalid number: negative sign is only valid at the beginning of a number", p->line, p->line_offset);
        return false;
      }
      u8 next = psPeek(*p);
      if (!isdigit(next)) {
        psError("%d:%d:invalid number: digits are expected after '-' character for a number", p->line, p->line_offset);
        return false;
      }
    }

    else {
      return false;
    }
    current = psConsume(p);
    end = p->cursor;
  }
  // TODO: remove once start actually parsing the fraction
  (void)start;
  (void)end;
  return true;
}

bool jsonParse(String json) {
  Parser p = {.data = (u8 *)json.data, .len = json.len, .cursor = 0};

  // must be at least an empty object '{}'
  if (p.len < 2) return false;
  psConsumeWhitespace(&p);
  if (!psEquals(psCurrent(p), '{')) return false;

  u8 current = {0};
  while ((current = psCurrent(p)) != '\0') {
    switch (current) {
    // parse a key
    case '"':
      if (!psConsumeString(&p)) return false;
      psConsumeWhitespace(&p);
      if (!psEquals(psCurrent(p), ':')) return false;
      break;

    // parse a value
    case ':':
      psConsume(&p);
      psConsumeWhitespace(&p);
      switch (psCurrent(p)) {
      case '"':
        if (!psConsumeString(&p)) return false;
        break;

      case '-':
      case '0' ... '9':
        if (!psConsumeNumber(&p)) return false;
        break;

      default: return false;
      }
      psConsumeWhitespace(&p);
      u8 current = psCurrent(p);
      if (psEquals(current, '}')) break;
      if (!psEquals(current, ',')) return false;
      break;

    case '}':
      psConsume(&p);
      psConsumeWhitespace(&p);
      if (p.cursor != p.len) return false;
      return true;

    case '{':
    case ',':
      psConsume(&p);
      psConsumeWhitespace(&p);
      break;
    }
  }

  if ((psConsume(&p)) != '\0') return false;
  if (p.data[p.cursor - 1] != '}') return false;
  ASSERT(p.cursor = p.len, "cursor should be truncated by len");

  return true;
}
