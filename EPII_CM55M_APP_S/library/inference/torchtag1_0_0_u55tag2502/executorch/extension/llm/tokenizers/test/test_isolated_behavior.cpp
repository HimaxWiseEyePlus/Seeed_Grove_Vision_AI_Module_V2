/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */
// @lint-ignore-every LICENSELINT

#include <gtest/gtest.h>
#include <pytorch/tokenizers/pre_tokenizer.h>

using namespace tokenizers;

TEST(IsolatedBehaviorTest, BasicIsolatedBehavior) {
  // Test the example from the comment: "the-final--countdown" -> ["the", "-",
  // "final", "-", "-", "countdown"]
  RegexPreTokenizer tokenizer("-", true, "Isolated");
  std::string input = "the-final--countdown";
  std::vector<std::string> expected = {
      "the", "-", "final", "-", "-", "countdown"};
  std::vector<std::string> result = tokenizer.pre_tokenize(input);

  EXPECT_EQ(result.size(), expected.size());
  for (size_t i = 0; i < expected.size(); ++i) {
    EXPECT_EQ(result[i], expected[i]) << "Mismatch at index " << i;
  }
}

TEST(IsolatedBehaviorTest, SingleDelimiter) {
  // Test with single delimiter
  RegexPreTokenizer tokenizer("-", true, "Isolated");
  std::string input = "hello-world";
  std::vector<std::string> expected = {"hello", "-", "world"};
  std::vector<std::string> result = tokenizer.pre_tokenize(input);

  EXPECT_EQ(result.size(), expected.size());
  for (size_t i = 0; i < expected.size(); ++i) {
    EXPECT_EQ(result[i], expected[i]) << "Mismatch at index " << i;
  }
}

TEST(IsolatedBehaviorTest, NoDelimiters) {
  // Test with no delimiters
  RegexPreTokenizer tokenizer("-", true, "Isolated");
  std::string input = "helloworld";
  std::vector<std::string> expected = {"helloworld"};
  std::vector<std::string> result = tokenizer.pre_tokenize(input);

  EXPECT_EQ(result.size(), expected.size());
  EXPECT_EQ(result[0], expected[0]);
}

TEST(IsolatedBehaviorTest, DelimiterAtStart) {
  // Test with delimiter at start
  RegexPreTokenizer tokenizer("-", true, "Isolated");
  std::string input = "-hello";
  std::vector<std::string> expected = {"-", "hello"};
  std::vector<std::string> result = tokenizer.pre_tokenize(input);

  EXPECT_EQ(result.size(), expected.size());
  for (size_t i = 0; i < expected.size(); ++i) {
    EXPECT_EQ(result[i], expected[i]) << "Mismatch at index " << i;
  }
}

TEST(IsolatedBehaviorTest, DelimiterAtEnd) {
  // Test with delimiter at end
  RegexPreTokenizer tokenizer("-", true, "Isolated");
  std::string input = "hello-";
  std::vector<std::string> expected = {"hello", "-"};
  std::vector<std::string> result = tokenizer.pre_tokenize(input);

  EXPECT_EQ(result.size(), expected.size());
  for (size_t i = 0; i < expected.size(); ++i) {
    EXPECT_EQ(result[i], expected[i]) << "Mismatch at index " << i;
  }
}

TEST(IsolatedBehaviorTest, OnlyDelimiters) {
  // Test with only delimiters
  RegexPreTokenizer tokenizer("-", true, "Isolated");
  std::string input = "---";
  std::vector<std::string> expected = {"-", "-", "-"};
  std::vector<std::string> result = tokenizer.pre_tokenize(input);

  EXPECT_EQ(result.size(), expected.size());
  for (size_t i = 0; i < expected.size(); ++i) {
    EXPECT_EQ(result[i], expected[i]) << "Mismatch at index " << i;
  }
}

TEST(IsolatedBehaviorTest, SpaceDelimiter) {
  // Test with space as delimiter
  RegexPreTokenizer tokenizer(" ", true, "Isolated");
  std::string input = "hello world test";
  std::vector<std::string> expected = {"hello", " ", "world", " ", "test"};
  std::vector<std::string> result = tokenizer.pre_tokenize(input);

  EXPECT_EQ(result.size(), expected.size());
  for (size_t i = 0; i < expected.size(); ++i) {
    EXPECT_EQ(result[i], expected[i]) << "Mismatch at index " << i;
  }
}

TEST(IsolatedBehaviorTest, JSONConfig) {
  // Test with JSON configuration
  nlohmann::json config = {
      {"type", "Split"},
      {"pattern", {{"String", "-"}}},
      {"behavior", "Isolated"},
      {"invert", false}};

  PreTokenizerConfig pre_config;
  pre_config.parse_json(config);
  auto tokenizer = pre_config.create();

  std::string input = "the-final--countdown";
  std::vector<std::string> expected = {
      "the", "-", "final", "-", "-", "countdown"};
  std::vector<std::string> result = tokenizer->pre_tokenize(input);

  EXPECT_EQ(result.size(), expected.size());
  for (size_t i = 0; i < expected.size(); ++i) {
    EXPECT_EQ(result[i], expected[i]) << "Mismatch at index " << i;
  }
}

TEST(IsolatedBehaviorTest, EmptyInput) {
  // Test with empty input
  RegexPreTokenizer tokenizer("-", true, "Isolated");
  std::string input = "";
  std::vector<std::string> result = tokenizer.pre_tokenize(input);

  EXPECT_EQ(result.size(), 1);
  EXPECT_EQ(result[0], "");
}
