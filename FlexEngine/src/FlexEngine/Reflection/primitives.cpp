// WLVERSE [https://wlverse.web.app]
// primitives.cpp
// 
// TypeDescriptor for primitive types.
// Supports the following types:
// - bool
// - int
// - unsigned
// - int64_t
// - uint64_t
// - double
// - float
// - std::string
//
// AUTHORS
// [100%] Chan Wen Loong (wenloong.c\@digipen.edu)
//   - Main Author
// 
// Copyright (c) 2025 DigiPen, All rights reserved.

#include "Reflection/base.h"

#pragma region Macros

// TypeDescriptor for primitive types
// Supports the same types that rapidjson supports except const char*
// Abstracted for easy editing
#define TYPE_DESCRIPTOR(NAME, TYPE) \
  struct __FLX_API TypeDescriptor_##NAME : TypeDescriptor \
  { \
    TypeDescriptor_##NAME() : TypeDescriptor{ #TYPE, sizeof(TYPE) } {} \
    virtual void Dump(const void* obj, std::ostream& os, int) const override \
    { \
      os << #TYPE << "{" << *(const TYPE*)obj << "}"; \
    } \
    virtual void Serialize(const void* obj, std::ostream& os) const override \
    { \
      os << R"({"type":")" << #TYPE << R"(","data":)" << *(const TYPE*)obj << "}"; \
    } \
    virtual void Deserialize(void* obj, const json& value) const override \
    { \
      TYPE data = value["data"].Get<TYPE>(); \
      *(TYPE*)obj = data; \
      /**reinterpret_cast<TYPE*>(obj) = data;*/ \
    } \
  }; \
  template <> \
  __FLX_API TypeDescriptor* GetPrimitiveDescriptor<TYPE>() \
  { \
    static TypeDescriptor_##NAME type_desc; \
    if (TYPE_DESCRIPTOR_LOOKUP.count(type_desc.name) == 0) \
    { \
      /* Register the type descriptor in the lookup table. */ \
      TYPE_DESCRIPTOR_LOOKUP[type_desc.name] = &type_desc; \
    } \
    return &type_desc; \
  }

#pragma endregion



namespace FlexEngine
{

  namespace Reflection
  {

    #pragma region TypeDescriptor Reflection

    FLX_REFL_REGISTER_START(TypeDescriptor)
      FLX_REFL_REGISTER_PROPERTY(name)
      FLX_REFL_REGISTER_PROPERTY(size)
    FLX_REFL_REGISTER_END;

    #pragma endregion

    // Primitive type registration
    //TYPE_DESCRIPTOR(Bool, bool) // specialized below
    TYPE_DESCRIPTOR(Int, int)
    TYPE_DESCRIPTOR(Unsigned, unsigned)
    TYPE_DESCRIPTOR(LongLong, int64_t) // long long
    TYPE_DESCRIPTOR(UnsignedLongLong, uint64_t) // unsigned long long
    TYPE_DESCRIPTOR(Double, double)
    TYPE_DESCRIPTOR(Float, float)
    // no support for const char*, just use std::string
    //TYPE_DESCRIPTOR(StdString, std::string) // specialized below


    // TypeDescriptor specialization for bool.
    // Even though this is a primitive type, a specialization for it
    // is necessary because it is represented by TRUE/FALSE in the json format.
    struct __FLX_API TypeDescriptor_Bool : TypeDescriptor
    {
      TypeDescriptor_Bool()
        : TypeDescriptor{ "bool", sizeof(bool) }
      {
      }
      virtual void Dump(const void* obj, std::ostream& os, int) const override
      {
        os << "bool" << "{" << *(const bool*)obj << "}";
      }
      virtual void Serialize(const void* obj, std::ostream& os) const override
      {
        os << R"({"type":")" << "bool" << R"(","data":)" << ((*(const bool*)obj) ? "true" : "false") << "}";
      }
      virtual void Deserialize(void* obj, const json& value) const override
      {
        bool data = value["data"].Get<bool>(); *(bool*)obj = data;
      }
    };
    template <>
    __FLX_API TypeDescriptor* GetPrimitiveDescriptor<bool>()
    {
      static TypeDescriptor_Bool type_desc;
      if (TYPE_DESCRIPTOR_LOOKUP.count(type_desc.name) == 0)
      {
        TYPE_DESCRIPTOR_LOOKUP[type_desc.name] = &type_desc;
      }
      return &type_desc;
    }



