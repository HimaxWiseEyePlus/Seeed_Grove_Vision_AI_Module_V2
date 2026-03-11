/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */
// @lint-ignore-every LICENSELINT

#include <gtest/gtest.h>
#include <pytorch/tokenizers/normalizer.h>

using namespace tokenizers;

TEST(NormalizerTest, ReplaceNormalizerBasic) {
  // Test basic string replacement
  ReplaceNormalizer normalizer(" ", "▁");
  std::string input = "Hello World Test";
  std::string expected = "Hello▁World▁Test";
  std::string result = normalizer.normalize(input);
  EXPECT_EQ(result, expected);
}

TEST(NormalizerTest, ReplaceNormalizerNoMatch) {
  // Test when pattern doesn't match
  ReplaceNormalizer normalizer("xyz", "▁");
  std::string input = "Hello World";
  std::string expected = "Hello World";
  std::string result = normalizer.normalize(input);
  EXPECT_EQ(result, expected);
}

TEST(NormalizerTest, ReplaceNormalizerMultipleMatches) {
  // Test multiple matches
  ReplaceNormalizer normalizer("a", "X");
  std::string input = "banana";
  std::string expected = "bXnXnX";
  std::string result = normalizer.normalize(input);
  EXPECT_EQ(result, expected);
}

TEST(NormalizerTest, NormalizerConfigFromJson) {
  // Test JSON parsing for Replace normalizer
  nlohmann::json config = {
      {"type", "Replace"}, {"pattern", {{"String", " "}}}, {"content", "▁"}};

  NormalizerConfig norm_config;
  norm_config.parse_json(config);
  auto normalizer = norm_config.create();

  std::string input = "Hello World Test";
  std::string expected = "Hello▁World▁Test";
  std::string result = normalizer->normalize(input);
  EXPECT_EQ(result, expected);
}

TEST(NormalizerTest, NormalizerConfigFromJsonRegex) {
  // Test JSON parsing for Replace normalizer with regex
  nlohmann::json config = {
      {"type", "Replace"}, {"pattern", {{"Regex", "\\s+"}}}, {"content", "_"}};

  NormalizerConfig norm_config;
  norm_config.parse_json(config);
  auto normalizer = norm_config.create();

  std::string input = "Hello   World\t\tTest";
  std::string expected = "Hello_World_Test";
  std::string result = normalizer->normalize(input);
  EXPECT_EQ(result, expected);
}

TEST(NormalizerTest, SequenceNormalizer) {
  // Test sequence of normalizers
  std::vector<Normalizer::Ptr> normalizers;
  normalizers.push_back(std::make_shared<ReplaceNormalizer>(" ", "▁"));
  normalizers.push_back(std::make_shared<ReplaceNormalizer>("a", "X"));

  SequenceNormalizer seq_normalizer(normalizers);

  std::string input = "banana split";
  std::string expected = "bXnXnX▁split";
  std::string result = seq_normalizer.normalize(input);
  EXPECT_EQ(result, expected);
}

TEST(NormalizerTest, SequenceNormalizerFromConfig) {
  // Test sequence normalizer from config
  nlohmann::json config = {
      {"type", "Sequence"},
      {"normalizers",
       {{{"type", "Replace"}, {"pattern", {{"String", " "}}}, {"content", "▁"}},
        {{"type", "Replace"},
         {"pattern", {{"String", "a"}}},
         {"content", "X"}}}}};

  NormalizerConfig norm_config;
  norm_config.parse_json(config);
  auto normalizer = norm_config.create();

  std::string input = "banana split";
  std::string expected = "bXnXnX▁split";
  std::string result = normalizer->normalize(input);
  EXPECT_EQ(result, expected);
}

TEST(NormalizerTest, EmptyInput) {
  // Test with empty input
  ReplaceNormalizer normalizer(" ", "▁");
  std::string input = "";
  std::string expected = "";
  std::string result = normalizer.normalize(input);
  EXPECT_EQ(result, expected);
}

TEST(NormalizerTest, ConfigBuilder) {
  // Test config builder pattern
  auto normalizer =
      NormalizerConfig("Replace").set_pattern(" ").set_content("▁").create();

  std::string input = "Hello World";
  std::string expected = "Hello▁World";
  std::string result = normalizer->normalize(input);
  EXPECT_EQ(result, expected);
}
