/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2014 Regents of the University of California.
 *
 * This file is part of ndn-cxx library (NDN C++ library with eXperimental eXtensions).
 *
 * ndn-cxx library is free software: you can redistribute it and/or modify it under the
 * terms of the GNU Lesser General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later version.
 *
 * ndn-cxx library is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
 * PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
 *
 * You should have received copies of the GNU General Public License and GNU Lesser
 * General Public License along with ndn-cxx, e.g., in COPYING.md file.  If not, see
 * <http://www.gnu.org/licenses/>.
 *
 * See AUTHORS.md for complete list of ndn-cxx authors and contributors.
 */


#include "encoding/encoding-buffer.hpp"
#include "encoding/buffer-stream.hpp"

#include "boost-test.hpp"

using namespace std;
namespace ndn {

BOOST_AUTO_TEST_SUITE(TestBlock)

BOOST_AUTO_TEST_CASE(BlockBasic)
{
  EncodingBuffer buffer;
  EncodingEstimator estimator;
  size_t s1, s2;

  // VarNumber checks

  s1 = buffer.prependVarNumber(252);
  s2 = estimator.prependVarNumber(252);
  BOOST_CHECK_EQUAL(buffer.size(), 1);
  BOOST_CHECK_EQUAL(s1, 1);
  BOOST_CHECK_EQUAL(s2, 1);
  buffer = EncodingBuffer();

  s1 = buffer.prependVarNumber(253);
  s2 = estimator.prependVarNumber(253);
  BOOST_CHECK_EQUAL(buffer.size(), 3);
  BOOST_CHECK_EQUAL(s1, 3);
  BOOST_CHECK_EQUAL(s2, 3);
  buffer = EncodingBuffer();

  s1 = buffer.prependVarNumber(255);
  s2 = estimator.prependVarNumber(255);
  BOOST_CHECK_EQUAL(buffer.size(), 3);
  BOOST_CHECK_EQUAL(s1, 3);
  BOOST_CHECK_EQUAL(s2, 3);
  buffer = EncodingBuffer();

  s1 = buffer.prependVarNumber(65535);
  s2 = estimator.prependVarNumber(65535);
  BOOST_CHECK_EQUAL(buffer.size(), 3);
  BOOST_CHECK_EQUAL(s1, 3);
  BOOST_CHECK_EQUAL(s2, 3);
  buffer = EncodingBuffer();

  s1 = buffer.prependVarNumber(65536);
  s2 = estimator.prependVarNumber(65536);
  BOOST_CHECK_EQUAL(buffer.size(), 5);
  BOOST_CHECK_EQUAL(s1, 5);
  BOOST_CHECK_EQUAL(s2, 5);
  buffer = EncodingBuffer();

  s1 = buffer.prependVarNumber(4294967295LL);
  s2 = estimator.prependVarNumber(4294967295LL);
  BOOST_CHECK_EQUAL(buffer.size(), 5);
  BOOST_CHECK_EQUAL(s1, 5);
  BOOST_CHECK_EQUAL(s2, 5);
  buffer = EncodingBuffer();

  s1 = buffer.prependVarNumber(4294967296LL);
  s2 = estimator.prependVarNumber(4294967296LL);
  BOOST_CHECK_EQUAL(buffer.size(), 9);
  BOOST_CHECK_EQUAL(s1, 9);
  BOOST_CHECK_EQUAL(s2, 9);
  buffer = EncodingBuffer();

  // nonNegativeInteger checks

  s1 = buffer.prependNonNegativeInteger(252);
  s2 = estimator.prependNonNegativeInteger(252);
  BOOST_CHECK_EQUAL(buffer.size(), 1);
  BOOST_CHECK_EQUAL(s1, 1);
  BOOST_CHECK_EQUAL(s2, 1);
  buffer = EncodingBuffer();

  s1 = buffer.prependNonNegativeInteger(255);
  s2 = estimator.prependNonNegativeInteger(255);
  BOOST_CHECK_EQUAL(buffer.size(), 1);
  BOOST_CHECK_EQUAL(s1, 1);
  BOOST_CHECK_EQUAL(s2, 1);
  buffer = EncodingBuffer();

  s1 = buffer.prependNonNegativeInteger(256);
  s2 = estimator.prependNonNegativeInteger(256);
  BOOST_CHECK_EQUAL(buffer.size(), 2);
  BOOST_CHECK_EQUAL(s1, 2);
  BOOST_CHECK_EQUAL(s2, 2);
  buffer = EncodingBuffer();

  s1 = buffer.prependNonNegativeInteger(65535);
  s2 = estimator.prependNonNegativeInteger(65535);
  BOOST_CHECK_EQUAL(buffer.size(), 2);
  BOOST_CHECK_EQUAL(s1, 2);
  BOOST_CHECK_EQUAL(s2, 2);
  buffer = EncodingBuffer();

  s1 = buffer.prependNonNegativeInteger(65536);
  s2 = estimator.prependNonNegativeInteger(65536);
  BOOST_CHECK_EQUAL(buffer.size(), 4);
  BOOST_CHECK_EQUAL(s1, 4);
  BOOST_CHECK_EQUAL(s2, 4);
  buffer = EncodingBuffer();

  s1 = buffer.prependNonNegativeInteger(4294967295LL);
  s2 = estimator.prependNonNegativeInteger(4294967295LL);
  BOOST_CHECK_EQUAL(buffer.size(), 4);
  BOOST_CHECK_EQUAL(s1, 4);
  BOOST_CHECK_EQUAL(s2, 4);
  buffer = EncodingBuffer();

  s1 = buffer.prependNonNegativeInteger(4294967296LL);
  s2 = estimator.prependNonNegativeInteger(4294967296LL);
  BOOST_CHECK_EQUAL(buffer.size(), 8);
  BOOST_CHECK_EQUAL(s1, 8);
  BOOST_CHECK_EQUAL(s2, 8);
  buffer = EncodingBuffer();
}

BOOST_AUTO_TEST_CASE(EncodingBufferToBlock)
{
  uint8_t value[4];

  EncodingBuffer buffer;
  size_t length = buffer.prependByteArray(value, sizeof(value));
  buffer.prependVarNumber(length);
  buffer.prependVarNumber(0xe0);

  Block block;
  BOOST_REQUIRE_NO_THROW(block = buffer.block());
  BOOST_CHECK_EQUAL(block.type(), 0xe0);
  BOOST_CHECK_EQUAL(block.value_size(), sizeof(value));

  BOOST_REQUIRE_NO_THROW(block = Block(buffer));
  BOOST_CHECK_EQUAL(block.type(), 0xe0);
  BOOST_CHECK_EQUAL(block.value_size(), sizeof(value));
}

BOOST_AUTO_TEST_CASE(BlockToBuffer)
{
  shared_ptr<Buffer> buf = make_shared<Buffer>(10);
  for (int i = 0; i < 10; i++) (*buf)[i] = i;

  Block block(0xab, buf);
  block.encode();

  EncodingBuffer buffer(0,0);
  BOOST_REQUIRE_NO_THROW(buffer = EncodingBuffer(block));
  BOOST_CHECK_EQUAL(buffer.size(), 12);
  BOOST_CHECK_EQUAL(buffer.capacity(), 12);

  (*buf)[1] = 0xe0;
  (*buf)[2] = 2;
  BOOST_REQUIRE_NO_THROW(block = Block(buf, buf->begin() + 1, buf->begin() + 5));
  BOOST_CHECK_EQUAL(block.type(), 0xe0);

  BOOST_REQUIRE_NO_THROW(buffer = EncodingBuffer(block));
  BOOST_CHECK_EQUAL(buffer.size(), 4);
  BOOST_CHECK_EQUAL(buffer.capacity(), 10);
}

BOOST_AUTO_TEST_CASE(FromBuffer)
{
  const uint8_t TEST_BUFFER[] = {0x00, 0x01, 0xfa, // ok
                                 0x01, 0x01, 0xfb, // ok
                                 0x03, 0x02, 0xff}; // not ok
  BufferPtr buffer(new Buffer(TEST_BUFFER, sizeof(TEST_BUFFER)));

  // using BufferPtr (avoids memory copy)
  size_t offset = 0;
  Block testBlock;
  BOOST_CHECK(Block::fromBuffer(buffer, offset, testBlock));
  BOOST_CHECK_EQUAL(testBlock.type(), 0);
  BOOST_CHECK_EQUAL(testBlock.size(), 3);
  BOOST_CHECK_EQUAL(testBlock.value_size(), 1);
  BOOST_CHECK_EQUAL(*testBlock.wire(),  0x00);
  BOOST_CHECK_EQUAL(*testBlock.value(), 0xfa);
  offset += testBlock.size();

  BOOST_CHECK(Block::fromBuffer(buffer, offset, testBlock));
  BOOST_CHECK_EQUAL(testBlock.type(), 1);
  BOOST_CHECK_EQUAL(testBlock.size(), 3);
  BOOST_CHECK_EQUAL(testBlock.value_size(), 1);
  BOOST_CHECK_EQUAL(*testBlock.wire(),  0x01);
  BOOST_CHECK_EQUAL(*testBlock.value(), 0xfb);
  offset += testBlock.size();

  BOOST_CHECK(!Block::fromBuffer(buffer, offset, testBlock));

  // just buffer, copies memory
  offset = 0;
  BOOST_CHECK(Block::fromBuffer(TEST_BUFFER + offset, sizeof(TEST_BUFFER) - offset, testBlock));
  BOOST_CHECK_EQUAL(testBlock.type(), 0);
  BOOST_CHECK_EQUAL(testBlock.size(), 3);
  BOOST_CHECK_EQUAL(testBlock.value_size(), 1);
  BOOST_CHECK_EQUAL(*testBlock.wire(),  0x00);
  BOOST_CHECK_EQUAL(*testBlock.value(), 0xfa);
  offset += testBlock.size();

  BOOST_CHECK(Block::fromBuffer(TEST_BUFFER + offset, sizeof(TEST_BUFFER) - offset, testBlock));
  BOOST_CHECK_EQUAL(testBlock.type(), 1);
  BOOST_CHECK_EQUAL(testBlock.size(), 3);
  BOOST_CHECK_EQUAL(testBlock.value_size(), 1);
  BOOST_CHECK_EQUAL(*testBlock.wire(),  0x01);
  BOOST_CHECK_EQUAL(*testBlock.value(), 0xfb);
  offset += testBlock.size();

  BOOST_CHECK(!Block::fromBuffer(TEST_BUFFER + offset, sizeof(TEST_BUFFER) - offset, testBlock));
}

BOOST_AUTO_TEST_CASE(BlockFromStream)
{
  const uint8_t TEST_BUFFER[] = {0x00, 0x01, 0xfa, // ok
                                 0x01, 0x01, 0xfb, // ok
                                 0x03, 0x02, 0xff}; // not ok

  typedef boost::iostreams::stream<boost::iostreams::array_source> ArrayStream;
  ArrayStream stream(reinterpret_cast<const char*>(TEST_BUFFER), sizeof(TEST_BUFFER));


  Block testBlock;
  BOOST_REQUIRE_NO_THROW(testBlock = Block::fromStream(stream));
  BOOST_CHECK_EQUAL(testBlock.type(), 0);
  BOOST_CHECK_EQUAL(testBlock.size(), 3);
  BOOST_CHECK_EQUAL(testBlock.value_size(), 1);
  BOOST_CHECK_EQUAL(*testBlock.wire(),  0x00);
  BOOST_CHECK_EQUAL(*testBlock.value(), 0xfa);

  BOOST_REQUIRE_NO_THROW(testBlock = Block::fromStream(stream));
  BOOST_CHECK_EQUAL(testBlock.type(), 1);
  BOOST_CHECK_EQUAL(testBlock.size(), 3);
  BOOST_CHECK_EQUAL(testBlock.value_size(), 1);
  BOOST_CHECK_EQUAL(*testBlock.wire(),  0x01);
  BOOST_CHECK_EQUAL(*testBlock.value(), 0xfb);

  BOOST_CHECK_THROW(Block::fromStream(stream), tlv::Error);
}

BOOST_AUTO_TEST_CASE(Equality)
{
  BOOST_CONCEPT_ASSERT((boost::EqualityComparable<Block>));

  Block a("\x08\x00", 2);
  Block b("\x08\x00", 2);;
  BOOST_CHECK_EQUAL(a == b, true);
  BOOST_CHECK_EQUAL(a != b, false);

  Block c("\x06\x00", 2);
  Block d("\x08\x00", 2);;
  BOOST_CHECK_EQUAL(c == d, false);
  BOOST_CHECK_EQUAL(c != d, true);

  Block e("\x06\x00", 2);
  Block f("\x06\x01\xcc", 3);;
  BOOST_CHECK_EQUAL(e == f, false);
  BOOST_CHECK_EQUAL(e != f, true);
}

BOOST_AUTO_TEST_SUITE_END()

} // namespace ndn