    // TypeDescriptor specialization for std::string.
    // Even though this is a primitive type, a specialization for it
    // is necessary because it needs to be wrapped by "" in the json format.
    struct __FLX_API TypeDescriptor_StdString : TypeDescriptor
    {
      TypeDescriptor_StdString()
        : TypeDescriptor{ "std::string", sizeof(std::string) }
      {
      }

      virtual void Dump(const void* obj, std::ostream& os, int) const override
      {
        os << "std::string" << "{" << *(const std::string*)obj << "}";
      }
      virtual void Serialize(const void* obj, std::ostream& os) const override
      {
        std::string data = *(const std::string*)obj;
        std::ostringstream escaped_data;

        // Transform and escape special characters
        for (char c : data)
        {
          switch (c)
          {
          case '\\': escaped_data << "\\\\"; break;
          case '\"': escaped_data << "\\\""; break;
          case '\b': escaped_data << "\\b"; break;
          case '\f': escaped_data << "\\f"; break;
          case '\n': escaped_data << "\\n"; break;
          case '\r': escaped_data << "\\r"; break;
          case '\t': escaped_data << "\\t"; break;
          default:
            // Escape non-printable ASCII characters
            if (static_cast<unsigned char>(c) < 0x20 || static_cast<unsigned char>(c) > 0x7E)
            {
              escaped_data << "\\u"
                << std::hex << std::setw(4) << std::setfill('0')
                << static_cast<int>(static_cast<unsigned char>(c));
            }
            else
            {
              escaped_data << c;
            }
            break;
          }
        }

        // Serialize
        os << R"({"type":")" << "std::string" << R"(","data":")" << escaped_data.str() << R"("})";
      }
      virtual void Deserialize(void* obj, const json& value) const override
      {
        std::string data = value["data"].Get<std::string>();
        std::ostringstream unescaped_data;

        // Transform and unescape special characters
        for (size_t i = 0; i < data.size(); ++i)
        {
          if (data[i] == '\\' && i + 1 < data.size())
          {
            switch (data[++i])
            {
            case '\\': unescaped_data << '\\'; break;
            case '\"': unescaped_data << '\"'; break;
            case 'b':  unescaped_data << '\b'; break;
            case 'f':  unescaped_data << '\f'; break;
            case 'n':  unescaped_data << '\n'; break;
            case 'r':  unescaped_data << '\r'; break;
            case 't':  unescaped_data << '\t'; break;
            case 'u':
            {
              // Parse Unicode escape sequence (e.g., \uXXXX)
              if (i + 4 < data.size()) {
                std::string hex = data.substr(i + 1, 4);
                char32_t unicode_char = static_cast<char32_t>(std::stoi(hex, nullptr, 16));
                unescaped_data << static_cast<char>(unicode_char); // Assuming ASCII subset for simplicity
                i += 4;
              }
              else {
                throw std::runtime_error("Invalid Unicode escape sequence in JSON string.");
              }
              break;
            }
            default:
              unescaped_data << '\\' << data[i]; // Preserve unexpected escape sequences
              break;
            }
          }
          else
          {
            unescaped_data << data[i];
          }
        }

        *(std::string*)obj = unescaped_data.str();
      }
    };
    template <>
    __FLX_API TypeDescriptor* GetPrimitiveDescriptor<std::string>()
    {
      static TypeDescriptor_StdString type_desc;
      if (TYPE_DESCRIPTOR_LOOKUP.count(type_desc.name) == 0)
      {
        TYPE_DESCRIPTOR_LOOKUP[type_desc.name] = &type_desc;
      }
      return &type_desc;
    }

  }

}
