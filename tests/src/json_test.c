// Disable greatest abbrevs: core.h already defines ASSERT().
#define GREATEST_USE_ABBREVS 0
#include "../vendor/greatest.h"

#define SUPPRESS_ERRORS true
#include "../../src/json.c"

// --- empty object: valid cases ---

GREATEST_TEST empty_object_bare_passes(void) {
  GREATEST_ASSERT_EQ(true, jsonParse(MCL_STRING("{}")));
  GREATEST_PASS();
}

GREATEST_TEST empty_object_with_inner_space_passes(void) {
  GREATEST_ASSERT_EQ(true, jsonParse(MCL_STRING("{ }")));
  GREATEST_ASSERT_EQ(true, jsonParse(MCL_STRING("{\n\t\r\n }")));
  GREATEST_PASS();
}

GREATEST_TEST empty_object_with_outer_whitespace_passes(void) {
  GREATEST_ASSERT_EQ(true, jsonParse(MCL_STRING("  {}\n")));
  GREATEST_ASSERT_EQ(true, jsonParse(MCL_STRING("\t{\n} \n")));
  GREATEST_PASS();
}

// --- empty object: invalid cases ---

GREATEST_TEST empty_input_fails(void) {
  // NOTE: can't use MCL_STRING("") — it asserts len > 0.
  String s = {.len = 0, .data = ""};
  GREATEST_ASSERT_EQ(false, jsonParse(s));
  GREATEST_PASS();
}

GREATEST_TEST unterminated_open_brace_fails(void) {
  GREATEST_ASSERT_EQ(false, jsonParse(MCL_STRING("{")));
  GREATEST_ASSERT_EQ(false, jsonParse(MCL_STRING("  {  ")));
  GREATEST_PASS();
}

GREATEST_TEST missing_open_brace_fails(void) {
  GREATEST_ASSERT_EQ(false, jsonParse(MCL_STRING("}")));
  GREATEST_ASSERT_EQ(false, jsonParse(MCL_STRING("[]")));
  GREATEST_ASSERT_EQ(false, jsonParse(MCL_STRING("x")));
  GREATEST_PASS();
}

GREATEST_TEST trailing_garbage_fails(void) {
  GREATEST_ASSERT_EQ(false, jsonParse(MCL_STRING("{}x")));
  GREATEST_ASSERT_EQ(false, jsonParse(MCL_STRING("{} {}")));
  GREATEST_PASS();
}

// --- newline collapsing ---

GREATEST_TEST parse_object_with_consecutive_newlines(void) {
  GREATEST_ASSERT_EQ(true, jsonParse(MCL_STRING("{\n}")));
  GREATEST_ASSERT_EQ(true, jsonParse(MCL_STRING("{\n\n\n}")));
  GREATEST_PASS();
}

// --- string: valid cases ---

GREATEST_TEST alphanumeric_string_passes(void) {
  String s = MCL_STRING("{\"abcd1234\": \"a1\"}");
  GREATEST_ASSERT_EQ(true, jsonParse(s));
  GREATEST_PASS();
}

GREATEST_TEST escape_in_key_passes(void) {
  GREATEST_ASSERT_EQ(true, jsonParse(MCL_STRING("{\"a\\\"b\": 1}")));
  GREATEST_ASSERT_EQ(true, jsonParse(MCL_STRING("{\"a\\\\\": 1}")));
  GREATEST_ASSERT_EQ(true, jsonParse(MCL_STRING("{\"a\\zb\": 1}")));
  GREATEST_PASS();
}

GREATEST_TEST escape_in_value_passes(void) {
  GREATEST_ASSERT_EQ(true, jsonParse(MCL_STRING("{\"a\": \"b\\\"c\"}")));
  GREATEST_PASS();
}

// --- value is string: valid cases ---

