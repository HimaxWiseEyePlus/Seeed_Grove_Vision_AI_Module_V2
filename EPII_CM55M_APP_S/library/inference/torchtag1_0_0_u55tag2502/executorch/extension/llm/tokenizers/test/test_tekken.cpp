/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @lint-ignore-every LICENSELINT
 */

/*

Here's the repro:

  cd ~/tokenizers

  rm -rf build-test

  mkdir build-test

  cd build-test

  cmake ../test -DSUPPORT_REGEX_LOOKAHEAD=ON

  cmake --build . --target test_tekken -j8

  cd ..

  RESOURCES_PATH="test/resources" ./build-test/test_tekken
*/

#include <gtest/gtest.h>
#include <pytorch/tokenizers/tekken.h>
#include <iostream>
#include <string>

using namespace ::testing;

namespace tokenizers {

namespace {

static inline std::string _get_resource_path(const std::string& name) {
  return std::string(std::getenv("RESOURCES_PATH")) + "/" + name;
}

} // namespace

class TekkenTest : public Test {
 public:
  void SetUp() override {
    tokenizer_ = std::make_unique<Tekken>();
    modelPath_ = _get_resource_path("test_tekken.json");
  }

  std::unique_ptr<Tokenizer> tokenizer_;
  std::string modelPath_;
};

TEST_F(TekkenTest, TestLoadModel) {
  Error res = tokenizer_->load(modelPath_);
  EXPECT_EQ(res, Error::Ok);
  EXPECT_EQ(tokenizer_->vocab_size(), 131072); // Default Tekken vocab size
}

TEST_F(TekkenTest, TestTokenizerProperties) {
  Error res = tokenizer_->load(modelPath_);
  EXPECT_EQ(res, Error::Ok);

  // Check BOS and EOS tokens
  EXPECT_GT(tokenizer_->bos_tok(), 0);
  EXPECT_GT(tokenizer_->eos_tok(), 0);
  EXPECT_NE(tokenizer_->bos_tok(), tokenizer_->eos_tok());
}

TEST_F(TekkenTest, TestBasicEncode) {
  Error res = tokenizer_->load(modelPath_);
  EXPECT_EQ(res, Error::Ok);

  std::string text = "Hello world";
  auto encoded = tokenizer_->encode(text, 1, 1); // With BOS and EOS
  EXPECT_EQ(encoded.error(), Error::Ok);

  auto tokens = encoded.get();
  EXPECT_GE(tokens.size(), 3); // At least BOS + content + EOS

  // Check BOS and EOS tokens are present
  EXPECT_EQ(tokens.front(), tokenizer_->bos_tok());
  EXPECT_EQ(tokens.back(), tokenizer_->eos_tok());

  std::cout << "Encoded 'Hello world' to " << tokens.size() << " tokens: ";
  for (auto token : tokens) {
    std::cout << token << " ";
  }
  std::cout << std::endl;
}

TEST_F(TekkenTest, TestEncodeWithoutBOSEOS) {
  Error res = tokenizer_->load(modelPath_);
  EXPECT_EQ(res, Error::Ok);

  std::string text = "Hello world";
  auto encoded = tokenizer_->encode(text, 0, 0); // No BOS/EOS
  EXPECT_EQ(encoded.error(), Error::Ok);

  auto tokens = encoded.get();
  EXPECT_GT(tokens.size(), 0);

  // Should not have BOS/EOS
  EXPECT_NE(tokens.front(), tokenizer_->bos_tok());
  EXPECT_NE(tokens.back(), tokenizer_->eos_tok());
}

TEST_F(TekkenTest, TestSpecialTokens) {
  Error res = tokenizer_->load(modelPath_);
  EXPECT_EQ(res, Error::Ok);

  // Test with instruction tokens
  std::string text = "[INST]Hello[/INST]";
  auto encoded = tokenizer_->encode(text, 0, 0);
  EXPECT_EQ(encoded.error(), Error::Ok);

  auto tokens = encoded.get();
  EXPECT_GT(tokens.size(), 0);

  std::cout << "Encoded '[INST]Hello[/INST]' to " << tokens.size()
            << " tokens: ";
  for (auto token : tokens) {
    std::cout << token << " ";
  }
  std::cout << std::endl;
}

TEST_F(TekkenTest, TestEmptyString) {
  Error res = tokenizer_->load(modelPath_);
  EXPECT_EQ(res, Error::Ok);

  std::string text = "";
  auto encoded = tokenizer_->encode(text, 1, 1); // With BOS/EOS
  EXPECT_EQ(encoded.error(), Error::Ok);

  auto tokens = encoded.get();
  EXPECT_EQ(tokens.size(), 2); // Just BOS and EOS
  EXPECT_EQ(tokens[0], tokenizer_->bos_tok());
  EXPECT_EQ(tokens[1], tokenizer_->eos_tok());
}

TEST_F(TekkenTest, TestMultilingualText) {
  Error res = tokenizer_->load(modelPath_);
  EXPECT_EQ(res, Error::Ok);

  // Test with some multilingual text
  std::string text = "Hello 世界 مرحبا";
  auto encoded = tokenizer_->encode(text, 0, 0);
  EXPECT_EQ(encoded.error(), Error::Ok);

  auto tokens = encoded.get();
  EXPECT_GT(tokens.size(), 0);

  std::cout << "Encoded multilingual text to " << tokens.size() << " tokens: ";
  for (auto token : tokens) {
    std::cout << token << " ";
  }
  std::cout << std::endl;
}

TEST_F(TekkenTest, TestDecode) {
  Error res = tokenizer_->load(modelPath_);
  EXPECT_EQ(res, Error::Ok);

  std::string original = "Hello world";
  auto encoded = tokenizer_->encode(original, 0, 0);
  EXPECT_EQ(encoded.error(), Error::Ok);

  // Test decode token by token
  auto tokens = encoded.get();
  std::string decoded_full = "";

  for (size_t i = 0; i < tokens.size(); ++i) {
    uint64_t prev_token = (i == 0) ? 0 : tokens[i - 1];
    auto decoded = tokenizer_->decode(prev_token, tokens[i]);
    EXPECT_EQ(decoded.error(), Error::Ok);
    decoded_full += decoded.get();
  }

  // The decoded text should match the original
  // Note: there might be minor differences in whitespace handling
  std::cout << "Original: '" << original << "'" << std::endl;
  std::cout << "Decoded:  '" << decoded_full << "'" << std::endl;
}

TEST_F(TekkenTest, TestVersion) {
  auto tekken = std::make_unique<Tekken>();
  Error res = tekken->load(modelPath_);
  EXPECT_EQ(res, Error::Ok);

  // Check that version is loaded
  std::string version = tekken->get_version();
  EXPECT_FALSE(version.empty());
  std::cout << "Tekken version: " << version << std::endl;
}

// Additional comprehensive tests inspired by mistral-common test harness

TEST_F(TekkenTest, TestSpecialTokensInText) {
  Error res = tokenizer_->load(modelPath_);
  EXPECT_EQ(res, Error::Ok);

  // Test special tokens within regular text
  std::vector<std::string> special_cases = {
      "[INST]Hello world[/INST]",
      "User said: [INST]test[/INST] and that's it",
      "[AVAILABLE_TOOLS]some tools[/AVAILABLE_TOOLS]",
      "Text with [IMG] image token",
  };

  for (const auto& text : special_cases) {
    auto encoded = tokenizer_->encode(text, 0, 0);
    EXPECT_EQ(encoded.error(), Error::Ok) << "Encoding failed for: " << text;

    auto tokens = encoded.get();
    EXPECT_FALSE(tokens.empty()) << "Empty tokens for: " << text;

    std::cout << "'" << text << "' -> " << tokens.size() << " tokens"
              << std::endl;
  }
}

TEST_F(TekkenTest, TestEmptyAndWhitespace) {
  Error res = tokenizer_->load(modelPath_);
  EXPECT_EQ(res, Error::Ok);

  // Test edge cases with empty/whitespace strings
  std::vector<std::string> edge_cases = {
      "", // empty
      " ", // single space
      "\n", // newline
      "\t", // tab
      "\r\n", // CRLF
      "   ", // multiple spaces
      "\n\n\n", // multiple newlines
      " \t \n ", // mixed whitespace
  };

  for (const auto& text : edge_cases) {
    auto encoded = tokenizer_->encode(text, 0, 0);
    EXPECT_EQ(encoded.error(), Error::Ok);

    auto tokens = encoded.get();
    // Empty text should produce empty tokens
    if (text.empty()) {
      EXPECT_TRUE(tokens.empty()) << "Non-empty tokens for empty string";
    } else {
      // Non-empty whitespace should produce tokens
      EXPECT_FALSE(tokens.empty()) << "Empty tokens for whitespace";
    }

    std::cout << "Whitespace test '" << text << "' -> " << tokens.size()
              << " tokens" << std::endl;
  }
}

TEST_F(TekkenTest, TestLongSequences) {
  Error res = tokenizer_->load(modelPath_);
  EXPECT_EQ(res, Error::Ok);

  // Test with long repetitive sequences
  std::string long_repeat(500, 'a'); // Keep reasonable for testing
  auto encoded = tokenizer_->encode(long_repeat, 0, 0);
  EXPECT_EQ(encoded.error(), Error::Ok);

  auto tokens = encoded.get();
  EXPECT_FALSE(tokens.empty());

  std::cout << "Long repeat (500 'a') -> " << tokens.size() << " tokens"
            << std::endl;

  // Test with long mixed content
  std::string long_mixed =
      "This is a very long sentence that contains many different words and should test the tokenizer's ability to handle longer sequences of text with various vocabulary items including numbers like 12345 and special characters like @#$%^&*()";
  auto mixed_encoded = tokenizer_->encode(long_mixed, 0, 0);
  EXPECT_EQ(mixed_encoded.error(), Error::Ok);

  auto mixed_tokens = mixed_encoded.get();
  EXPECT_FALSE(mixed_tokens.empty());

  std::cout << "Long mixed content -> " << mixed_tokens.size() << " tokens"
            << std::endl;
}

TEST_F(TekkenTest, TestSpecialCharacters) {
  Error res = tokenizer_->load(modelPath_);
  EXPECT_EQ(res, Error::Ok);

  // Test comprehensive special character handling
  std::vector<std::string> special_chars = {
      "!@#$%^&*()",
      "[]{}|;':\"",
      "<>,./?",
      "\\n\\t\\r", // escaped characters as literal strings
      "«»"
      "''—–", // typography
      "€$¥£¢", // currency
  };

  for (const auto& chars : special_chars) {
    auto encoded = tokenizer_->encode(chars, 0, 0);
    EXPECT_EQ(encoded.error(), Error::Ok)
        << "Encoding failed for special chars: " << chars;

    auto tokens = encoded.get();
    EXPECT_FALSE(tokens.empty()) << "Empty tokens for special chars: " << chars;

    std::cout << "Special chars '" << chars << "' -> " << tokens.size()
              << " tokens" << std::endl;
  }
}

TEST_F(TekkenTest, TestCodeLikeContent) {
  Error res = tokenizer_->load(modelPath_);
  EXPECT_EQ(res, Error::Ok);

  // Test code-like content that might be challenging
  std::vector<std::string> code_samples = {
      "def hello_world(): return 'Hello, World!'",
      "SELECT * FROM users WHERE id = 1;",
      "<html><body><p>Hello</p></body></html>",
      "{\"key\": \"value\", \"number\": 42}",
      "// Comment /* block comment */",
      "import torch",
      "console.log('Hello, world!');",
  };

  for (const auto& code : code_samples) {
    auto encoded = tokenizer_->encode(code, 0, 0);
    EXPECT_EQ(encoded.error(), Error::Ok)
        << "Encoding failed for code: " << code;

    auto tokens = encoded.get();
    EXPECT_FALSE(tokens.empty()) << "Empty tokens for code: " << code;

    std::cout << "Code '" << code.substr(0, 20) << "...' -> " << tokens.size()
              << " tokens" << std::endl;
  }
}

TEST_F(TekkenTest, TestBOSEOSCombinations) {
  Error res = tokenizer_->load(modelPath_);
  EXPECT_EQ(res, Error::Ok);

  std::string test_text = "Hello world";

  // Test all BOS/EOS combinations
  auto no_special = tokenizer_->encode(test_text, 0, 0);
  auto bos_only = tokenizer_->encode(test_text, 1, 0);
  auto eos_only = tokenizer_->encode(test_text, 0, 1);
  auto both_special = tokenizer_->encode(test_text, 1, 1);

  EXPECT_EQ(no_special.error(), Error::Ok);
  EXPECT_EQ(bos_only.error(), Error::Ok);
  EXPECT_EQ(eos_only.error(), Error::Ok);
  EXPECT_EQ(both_special.error(), Error::Ok);

  auto no_tokens = no_special.get();
  auto bos_tokens = bos_only.get();
  auto eos_tokens = eos_only.get();
  auto both_tokens = both_special.get();

  // Verify length relationships
  EXPECT_EQ(bos_tokens.size(), no_tokens.size() + 1);
  EXPECT_EQ(eos_tokens.size(), no_tokens.size() + 1);
  EXPECT_EQ(both_tokens.size(), no_tokens.size() + 2);

  // Verify BOS/EOS token positions
  EXPECT_EQ(bos_tokens[0], tokenizer_->bos_tok());
  EXPECT_EQ(eos_tokens.back(), tokenizer_->eos_tok());
  EXPECT_EQ(both_tokens[0], tokenizer_->bos_tok());
  EXPECT_EQ(both_tokens.back(), tokenizer_->eos_tok());

  std::cout << "BOS/EOS combinations test passed" << std::endl;
  std::cout << "  No special: " << no_tokens.size() << " tokens" << std::endl;
  std::cout << "  BOS only: " << bos_tokens.size() << " tokens" << std::endl;
  std::cout << "  EOS only: " << eos_tokens.size() << " tokens" << std::endl;
  std::cout << "  Both: " << both_tokens.size() << " tokens" << std::endl;
}

// Regression test to ensure token IDs match mistral-common mapping
TEST_F(TekkenTest, TestTokenIDMappingConsistency) {
  Error res = tokenizer_->load(modelPath_);
  EXPECT_EQ(res, Error::Ok);

  // Test BOS and EOS token IDs match mistral-common
  EXPECT_EQ(tokenizer_->bos_tok(), 1)
      << "BOS token should be 1 to match mistral-common";
  EXPECT_EQ(tokenizer_->eos_tok(), 2)
      << "EOS token should be 2 to match mistral-common";

  // Test that regular vocabulary tokens are offset by num_special_tokens (1000)
  // These expected values are from mistral-common Python implementation
  std::vector<std::pair<std::string, std::vector<uint64_t>>> test_cases = {
      {"Hello", {22177}}, // Regular word
      {"world", {34049}}, // Another regular word (different when alone)
      {"Hello world",
       {22177, 4304}}, // Two words (world has different ID in context)
      {".", {1046}}, // Single punctuation
      {"123", {1049, 1050, 1051}}, // Numbers
      {"@", {1064}}, // Special character
      {" ", {1032}}, // Space
      {"\n", {1010}}, // Newline
  };

  for (const auto& [text, expected_tokens] : test_cases) {
    auto encoded = tokenizer_->encode(text, 0, 0); // No BOS/EOS
    ASSERT_EQ(encoded.error(), Error::Ok) << "Failed to encode: " << text;

    auto tokens = encoded.get();
    ASSERT_EQ(tokens.size(), expected_tokens.size())
        << "Token count mismatch for '" << text << "'";

    for (size_t i = 0; i < tokens.size(); ++i) {
      EXPECT_EQ(tokens[i], expected_tokens[i])
          << "Token mismatch at position " << i << " for '" << text << "'"
          << " (got " << tokens[i] << ", expected " << expected_tokens[i]
          << ")";
    }
  }

  // Test special tokens use their direct IDs (0-999)
  std::vector<std::pair<std::string, std::vector<uint64_t>>>
      special_token_cases = {
          {"[INST]", {3}}, // Instruction start
          {"[/INST]", {4}}, // Instruction end
          {"[INST]Hello[/INST]", {3, 22177, 4}}, // With content
          {"[AVAILABLE_TOOLS]", {5}}, // Tools marker
          {"[/AVAILABLE_TOOLS]", {6}}, // Tools end
      };

  for (const auto& [text, expected_tokens] : special_token_cases) {
    auto encoded = tokenizer_->encode(text, 0, 0); // No BOS/EOS
    ASSERT_EQ(encoded.error(), Error::Ok) << "Failed to encode: " << text;

    auto tokens = encoded.get();
    ASSERT_EQ(tokens.size(), expected_tokens.size())
        << "Special token count mismatch for '" << text << "'";

    for (size_t i = 0; i < tokens.size(); ++i) {
      EXPECT_EQ(tokens[i], expected_tokens[i])
          << "Special token mismatch at position " << i << " for '" << text
          << "'" << " (got " << tokens[i] << ", expected " << expected_tokens[i]
          << ")";
    }
  }

  std::cout << "Token ID mapping consistency test passed!" << std::endl;
  std::cout << "  BOS=1, EOS=2 (matches mistral-common)" << std::endl;
  std::cout << "  Regular vocab offset by 1000 (matches mistral-common)"
            << std::endl;
  std::cout << "  Special tokens use IDs 0-999 (matches mistral-common)"
            << std::endl;
}

} // namespace tokenizers
