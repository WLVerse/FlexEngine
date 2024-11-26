// WLVERSE [https://wlverse.web.app]
// date.cpp
// 
// Wrapper for date and time functions to make them easy to use.
// 
// AUTHORS
// [100%] Chan Wen Loong (wenloong.c\@digipen.edu)
//   - Main Author
// 
// Copyright (c) 2024 DigiPen, All rights reserved.

#include "datetime.h"

#include <sstream>
#include <iomanip>

namespace FlexEngine
{

  Date::Date()
  {
    auto time = DateTime::GetDateTime();
    year = time.tm_year + 1900;
    month = time.tm_mon + 1;
    day = time.tm_mday;
  }

  Date::Date(int year, int month, int day)
    : year(year), month(month), day(day)
  {
  }

  Date Date::Now()
  {
    return Date();
  }

  #pragma region Operator Overloads

  bool Date::operator==(const Date& other) const
  {
    return year == other.year && month == other.month && day == other.day;
  }

  bool Date::operator!=(const Date& other) const
  {
    return !(*this == other);
  }

  bool Date::operator<(const Date& other) const
  {
    if (year < other.year) return true;
    if (year > other.year) return false;
    if (month < other.month) return true;
    if (month > other.month) return false;
    return day < other.day;
  }

  bool Date::operator>(const Date& other) const
  {
    if (year > other.year) return true;
    if (year < other.year) return false;
    if (month > other.month) return true;
    if (month < other.month) return false;
    return day > other.day;
  }

  bool Date::operator<=(const Date& other) const
  {
    return *this < other || *this == other;
  }

  bool Date::operator>=(const Date& other) const
  {
    return *this > other || *this == other;
  }

  #pragma endregion

  #pragma region Stringify

  // Converts the date to a string in the format: yyyy-mm-dd
  std::string Date::ToString() const
  {
    std::stringstream ss;
    ss
      << std::setw(4) << std::setfill('0') << year  << "-"
      << std::setw(2) << std::setfill('0') << month << "-"
      << std::setw(2) << std::setfill('0') << day
    ;
    return ss.str();
  }

  Date::operator std::string() const
  {
    return ToString();
  }

  #pragma endregion

}