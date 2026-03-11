/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */
// @lint-ignore-every LICENSELINT

// Local
#include <pytorch/tokenizers/normalizer.h>

// Third Party
#include <unicode.h>

// Standard
#include <algorithm>
#include <iterator>
#include <utility>

// Third Party
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace tokenizers {

// NormalizerConfig ////////////////////////////////////////////////////////////

NormalizerConfig::NormalizerConfig(std::string type) : type(std::move(type)) {}

Normalizer::Ptr NormalizerConfig::create() const {
  // NOTE: These types must line up with the type strings found in the
  //  tokenizers library
  //  https://github.com/huggingface/tokenizers/blob/main/tokenizers/src/normalizers/mod.rs
  if (type == "Replace") {
    if (!pattern) {
      throw std::runtime_error(
          "Missing pattern for Normalizer of type Replace");
    }
    if (!content) {
      throw std::runtime_error(
          "Missing content for Normalizer of type Replace");
    }
    return Normalizer::Ptr(new ReplaceNormalizer(*pattern, *content));
  }
  if (type == "Sequence") {
    if (!normalizers or normalizers->empty()) {
      throw std::runtime_error(
          "Missing normalizers for Normalizer of type Sequence");
    }
    std::vector<Normalizer::Ptr> norms;
    std::transform(
        normalizers->begin(),
        normalizers->end(),
        std::back_inserter(norms),
        [](const NormalizerConfig& cfg) { return cfg.create(); });
    return Normalizer::Ptr(new SequenceNormalizer(norms));
  }
  if (type == "NFC") {
    return Normalizer::Ptr(new NFCNormalizer());
  }
  throw std::runtime_error("Unsupported Normalizer type: " + type);
}

NormalizerConfig& NormalizerConfig::parse_json(const json& json_config) {
  type = json_config.at("type");
  if (type == "Replace") {
    try {
      pattern = json_config.at("pattern").at("Regex");
    } catch (json::out_of_range&) {
      // "Regex" is not there, check "String", which is a literal string
      std::string literal = json_config.at("pattern").at("String");
      // For string patterns, escape regex special characters to treat them as
      // literal strings (same as Rust's regex::escape)
      pattern = IRegex::escape(literal);
    }

    content = json_config.at("content");
  } else if (type == "Sequence") {
    normalizers = std::vector<NormalizerConfig>();
    for (const auto& entry : json_config.at("normalizers")) {
      normalizers->push_back(NormalizerConfig().parse_json(entry));
    }
  } else if (type == "NFC") {
    // NFC normalizer has no additional configuration parameters
    TK_LOG(
        Info,
        "Using NFC normalizer. Please notice that our implementation may not handle all edge cases.");
  } else {
    throw std::runtime_error("Unsupported Normalizer type: " + type);
  }
  return *this;
}

// ReplaceNormalizer ///////////////////////////////////////////////////////////

std::unique_ptr<IRegex> ReplaceNormalizer::create_regex_(
    const std::string& pattern) {
  assert(!pattern.empty());
  return TK_UNWRAP_THROW(create_regex(pattern));
}

std::string ReplaceNormalizer::normalize(const std::string& input) const {
  if (!regex_)
    return input;

  std::string result = input;
  auto matches = regex_->find_all(result);

  // Process matches in reverse order to avoid offset issues
  for (auto it = matches.rbegin(); it != matches.rend(); ++it) {
    const auto& match = *it;
    result.replace(match.start, match.end - match.start, content_);
  }

  return result;
}

// SequenceNormalizer //////////////////////////////////////////////////////////

SequenceNormalizer::SequenceNormalizer(std::vector<Normalizer::Ptr> normalizers)
    : normalizers_(std::move(normalizers)) {}

std::string SequenceNormalizer::normalize(const std::string& input) const {
  std::string result = input;
  for (const auto& normalizer : normalizers_) {
    result = normalizer->normalize(result);
  }
  return result;
}

// NFCNormalizer ///////////////////////////////////////////////////////////////

std::string NFCNormalizer::normalize(const std::string& input) const {
  // Convert UTF-8 string to codepoints
  auto codepoints = unicode_cpts_from_utf8(input);

  // Apply NFC normalization
  auto normalized_cpts = unicode_cpts_normalize_nfc(codepoints);

  // Convert back to UTF-8 string
  std::string result;
  for (uint32_t cpt : normalized_cpts) {
    result += unicode_cpt_to_utf8(cpt);
  }

  return result;
}

} // namespace tokenizers
