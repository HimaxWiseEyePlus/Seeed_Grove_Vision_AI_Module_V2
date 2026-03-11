/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */
// @lint-ignore-every LICENSELINT

#include <pytorch/tokenizers/hf_tokenizer.h>

// Standard
#include <algorithm>
#include <cinttypes>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

// Third Party
#include <nlohmann/json.hpp>

namespace fs = std::filesystem;
using json = nlohmann::json;

namespace tokenizers {

// -------------------------private method end-------------------------------
// -------------------------public method start-------------------------------

Error HFTokenizer::load(const std::string& path) {
  // If this is a directory, look for tokenizer.json and tokenizer_config.json
  std::string model_json = path;
  std::string model_config_json = "";
  if (fs::is_directory(path)) {
    const fs::path root(path);
    model_json = (root / "tokenizer.json").string();
    if (!fs::exists(model_json)) {
      TK_LOG(Info, "no tokenizer.json found in %s", path.c_str());
      return Error::LoadFailure;
    }
    const auto model_config_json_path = root / "tokenizer_config.json";
    if (fs::exists(model_config_json_path)) {
      model_config_json = model_config_json_path.string();
    }
  }

  // Load the tokenizer.json file
  std::ifstream file(model_json);
  if (!file) {
    TK_LOG(Info, "failed to open encoder file: %s", path.c_str());
    return Error::LoadFailure;
  }
  std::string contents(
      (std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
  json parsed_json;
  try {
    parsed_json = json::parse(contents);
  } catch (const std::exception& e) {
    TK_LOG(Error, "Error parsing json file: %s", e.what());
    return Error::LoadFailure;
  }

  // Parse the special tokens
  try {
    std::vector<std::pair<std::string, std::uint64_t>> special_token_pairs;
    const auto& special_tokens = parsed_json.at("added_tokens");
    auto special_token_map = TK_UNWRAP(detail::build_token_map(
        special_tokens,
        [](const auto& it) -> std::string { return it.at("content"); },
        [](const auto& it) -> std::uint64_t { return it.at("id"); }));

    // Create special token regex to help later with encoding.
    special_token_regex_ =
        TK_UNWRAP(detail::build_special_token_regex(special_token_map));

    // Store for future use.
    special_token_map_.emplace(std::move(special_token_map));
  } catch (const std::exception& e) {
    TK_LOG(Info, "Could not parse special tokens: %s", e.what());
    return Error::LoadFailure;
  }

  // Parse the standard tokens
  try {
    std::vector<std::pair<std::string, std::uint64_t>> token_pairs;
    const auto& vocab = parsed_json.at("/model/vocab"_json_pointer);
    for (const auto& entry : vocab.items()) {
      const std::string token = entry.key();
      const uint64_t token_id = entry.value();
      // Skip adding special tokens to the standard encoder/decoder
      if (!special_token_map_->tryGetString(token_id)) {
        token_pairs.emplace_back(token, token_id);
      }
    }

    auto token_map = TK_UNWRAP(detail::build_token_map(std::move(token_pairs)));
    token_map_.emplace(std::move(token_map));
  } catch (const std::exception& e) {
    TK_LOG(Info, "Could not parse tokens: %s", e.what());
    return Error::LoadFailure;
  }

  // Set the vocab size to include special tokens
  vocab_size_ = token_map_->size() + special_token_map_->size();

  // Set up the normalizer (optional)
  try {
    TK_LOG(Info, "Setting up normalizer...");
    const auto& normalizer_json = parsed_json.at("normalizer");
    if (!normalizer_json.is_null()) {
      _normalizer = NormalizerConfig().parse_json(normalizer_json).create();
      TK_LOG(Info, "Normalizer set up");
    } else {
      TK_LOG(Info, "Normalizer field is null, skipping");
    }
  } catch (const std::exception& e) {
    // No "Normalizer" field found
    TK_LOG(
        Info,
        "No 'Normalizer' field found in json, out of range error: %s",
        e.what());
  }

  // Set up the pre-tokenizer
  try {
    TK_LOG(Info, "Setting up pretokenizer...");
    _pretokenizer = PreTokenizerConfig()
                        .parse_json(parsed_json.at("pre_tokenizer"))
                        .create();
    TK_LOG(Info, "Pretokenizer set up");
  } catch (const std::exception& e) {
    TK_LOG(Info, "Could not parse pre_tokenizer: %s", e.what());
    return Error::LoadFailure;
  }

  // Set up the decoder (optional)
  try {
    _decoder =
        TokenDecoderConfig().parse_json(parsed_json.at("decoder")).create();
  } catch (const std::exception&) {
    // No decoder specified
  }

  // Parse the BPE merges
  try {
    TK_LOG(Info, "Loading BPE merges...");
    const auto& merges = parsed_json.at("/model/merges"_json_pointer);
    std::vector<std::pair<std::string, std::string>> merge_pairs;

    for (const auto& merge : merges) {
      if (merge.size() == 2) {
        std::string first = merge[0];
        std::string second = merge[1];
        merge_pairs.emplace_back(first, second);
      }
    }

    // Build merge map: (token_id_1, token_id_2) -> (rank, merged_token_id)
    merge_map_ = std::make_unique<detail::MergeMap>();
    for (size_t i = 0; i < merge_pairs.size(); ++i) {
      const auto& [first, second] = merge_pairs[i];

      // Get token IDs for the merge pair
      auto first_id = token_map_->tryGetInteger(first);
      auto second_id = token_map_->tryGetInteger(second);

      if (first_id && second_id) {
        // Create merged token string
        std::string merged = first + second;
        auto merged_id = token_map_->tryGetInteger(merged);

        if (merged_id) {
          // Store merge rule: (first_id, second_id) -> (rank, merged_id)
          merge_map_->emplace(
              std::make_pair(*first_id, *second_id),
              std::make_pair(static_cast<uint32_t>(i), *merged_id));
        }
      }
    }

    TK_LOG(
        Info,
        "Loaded %" PRId64 " BPE merge rules",
        static_cast<int64_t>(merge_map_->size()));

    // Pre-compute merge ranks for efficient BPE encoding
    auto merge_ranks =
        TK_UNWRAP(detail::build_merge_ranks_map(*merge_map_, *token_map_));
    TK_LOG(
        Info,
        "Built merge ranks map with %" PRId64 " entries",
        static_cast<int64_t>(merge_ranks.size()));
    merge_ranks_.emplace(std::move(merge_ranks));
  } catch (const std::exception& e) {
    TK_LOG(Error, "Could not parse merges: %s", e.what());
    return Error::LoadFailure;
  }

  // If a tokenizer config file is found, parse it to look up the eos/bos tokens
  if (!model_config_json.empty()) {
    // Load it and parse it as json
    std::ifstream config_file(model_config_json);
    if (!config_file) {
      TK_LOG(Error, "failed to open encoder file: %s", path.c_str());
      return Error::LoadFailure;
    }
    std::string config_contents(
        (std::istreambuf_iterator<char>(config_file)),
        std::istreambuf_iterator<char>());
    json parsed_config_json;
    try {
      parsed_config_json = json::parse(config_contents);
    } catch (const std::exception& e) {
      TK_LOG(Error, "Error parsing model config json json file: %s", e.what());
      return Error::LoadFailure;
    }

    // Pull out the token strings
    try {
      const std::string bos_token = parsed_config_json.contains("bos_token") &&
              !parsed_config_json["bos_token"].is_null()
          ? parsed_config_json["bos_token"].get<std::string>()
          : "";

      const std::string eos_token = parsed_config_json.contains("eos_token") &&
              !parsed_config_json["eos_token"].is_null()
          ? parsed_config_json["eos_token"].get<std::string>()
          : "";
      const auto bos_res = special_token_map_->tryGetInteger(bos_token);
      const auto eos_res = special_token_map_->tryGetInteger(eos_token);
      if (!bos_res) {
        TK_LOG(Error, "BOS token %s not in special tokens", bos_token.c_str());
        return Error::LoadFailure;
      }
      if (!eos_res) {
        TK_LOG(Error, "EOS token %s not in special tokens", eos_token.c_str());
        return Error::LoadFailure;
      }
      bos_tok_ = *bos_res;
      eos_tok_ = *eos_res;
    } catch (const std::exception& e) {
      TK_LOG(Error, "Could not eos/bos from tokenizer config: %s", e.what());
      return Error::LoadFailure;
    }
  }

  // Otherwise, make an educated guess with the following logic:
  // 1. Look for special tokens with "bos"/"begin" or "eos"/"end" in them
  // 2. Sub-qualify with the word "text" if needed
  // 3. If EOS found, but BOS is not (or vice versa), assume they are the same
  else {
    std::vector<std::string_view> bos_candidates;
    std::vector<std::string_view> eos_candidates;
    for (std::size_t token_idx = 0; token_idx < special_token_map_->size();
         ++token_idx) {
      const auto [token, _] = special_token_map_->getElement(token_idx);
      if (token.find("bos") != std::string::npos ||
          token.find("begin") != std::string::npos) {
        bos_candidates.push_back(token);
      }
      if (token.find("eos") != std::string::npos ||
          token.find("end") != std::string::npos) {
        eos_candidates.push_back(token);
      }
    }

    if (bos_candidates.size() > 1) {
      const auto orig_candidates = std::move(bos_candidates);
      bos_candidates.clear();
      for (const auto& cand : orig_candidates) {
        if (cand.find("text") != std::string::npos) {
          bos_candidates.push_back(cand);
        }
      }
    }
    if (eos_candidates.size() > 1) {
      const auto orig_candidates = std::move(eos_candidates);
      eos_candidates.clear();
      for (const auto& cand : orig_candidates) {
        if (cand.find("text") != std::string::npos) {
          eos_candidates.push_back(cand);
        }
      }
    }

    // Use if a single candidate
    bool bos_found = false;
    bool eos_found = false;
    if (bos_candidates.size() == 1) {
      bos_found = true;
      bos_tok_ = *(special_token_map_->tryGetInteger(bos_candidates[0]));
    }
    if (eos_candidates.size() == 1) {
      eos_found = true;
      eos_tok_ = *(special_token_map_->tryGetInteger(eos_candidates[0]));
    }

    // Make them the same if only one found
    if (bos_found && !eos_found) {
      eos_tok_ = bos_tok_;
    } else if (!bos_found && eos_found) {
      bos_tok_ = eos_tok_;
    }
  }

  // Mark initialized once everything is done
  initialized_ = true;

  return Error::Ok;
}
// -------------------------public method end-----------------------------------
// -------------------------private method start--------------------------------

Error HFTokenizer::_encode(
    const std::string& input,
    std::vector<uint64_t>& ret,
    uint64_t& last_piece_token_len) const {
  // Apply normalization first if normalizer is available
  std::string normalized_input = input;
  if (_normalizer) {
    normalized_input = _normalizer->normalize(input);
    TK_LOG(
        Info,
        "normalized input: '%s' -> '%s'",
        input.c_str(),
        normalized_input.c_str());
  }

  for (const auto& piece : _pretokenizer->pre_tokenize(normalized_input)) {
    // Check if the entire word is already a token to skip merging.
    const auto result = token_map_->tryGetInteger(piece);
    if (result) {
      last_piece_token_len = 1;
      ret.push_back(*result);
      continue;
    }
    auto tokens = TK_UNWRAP(byte_pair_encode_(piece, *token_map_));

    last_piece_token_len = tokens.size();
    ret.insert(ret.end(), tokens.begin(), tokens.end());
  }
  return Error::Ok;
}

void HFTokenizer::_decode(const std::string& input, std::string& ret) const {
  if (_decoder) {
    ret += _decoder->decode(input);
  } else {
    ret += input;
  }
}

Result<std::vector<uint64_t>> HFTokenizer::byte_pair_encode_(
    const std::string& piece,
    const detail::TokenMap& token_map) const {
  if (piece.size() == 1) {
    const auto result = token_map.tryGetInteger(piece);
    if (result) {
      return std::vector<uint64_t>(*result);
    } else {
      TK_LOG(Error, "unknown token: '%s'", piece.c_str());
      return Error::EncodeFailure;
    }
  }

  // Use the pre-computed merge ranks (computed once during loading)
  const detail::TokenMap& merge_ranks =
      merge_ranks_ ? *merge_ranks_ : token_map;

  // Use the overridden _byte_pair_merge function with the proper merge ranks
  return _byte_pair_merge(
      piece, merge_ranks, [&piece, &token_map](uint64_t start, uint64_t stop) {
        std::string key = piece.substr(start, stop - start);
        const auto result = token_map.tryGetInteger(key);
        if (result) {
          return *result;
        } else {
          TK_LOG(
              Error,
              "BPE merge produced unknown token: '%s', start: %" PRIu64
              ", stop: %" PRIu64,
              key.c_str(),
              start,
              stop);
          return uint64_t(0); // Return unknown token ID instead of padding
        }
      });
}

std::vector<uint64_t> HFTokenizer::_byte_pair_merge(
    const std::string& piece,
    const detail::TokenMap& ranks,
    std::function<uint64_t(uint64_t, uint64_t)> func) const {
  // HF-specific BPE implementation that uses the Rust-style approach
  // with pre-computed merge ranks

  // Start with individual characters (like Rust implementation)
  HFWord word;

  // Process each UTF-8 character individually
  size_t i = 0;
  while (i < piece.size()) {
    size_t char_start = i;
    size_t char_len = 1;

    // Determine UTF-8 character length
    unsigned char byte = static_cast<unsigned char>(piece[i]);
    if ((byte & 0x80) == 0) {
      // ASCII character (0xxxxxxx)
      char_len = 1;
    } else if ((byte & 0xE0) == 0xC0) {
      // 2-byte UTF-8 character (110xxxxx)
      char_len = 2;
    } else if ((byte & 0xF0) == 0xE0) {
      // 3-byte UTF-8 character (1110xxxx)
      char_len = 3;
    } else if ((byte & 0xF8) == 0xF0) {
      // 4-byte UTF-8 character (11110xxx)
      char_len = 4;
    } else {
      // Invalid UTF-8 start byte, treat as single byte
      char_len = 1;
    }

    // Make sure we don't go beyond the string boundary
    if (char_start + char_len > piece.size()) {
      char_len = piece.size() - char_start;
    }

    uint64_t token_id = func(char_start, char_start + char_len);
    if (token_id != 0) { // Assuming 0 is padding/error token
      word.add(token_id, char_len);
    } else {
      // Handle unknown character
      TK_LOG(Error, "Unknown character in HF BPE at position %zu", char_start);
      return {}; // Return empty vector to indicate failure
    }

    i += char_len;
  }

  // Apply BPE merges using the pre-computed merge ranks and token map
  if (merge_ranks_ && token_map_) {
    word.merge_all(*merge_ranks_, *token_map_);
  }

  return word.tokens;
}

} // namespace tokenizers