GREATEST_TEST plain_string_value_passes(void) {
  GREATEST_ASSERT_EQ(true, jsonParse(MCL_STRING("{\"a\": \"hello\"}")));
  GREATEST_ASSERT_EQ(true, jsonParse(MCL_STRING("{\"a\": \"a1\"}")));
  GREATEST_ASSERT_EQ(true, jsonParse(MCL_STRING("{\"a\": \"\"}")));
  GREATEST_ASSERT_EQ(true, jsonParse(MCL_STRING("{\"a\":\"b\"}")));
  GREATEST_ASSERT_EQ(true, jsonParse(MCL_STRING("{\"a\" : \"b\" }")));
  GREATEST_PASS();
}

GREATEST_TEST escape_sequences_in_value_passes(void) {
  GREATEST_ASSERT_EQ(true, jsonParse(MCL_STRING("{\"a\": \"b\\\\c\"}")));
  GREATEST_ASSERT_EQ(true, jsonParse(MCL_STRING("{\"a\": \"b\\/c\"}")));
  GREATEST_ASSERT_EQ(true, jsonParse(MCL_STRING("{\"a\": \"b\\nc\"}")));
  GREATEST_ASSERT_EQ(true, jsonParse(MCL_STRING("{\"a\": \"b\\tc\"}")));
  GREATEST_PASS();
}

GREATEST_TEST unicode_escape_in_value_passes(void) {
  GREATEST_ASSERT_EQ(true, jsonParse(MCL_STRING("{\"a\": \"\\u1234\"}")));
  GREATEST_ASSERT_EQ(true, jsonParse(MCL_STRING("{\"a\": \"\\u00e9\"}")));
  GREATEST_ASSERT_EQ(true, jsonParse(MCL_STRING("{\"a\": \"\\u00E9\"}")));
  GREATEST_ASSERT_EQ(true, jsonParse(MCL_STRING("{\"a\": \"\\uFFFF\"}")));
  GREATEST_PASS();
}

GREATEST_TEST multi_pair_string_values_pass(void) {
  GREATEST_ASSERT_EQ(true, jsonParse(MCL_STRING("{\"a\": \"b\", \"c\": \"d\"}")));
  GREATEST_PASS();
}

// --- value is string: invalid cases ---

GREATEST_TEST unterminated_string_value_fails(void) {
  GREATEST_ASSERT_EQ(false, jsonParse(MCL_STRING("{\"a\": \"abc}")));
  GREATEST_ASSERT_EQ(false, jsonParse(MCL_STRING("{\"a\": \"a\"")));
  GREATEST_ASSERT_EQ(false, jsonParse(MCL_STRING("{\"a\": \"abcd\"")));
  GREATEST_PASS();
}

GREATEST_TEST unicode_escape_in_value_fails(void) {
  GREATEST_ASSERT_EQ(false, jsonParse(MCL_STRING("{\"a\": \"\\u12G4\"}")));
  GREATEST_ASSERT_EQ(false, jsonParse(MCL_STRING("{\"a\": \"\\uzzzz\"}")));
  GREATEST_ASSERT_EQ(false, jsonParse(MCL_STRING("{\"a\": \"\\u123\"}")));
  GREATEST_ASSERT_EQ(false, jsonParse(MCL_STRING("{\"a\": \"\\u\"}")));
  GREATEST_PASS();
}

GREATEST_TEST embedded_nul_in_value_fails(void) {
  // NOTE: MCL_STRING can't express this — strlen stops at NUL.
  char buf[] = {'{', '"', 'a', '"', ':', ' ', '"', 'b', '\0', 'c', '"', '}'};
  String s = {.len = sizeof(buf), .data = buf};
  GREATEST_ASSERT_EQ(false, jsonParse(s));
  GREATEST_PASS();
}

// --- value is positive integer: valid cases ---

