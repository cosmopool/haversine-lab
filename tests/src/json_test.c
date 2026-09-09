// Disable greatest abbrevs: core.h already defines ASSERT().
#define GREATEST_USE_ABBREVS 0
#include "../vendor/greatest.h"

#define SUPPRESS_ERRORS true
#include "../../src/json.c"

// --- empty object: valid cases ---

GREATEST_TEST empty_object_bare_passes(void) {
  GREATEST_ASSERT_EQ(true, jsonParse(mclStringNewC("{}")));
  GREATEST_PASS();
}

GREATEST_TEST empty_object_with_inner_space_passes(void) {
  GREATEST_ASSERT_EQ(true, jsonParse(mclStringNewC("{ }")));
  GREATEST_ASSERT_EQ(true, jsonParse(mclStringNewC("{\n\t\r\n }")));
  GREATEST_PASS();
}

GREATEST_TEST empty_object_with_outer_whitespace_passes(void) {
  GREATEST_ASSERT_EQ(true, jsonParse(mclStringNewC("  {}\n")));
  GREATEST_ASSERT_EQ(true, jsonParse(mclStringNewC("\t{\n} \n")));
  GREATEST_PASS();
}

// --- empty object: invalid cases ---

GREATEST_TEST empty_input_fails(void) {
  // NOTE: can't use mclStringNewC("") — it asserts len > 0.
  String s = {.len = 0, .data = ""};
  GREATEST_ASSERT_EQ(false, jsonParse(s));
  GREATEST_PASS();
}

GREATEST_TEST unterminated_open_brace_fails(void) {
  GREATEST_ASSERT_EQ(false, jsonParse(mclStringNewC("{")));
  GREATEST_ASSERT_EQ(false, jsonParse(mclStringNewC("  {  ")));
  GREATEST_PASS();
}

GREATEST_TEST missing_open_brace_fails(void) {
  GREATEST_ASSERT_EQ(false, jsonParse(mclStringNewC("}")));
  GREATEST_ASSERT_EQ(false, jsonParse(mclStringNewC("[]")));
  GREATEST_ASSERT_EQ(false, jsonParse(mclStringNewC("x")));
  GREATEST_PASS();
}

GREATEST_TEST trailing_garbage_fails(void) {
  GREATEST_ASSERT_EQ(false, jsonParse(mclStringNewC("{}x")));
  GREATEST_ASSERT_EQ(false, jsonParse(mclStringNewC("{} {}")));
  GREATEST_PASS();
}

GREATEST_SUITE(empty_object_suite) {
  GREATEST_RUN_TEST(empty_object_bare_passes);
  GREATEST_RUN_TEST(empty_object_with_inner_space_passes);
  GREATEST_RUN_TEST(empty_object_with_outer_whitespace_passes);
  GREATEST_RUN_TEST(empty_input_fails);
  GREATEST_RUN_TEST(unterminated_open_brace_fails);
  GREATEST_RUN_TEST(missing_open_brace_fails);
  GREATEST_RUN_TEST(trailing_garbage_fails);
}

GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
  GREATEST_MAIN_BEGIN();
  GREATEST_RUN_SUITE(empty_object_suite);
  GREATEST_MAIN_END();
}
