# Copyright (c) Meta Platforms, Inc. and affiliates.
# All rights reserved.
#
# This source code is licensed under the BSD-style license found in the
# LICENSE file in the root directory of this source tree.
# @lint-ignore-every LICENSELINT

"""
Test script for PyTorch Tokenizers Python bindings
"""

import os
import sys
import unittest

try:
    import pytorch_tokenizers
except ImportError as e:
    print(f"Failed to import pytorch_tokenizers: {e}")
    print("Make sure the package is installed with: pip install .")
    sys.exit(1)


class TestPythonBindings(unittest.TestCase):
    """Test cases for Python bindings"""

    def test_import_success(self):
        """Test that all classes can be imported successfully"""
        # Test that all expected classes are available
        self.assertTrue(hasattr(pytorch_tokenizers, "Error"))
        self.assertTrue(hasattr(pytorch_tokenizers, "TokenIndex"))
        self.assertTrue(hasattr(pytorch_tokenizers, "Tokenizer"))
        self.assertTrue(hasattr(pytorch_tokenizers, "CppHFTokenizer"))
        self.assertTrue(hasattr(pytorch_tokenizers, "CppTiktoken"))
        self.assertTrue(hasattr(pytorch_tokenizers, "CppLlama2cTokenizer"))
        self.assertTrue(hasattr(pytorch_tokenizers, "CppSPTokenizer"))

    def test_error_enum(self):
        """Test Error enum values"""
        self.assertTrue(hasattr(pytorch_tokenizers.Error, "Ok"))
        self.assertTrue(hasattr(pytorch_tokenizers.Error, "Internal"))
        self.assertTrue(hasattr(pytorch_tokenizers.Error, "Uninitialized"))
        self.assertTrue(hasattr(pytorch_tokenizers.Error, "OutOfRange"))
        self.assertTrue(hasattr(pytorch_tokenizers.Error, "LoadFailure"))
        self.assertTrue(hasattr(pytorch_tokenizers.Error, "EncodeFailure"))
        self.assertTrue(hasattr(pytorch_tokenizers.Error, "Base64DecodeFailure"))
        self.assertTrue(hasattr(pytorch_tokenizers.Error, "ParseFailure"))
        self.assertTrue(hasattr(pytorch_tokenizers.Error, "DecodeFailure"))
        self.assertTrue(hasattr(pytorch_tokenizers.Error, "RegexFailure"))

    def test_tokenizer_creation(self):
        """Test that tokenizers can be created"""
        # Test HFTokenizer creation
        hf_tokenizer = pytorch_tokenizers.CppHFTokenizer()
        self.assertIsInstance(hf_tokenizer, pytorch_tokenizers.CppHFTokenizer)
        self.assertFalse(hf_tokenizer.is_loaded())

        # Test Tiktoken creation
        tiktoken_tokenizer = pytorch_tokenizers.CppTiktoken()
        self.assertIsInstance(tiktoken_tokenizer, pytorch_tokenizers.CppTiktoken)
        self.assertFalse(tiktoken_tokenizer.is_loaded())

        # Test Llama2cTokenizer creation
        llama2c_tokenizer = pytorch_tokenizers.CppLlama2cTokenizer()
        self.assertIsInstance(llama2c_tokenizer, pytorch_tokenizers.CppLlama2cTokenizer)
        self.assertFalse(llama2c_tokenizer.is_loaded())

        # Test SPTokenizer creation
        sp_tokenizer = pytorch_tokenizers.CppSPTokenizer()
        self.assertIsInstance(sp_tokenizer, pytorch_tokenizers.CppSPTokenizer)
        self.assertFalse(sp_tokenizer.is_loaded())

    def test_tokenizer_methods(self):
        """Test that tokenizer methods exist and behave correctly for unloaded tokenizers"""
        hf_tokenizer = pytorch_tokenizers.CppHFTokenizer()

        # Test basic properties
        self.assertEqual(hf_tokenizer.vocab_size(), 0)
        self.assertEqual(hf_tokenizer.bos_tok(), 0)
        self.assertEqual(hf_tokenizer.eos_tok(), 0)
        self.assertFalse(hf_tokenizer.is_loaded())

        # Test that encode fails with unloaded tokenizer
        with self.assertRaises(RuntimeError):
            hf_tokenizer.encode("Hello world", 1, 1)

        # Test that decode fails with unloaded tokenizer
        with self.assertRaises(RuntimeError):
            hf_tokenizer.decode(1)

    def test_version(self):
        """Test that version is available"""
        self.assertTrue(hasattr(pytorch_tokenizers, "__version__"))
        self.assertEqual(pytorch_tokenizers.__version__, "0.1.0")

    def test_hf_tokenizer_encode_decode(self):
        """Test HFTokenizer with test_hf_tokenizer.json to encode/decode 'Hello world!'"""
        # Get the path to the test tokenizer file
        tokenizer_path = os.path.join(
            os.path.dirname(__file__), "resources/test_hf_tokenizer.json"
        )
        print(tokenizer_path)

        # Create and load the tokenizer
        hf_tokenizer = pytorch_tokenizers.CppHFTokenizer()
        self.assertFalse(hf_tokenizer.is_loaded())

        # Load the tokenizer from JSON file
        hf_tokenizer.load(tokenizer_path)
        self.assertTrue(hf_tokenizer.is_loaded())

        # Test encoding "Hello world!"
        text = "Hello world!"
        encoded_tokens = hf_tokenizer.encode(text, 1, 0)  # bos=1, eos=0
        self.assertIsInstance(encoded_tokens, list)
        self.assertGreater(len(encoded_tokens), 0)

        # Test decoding the encoded tokens
        for token_id in encoded_tokens:
            decoded_text = hf_tokenizer.decode(token_id)
            self.assertIsInstance(decoded_text, str)

        # Test that we can get vocab size
        vocab_size = hf_tokenizer.vocab_size()
        self.assertGreater(vocab_size, 0)

        # Test BOS and EOS tokens
        bos_token = hf_tokenizer.bos_tok()
        eos_token = hf_tokenizer.eos_tok()
        self.assertIsInstance(bos_token, int)
        self.assertIsInstance(eos_token, int)