GREATEST_TEST positive_integer_value_passes(void) {
  GREATEST_ASSERT_EQ(true, jsonParse(MCL_STRING("{\"a\": 0}")));
  GREATEST_ASSERT_EQ(true, jsonParse(MCL_STRING("{\"a\": 7}")));
  GREATEST_ASSERT_EQ(true, jsonParse(MCL_STRING("{\"a\": 42}")));
  GREATEST_ASSERT_EQ(true, jsonParse(MCL_STRING("{\"a\": 1234567890}")));
  GREATEST_ASSERT_EQ(true, jsonParse(MCL_STRING("{\"a\":0}")));
  GREATEST_ASSERT_EQ(true, jsonParse(MCL_STRING("{\"a\" : 42 }")));
  GREATEST_PASS();
}

GREATEST_TEST multi_pair_integer_values_pass(void) {
  GREATEST_ASSERT_EQ(true, jsonParse(MCL_STRING("{\"a\": 1, \"b\": 2}")));
  GREATEST_PASS();
}

// --- value is positive integer: invalid cases ---
// NOTE: leading zeros, explicit plus, and trailing junk are rejected
// per JSON number grammar (a number is 0 | [1-9][0-9]*).

GREATEST_TEST positive_integer_value_fails(void) {
  GREATEST_ASSERT_EQ(false, jsonParse(MCL_STRING("{\"a\": 01}")));
  GREATEST_ASSERT_EQ(false, jsonParse(MCL_STRING("{\"a\": 007}")));
  GREATEST_ASSERT_EQ(false, jsonParse(MCL_STRING("{\"a\": +1}")));
  GREATEST_ASSERT_EQ(false, jsonParse(MCL_STRING("{\"a\": 12a}")));
  GREATEST_ASSERT_EQ(false, jsonParse(MCL_STRING("{\"a\": 1x}")));
  GREATEST_ASSERT_EQ(false, jsonParse(MCL_STRING("{\"a\": }")));
  GREATEST_PASS();
}

// --- value is fraction: valid cases ---
// NOTE: frac-part is "." 1*DIGIT per JSON number grammar.
// The int-part follows the same rule as integers (0 | [1-9][0-9]*),
// but the frac-part may contain leading zeros (e.g. 1.02).

GREATEST_TEST fraction_value_passes(void) {
  GREATEST_ASSERT_EQ(true, jsonParse(MCL_STRING("{\"a\": 0.5}")));
  GREATEST_ASSERT_EQ(true, jsonParse(MCL_STRING("{\"a\": 0.0}")));
  GREATEST_ASSERT_EQ(true, jsonParse(MCL_STRING("{\"a\": 3.14}")));
  GREATEST_ASSERT_EQ(true, jsonParse(MCL_STRING("{\"a\": 123.456}")));
  GREATEST_ASSERT_EQ(true, jsonParse(MCL_STRING("{\"a\": 1.02}")));
  GREATEST_ASSERT_EQ(true, jsonParse(MCL_STRING("{\"a\": 0.007}")));
  GREATEST_ASSERT_EQ(true, jsonParse(MCL_STRING("{\"a\":0.5}")));
  GREATEST_ASSERT_EQ(true, jsonParse(MCL_STRING("{\"a\" : 3.14 }")));
  GREATEST_PASS();
}

GREATEST_TEST multi_pair_fraction_values_pass(void) {
  GREATEST_ASSERT_EQ(true, jsonParse(MCL_STRING("{\"a\": 1.5, \"b\": 2.25}")));
  GREATEST_PASS();
}

// --- value is fraction: invalid cases ---
// NOTE: a dot must have digits on both sides; leading zeros in the
// int-part and extra dots / trailing junk are rejected.

GREATEST_TEST fraction_value_fails(void) {
  GREATEST_ASSERT_EQ(false, jsonParse(MCL_STRING("{\"a\": 1.}")));
  GREATEST_ASSERT_EQ(false, jsonParse(MCL_STRING("{\"a\": .5}")));
  GREATEST_ASSERT_EQ(false, jsonParse(MCL_STRING("{\"a\": 01.5}")));
  GREATEST_ASSERT_EQ(false, jsonParse(MCL_STRING("{\"a\": 00.5}")));
  GREATEST_ASSERT_EQ(false, jsonParse(MCL_STRING("{\"a\": 1..2}")));
  GREATEST_ASSERT_EQ(false, jsonParse(MCL_STRING("{\"a\": 1.2.3}")));
  GREATEST_ASSERT_EQ(false, jsonParse(MCL_STRING("{\"a\": 1.2a}")));
  GREATEST_ASSERT_EQ(false, jsonParse(MCL_STRING("{\"a\": 1. 2}")));
  GREATEST_PASS();
}

