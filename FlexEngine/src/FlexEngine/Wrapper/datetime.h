// WLVERSE [https://wlverse.web.app]
// datetime.h
// 
// Wrapper for date and time functions to make them easy to use.
// 
// AUTHORS
// [100%] Chan Wen Loong (wenloong.c\@digipen.edu)
//   - Main Author
// 
// Copyright (c) 2024 DigiPen, All rights reserved.

#pragma once

#include "flx_api.h"

#include <iostream>
#include <chrono>
#include <string>

namespace FlexEngine
{

  #pragma region Date

  // Date structure that holds the year, month, and day.
  // Useful for just storing the date without the time.
  struct __FLX_API Date
  {
    int year = 1900;
    int month = 0;
    int day = 0;

    #pragma region Enums

    // Allows implicit conversion to int.
    enum __FLX_API Month
    {
      January = 1,
      February,
      March,
      April,
      May,
      June,
      July,
      August,
      September,
      October,
      November,
      December,
    };

    #pragma endregion

    // Default constructor that initializes the date with the current date.
    Date();

    // Parameterized constructor that initializes the date with the given year, month, and day.
    Date(int year, int month, int day);

    // Gets the current date.
    static Date Now();

    #pragma region Operator Overloads

    bool operator==(const Date& other) const;
    bool operator!=(const Date& other) const;
    bool operator<(const Date& other) const;
    bool operator<=(const Date& other) const;
    bool operator>(const Date& other) const;
    bool operator>=(const Date& other) const;

    #pragma endregion

    #pragma region Stringify

    // Converts the date to a string in the format: yyyy-mm-dd
    std::string ToString() const;

    // Conversion operator to std::string
    operator std::string() const;

    #pragma endregion

  };

  #pragma endregion

  // Ease of use wrapper class that provides date and time functionality.
  // Format the date using the GetFormattedDateTime method.
  class __FLX_API DateTime
  {
  public:
    // remove constructors
    DateTime() = delete;
    DateTime(const DateTime&) = delete;
    DateTime(DateTime&&) = delete;
    DateTime& operator=(const DateTime&) = delete;
    DateTime& operator=(DateTime&&) = delete;

    // Gets the date in std::tm format.
    static std::tm GetDateTime();

    // Gets the formatted date in the string.
    static std::string GetFormattedDateTime(const char* fmt = "%Y-%m-%d %X");
  };

}