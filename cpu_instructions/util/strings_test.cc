#include "cpu_instructions/util/strings.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "strings/str_cat.h"
#include "util/task/canonical_errors.h"

namespace cpu_instructions {
namespace {

using ::testing::ElementsAreArray;
using util::Status;
using util::StatusOr;

void CheckInput(const string& hex_string,
                const std::vector<uint8_t>& expected_bytes) {
  SCOPED_TRACE(StrCat("hex_string = ", hex_string));
  const StatusOr<std::vector<uint8_t>> status_or_bytes =
      ParseHexString(hex_string);
  ASSERT_OK(status_or_bytes.status());

  EXPECT_THAT(status_or_bytes.ValueOrDie(), ElementsAreArray(expected_bytes));
}

void CheckError(const string& hex_string,
                const string& expected_unparsed_part) {
  SCOPED_TRACE(StrCat("hex_string = ", hex_string));
  const StatusOr<std::vector<uint8_t>> status_or_bytes =
      ParseHexString(hex_string);
  ASSERT_FALSE(status_or_bytes.ok());
  const Status& status = status_or_bytes.status();
  EXPECT_TRUE(util::IsInvalidArgument(status));
  EXPECT_EQ(StrCat("Could not parse: ", expected_unparsed_part),
            status.error_message());
}

TEST(ParseHexStringTest, TestEmptyString) { CheckInput("", {}); }

TEST(ParseHexStringTest, TestIntelManual) {
  CheckInput("AB BA F0 00", {0xab, 0xba, 0xf0, 0x0});
}

TEST(ParseHexStringTest, TestLowerCaseIntelManual) {
  CheckInput("ab ba f0 00", {0xab, 0xba, 0xf0, 0x0});
}

TEST(ParseHexStringTest, TestCPPArrayForamt) {
  CheckInput("0x0, 0x1, 0x2, 0xab", {0x0, 0x1, 0x2, 0xab});
}

TEST(ParseHexStringTest, TestCPPArrayWithNoSpaces) {
  CheckInput("0x0,0x1,0x2,0xab", {0x0, 0x1, 0x2, 0xab});
}

TEST(ParseHexStringTest, TestIntelManualWithCommas) {
  CheckInput("00,aB,Ba,cD, FF c0", {0x0, 0xab, 0xba, 0xcd, 0xff, 0xc0});
}

TEST(ParseHexStringTest, TestNonHexString) {
  CheckError("I'm not a hex string", "I'm not a hex string");
}

TEST(ParseHexStringTest, TestValidPrefix) {
  CheckError("0F AB blabla", "labla");
}

TEST(ToHumanReadableHexStringTest, EmptyVector) {
  const std::vector<uint8_t> kEmptyVector;
  EXPECT_TRUE(ToHumanReadableHexString(kEmptyVector).empty());
}

TEST(ToHumanReadableHexStringTest, FromVector) {
  const std::vector<uint8_t> kBinaryData = {0xab, 0xba, 0x1, 0x0};
  constexpr char kExpectedString[] = "AB BA 01 00";
  EXPECT_EQ(kExpectedString, ToHumanReadableHexString(kBinaryData));
}

TEST(ToHumanReadableHexStringTest, FromArray) {
  constexpr uint8_t kBinaryData[] = {0xab, 0xba, 0x1, 0x0};
  constexpr char kExpectedString[] = "AB BA 01 00";
  EXPECT_EQ(kExpectedString, ToHumanReadableHexString(kBinaryData));
}

TEST(ToPastableHexStringTest, EmptyVector) {
  const std::vector<uint8_t> kEmptyVector;
  EXPECT_TRUE(ToPastableHexString(kEmptyVector).empty());
}

TEST(ToPastableHexStringTest, FromVector) {
  const std::vector<uint8_t> kBinaryData = {0xab, 0xba, 0x1, 0x0};
  constexpr char kExpectedString[] = "0xAB, 0xBA, 0x01, 0x00";
  EXPECT_EQ(kExpectedString, ToPastableHexString(kBinaryData));
}

TEST(ToPastableHexStringTest, FromArray) {
  constexpr uint8_t kBinaryData[] = {0xab, 0xba, 0x1, 0x0};
  constexpr char kExpectedString[] = "0xAB, 0xBA, 0x01, 0x00";
  EXPECT_EQ(kExpectedString, ToPastableHexString(kBinaryData));
}

}  // namespace
}  // namespace cpu_instructions