// --- value is exponent: valid cases ---
// NOTE: exp-part is ("e" / "E") ["-" / "+"] 1*DIGIT per JSON number grammar.
// It may follow an int-part (e.g. 1e10) or a frac-part (e.g. 1.5e10).

GREATEST_TEST exponent_value_passes(void) {
  GREATEST_ASSERT_EQ(true, jsonParse(MCL_STRING("{\"a\": 1e10}")));
  GREATEST_ASSERT_EQ(true, jsonParse(MCL_STRING("{\"a\": 1E10}")));
  GREATEST_ASSERT_EQ(true, jsonParse(MCL_STRING("{\"a\": 1e+10}")));
  GREATEST_ASSERT_EQ(true, jsonParse(MCL_STRING("{\"a\": 1e-10}")));
  GREATEST_ASSERT_EQ(true, jsonParse(MCL_STRING("{\"a\": 1E+10}")));
  GREATEST_ASSERT_EQ(true, jsonParse(MCL_STRING("{\"a\": 1E-10}")));
  GREATEST_ASSERT_EQ(true, jsonParse(MCL_STRING("{\"a\": 0e1}")));
  GREATEST_ASSERT_EQ(true, jsonParse(MCL_STRING("{\"a\": 123e45}")));
  GREATEST_ASSERT_EQ(true, jsonParse(MCL_STRING("{\"a\": 1.5e10}")));
  GREATEST_ASSERT_EQ(true, jsonParse(MCL_STRING("{\"a\": 3.14E-2}")));
  GREATEST_ASSERT_EQ(true, jsonParse(MCL_STRING("{\"a\": 0.5e+3}")));
  GREATEST_ASSERT_EQ(true, jsonParse(MCL_STRING("{\"a\":0e1}")));
  GREATEST_ASSERT_EQ(true, jsonParse(MCL_STRING("{\"a\" : 1e10 }")));
  GREATEST_PASS();
}

GREATEST_TEST multi_pair_exponent_values_pass(void) {
  GREATEST_ASSERT_EQ(true, jsonParse(MCL_STRING("{\"a\": 1e10, \"b\": 2.5E-3}")));
  GREATEST_PASS();
}

// --- value is exponent: invalid cases ---
// NOTE: exp-part requires one or more digits after optional sign;
// extra 'e's, dots, spaces, trailing junk, and leading zeros in the
// int-part are rejected.

GREATEST_TEST exponent_value_fails(void) {
  GREATEST_ASSERT_EQ(false, jsonParse(MCL_STRING("{\"a\": 1e}")));
  GREATEST_ASSERT_EQ(false, jsonParse(MCL_STRING("{\"a\": 1E}")));
  GREATEST_ASSERT_EQ(false, jsonParse(MCL_STRING("{\"a\": 1e+}")));
  GREATEST_ASSERT_EQ(false, jsonParse(MCL_STRING("{\"a\": 1e-}")));
  GREATEST_ASSERT_EQ(false, jsonParse(MCL_STRING("{\"a\": 1ee10}")));
  GREATEST_ASSERT_EQ(false, jsonParse(MCL_STRING("{\"a\": 1e1.5}")));
  GREATEST_ASSERT_EQ(false, jsonParse(MCL_STRING("{\"a\": 1ea}")));
  GREATEST_ASSERT_EQ(false, jsonParse(MCL_STRING("{\"a\": 1e 10}")));
  GREATEST_ASSERT_EQ(false, jsonParse(MCL_STRING("{\"a\": e10}")));
  GREATEST_ASSERT_EQ(false, jsonParse(MCL_STRING("{\"a\": 01e10}")));
  GREATEST_ASSERT_EQ(false, jsonParse(MCL_STRING("{\"a\": 1.5e}")));
  GREATEST_PASS();
}

