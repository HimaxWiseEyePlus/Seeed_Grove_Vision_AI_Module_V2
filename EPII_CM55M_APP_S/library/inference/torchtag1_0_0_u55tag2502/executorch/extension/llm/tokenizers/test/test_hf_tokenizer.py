# Copyright (c) Meta Platforms, Inc. and affiliates.
# All rights reserved.
#
# This source code is licensed under the BSD-style license found in the
# LICENSE file in the root directory of this source tree.
# @lint-ignore-every LICENSELINT

"""
Test script for hf tokenizers.
"""

import unittest
from tempfile import TemporaryDirectory

import pytest
from pytorch_tokenizers import CppHFTokenizer
from transformers import AutoTokenizer

PROMPT = "What is the capital of France?"


@pytest.mark.parametrize("model_id", ["HuggingFaceTB/SmolLM3-3B", "Qwen/Qwen2.5-0.5B"])
def test_models(model_id: str) -> None:
    with TemporaryDirectory() as temp_dir:
        tokenizer = AutoTokenizer.from_pretrained(model_id)
        tokenizer_path = tokenizer.save_pretrained(temp_dir)[-1]

        cpp_tokenizer = CppHFTokenizer()
        cpp_tokenizer.load(tokenizer_path)

        tokens = tokenizer.encode(PROMPT)
        cpp_tokens = cpp_tokenizer.encode(PROMPT)
        assert tokens == cpp_tokens


class TestHfTokenizer(unittest.TestCase):
    def setUp(self) -> None:
        self.temp_dir = TemporaryDirectory()
        super().setUp()

    def test_llama3_2_1b(self) -> None:
        tokenizer = AutoTokenizer.from_pretrained("unsloth/Llama-3.2-1B-Instruct")
        tokenizer_path = tokenizer.save_pretrained(self.temp_dir.name)[-1]

        cpp_tokenizer = CppHFTokenizer()
        cpp_tokenizer.load(tokenizer_path)

        tokens = tokenizer.encode(PROMPT)
        cpp_tokens = cpp_tokenizer.encode(PROMPT, bos=1)
        self.assertEqual(tokens, cpp_tokens)

    def test_phi_4_mini(self) -> None:
        tokenizer = AutoTokenizer.from_pretrained(
            "software-mansion/react-native-executorch-phi-4-mini"
        )
        tokenizer_path = tokenizer.save_pretrained(self.temp_dir.name)[-1]

        cpp_tokenizer = CppHFTokenizer()
        cpp_tokenizer.load(tokenizer_path)

        tokens = tokenizer.encode(PROMPT)
        cpp_tokens = cpp_tokenizer.encode(PROMPT)
        self.assertEqual(tokens, cpp_tokens)