// --- unicode escape (\uHHHH): valid cases ---
// NOTE: C string literals need "\\u" so the compiler emits a literal
// backslash + 'u' instead of a C universal-character escape.

GREATEST_TEST unicode_escape_in_key_passes(void) {
  GREATEST_ASSERT_EQ(true, jsonParse(MCL_STRING("{\"a\\u1234b\": 1}")));
  GREATEST_ASSERT_EQ(true, jsonParse(MCL_STRING("{\"\\u0041\": 1}")));
  GREATEST_ASSERT_EQ(true, jsonParse(MCL_STRING("{\"a\\u0000b\": 1}")));
  GREATEST_ASSERT_EQ(true, jsonParse(MCL_STRING("{\"a\\u00e9b\": 1}")));
  GREATEST_ASSERT_EQ(true, jsonParse(MCL_STRING("{\"a\\u00E9b\": 1}")));
  GREATEST_ASSERT_EQ(true, jsonParse(MCL_STRING("{\"a\\uFFFFb\": 1}")));
  GREATEST_ASSERT_EQ(true, jsonParse(MCL_STRING("{\"\\u0041\\u0042\": 1}")));
  GREATEST_PASS();
}

// --- unicode escape (\uHHHH): invalid cases ---

GREATEST_TEST unicode_escape_truncated_fails(void) {
  GREATEST_ASSERT_EQ(false, jsonParse(MCL_STRING("{\"a\\u123\": 1}")));
  GREATEST_ASSERT_EQ(false, jsonParse(MCL_STRING("{\"a\\u12\": 1}")));
  GREATEST_ASSERT_EQ(false, jsonParse(MCL_STRING("{\"a\\u1\": 1}")));
  GREATEST_ASSERT_EQ(false, jsonParse(MCL_STRING("{\"a\\u\": 1}")));
  GREATEST_PASS();
}

GREATEST_TEST unicode_escape_non_hex_fails(void) {
  GREATEST_ASSERT_EQ(false, jsonParse(MCL_STRING("{\"a\\u12G4\": 1}")));
  GREATEST_ASSERT_EQ(false, jsonParse(MCL_STRING("{\"a\\u12z4\": 1}")));
  GREATEST_ASSERT_EQ(false, jsonParse(MCL_STRING("{\"a\\uzzzz\": 1}")));
  GREATEST_ASSERT_EQ(false, jsonParse(MCL_STRING("{\"a\\u    \": 1}")));
  GREATEST_PASS();
}

// --- string: invalid cases ---

GREATEST_TEST unterminated_string_key_fails(void) {
  GREATEST_ASSERT_EQ(false, jsonParse(MCL_STRING("{\"abcd}")));
  GREATEST_ASSERT_EQ(false, jsonParse(MCL_STRING("{\"a\"")));
  GREATEST_ASSERT_EQ(false, jsonParse(MCL_STRING("{\"abcd\"")));
  GREATEST_PASS();
}

GREATEST_TEST missing_colon_after_key_fails(void) {
  GREATEST_ASSERT_EQ(false, jsonParse(MCL_STRING("{\"abcd\"}")));
  GREATEST_ASSERT_EQ(false, jsonParse(MCL_STRING("{\"abcd\" \"a1\"}")));
  GREATEST_ASSERT_EQ(false, jsonParse(MCL_STRING("{\"a\" 1}")));
  GREATEST_PASS();
}

GREATEST_TEST control_chars_in_string_fails(void) {
  GREATEST_ASSERT_EQ(false, jsonParse(MCL_STRING("{\"\n\"}")));
  GREATEST_PASS();
}

GREATEST_TEST control_chars_in_key_fail(void) {
  GREATEST_ASSERT_EQ(false, jsonParse(MCL_STRING("{\"a\nb\": 1}")));
  GREATEST_ASSERT_EQ(false, jsonParse(MCL_STRING("{\"a\tb\": 1}")));
  GREATEST_ASSERT_EQ(false, jsonParse(MCL_STRING("{\"a\rb\": 1}")));
  GREATEST_PASS();
}

GREATEST_TEST control_chars_in_value_fail(void) {
  GREATEST_ASSERT_EQ(false, jsonParse(MCL_STRING("{\"a\": \"b\nc\"}")));
  GREATEST_PASS();
}

GREATEST_TEST embedded_nul_in_key_fails(void) {
  // NOTE: MCL_STRING can't express this — strlen stops at NUL.
  char buf[] = {'{', '"', 'a', '\0', 'b', '"', ':', ' ', '1', '}'};
  String s = {.len = sizeof(buf), .data = buf};
  GREATEST_ASSERT_EQ(false, jsonParse(s));
  GREATEST_PASS();
}

// --- suites ---

GREATEST_SUITE(string_parsing) {
  GREATEST_RUN_TEST(alphanumeric_string_passes);
  GREATEST_RUN_TEST(unterminated_string_key_fails);
  GREATEST_RUN_TEST(missing_colon_after_key_fails);
  GREATEST_RUN_TEST(escape_in_key_passes);
  GREATEST_RUN_TEST(escape_in_value_passes);
  GREATEST_RUN_TEST(plain_string_value_passes);
  GREATEST_RUN_TEST(escape_sequences_in_value_passes);
  GREATEST_RUN_TEST(unicode_escape_in_value_passes);
  GREATEST_RUN_TEST(multi_pair_string_values_pass);
  GREATEST_RUN_TEST(unterminated_string_value_fails);
  GREATEST_RUN_TEST(unicode_escape_in_value_fails);
  GREATEST_RUN_TEST(embedded_nul_in_value_fails);
  GREATEST_RUN_TEST(positive_integer_value_passes);
  GREATEST_RUN_TEST(multi_pair_integer_values_pass);
  GREATEST_RUN_TEST(positive_integer_value_fails);
  GREATEST_RUN_TEST(fraction_value_passes);
  GREATEST_RUN_TEST(multi_pair_fraction_values_pass);
  GREATEST_RUN_TEST(fraction_value_fails);
  GREATEST_RUN_TEST(exponent_value_passes);
  GREATEST_RUN_TEST(multi_pair_exponent_values_pass);
  GREATEST_RUN_TEST(exponent_value_fails);
  GREATEST_RUN_TEST(unicode_escape_in_key_passes);
  GREATEST_RUN_TEST(unicode_escape_truncated_fails);
  GREATEST_RUN_TEST(unicode_escape_non_hex_fails);
  GREATEST_RUN_TEST(control_chars_in_string_fails);
  GREATEST_RUN_TEST(control_chars_in_key_fail);
  GREATEST_RUN_TEST(control_chars_in_value_fail);
  GREATEST_RUN_TEST(embedded_nul_in_key_fails);
}

GREATEST_SUITE(empty_object_suite) {
  GREATEST_RUN_TEST(empty_object_bare_passes);
  GREATEST_RUN_TEST(empty_object_with_inner_space_passes);
  GREATEST_RUN_TEST(empty_object_with_outer_whitespace_passes);
  GREATEST_RUN_TEST(empty_input_fails);
  GREATEST_RUN_TEST(unterminated_open_brace_fails);
  GREATEST_RUN_TEST(missing_open_brace_fails);
  GREATEST_RUN_TEST(trailing_garbage_fails);
  GREATEST_RUN_TEST(parse_object_with_consecutive_newlines);
}

GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
  GREATEST_MAIN_BEGIN();
  GREATEST_RUN_SUITE(empty_object_suite);
  GREATEST_RUN_SUITE(string_parsing);
  GREATEST_MAIN_END();
}
