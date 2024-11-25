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
        // Escape all `\` characters in the string.
        std::string data = *(const std::string*)obj;
        for (size_t i = 0; i < data.size(); ++i)
        {
          if (data[i] == '\\')
          {
            data.insert(i, "\\");
            ++i;
          }
        }

        // Serialize
        os << R"({"type":")" << "std::string" << R"(","data":")" << data << R"("})";
      }
      virtual void Deserialize(void* obj, const json& value) const override
      {
        std::string data = value["data"].Get<std::string>();

        // Unescape all `\\` characters in the string.
        for (size_t i = 0; i < data.size(); ++i)
        {
          if (data[i] == '\\' && i + 1 < data.size() && data[i + 1] == '\\')
          {
            data.erase(i, 1);
          }
        }

        *(std::string*)obj = data;
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


    // TypeDescriptor specialization for void*.
    // This is a primitive type, but it is not supported by the json format.
    // This specialization is necessary for serialization/deserialization of void*.
    struct __FLX_API TypeDescriptor_VoidPtr : TypeDescriptor
    {
      TypeDescriptor_VoidPtr()
        : TypeDescriptor{ "void*", sizeof(void*) }
      {
      }

      virtual void Dump(const void* obj, std::ostream& os, int) const override
      {
        if (obj)
        {
          os << "void*" << "{" << obj << "}";
        }
        else
        {
          os << "void*" << "{null}";
        }
      }
      virtual void Serialize(const void* obj, std::ostream& os) const override
      {
        if (obj)
        {
          // Obtain the raw pointer and serialize it
          // 
          // How this works:
          // Since the shared_ptr is a void pointer, we can't get the size directly.
          // The serialized_str needs to be constructed from the raw pointer which needs the full size.
          // Thus, the shared_ptr stores the size of the data in the first sizeof(std::size_t) == 4 or 8 bytes.
          // This allows us to get the size of the data, add sizeof(std::size_t), which gives us the full size.
          // Implemented in FlexECS::Internal_GetComponentData.

          void* ptr = const_cast<void*>(obj);
          std::size_t data_size = *static_cast<std::size_t*>(ptr);
          BYTE* byte_ptr = static_cast<BYTE*>(ptr);

          // Get the data as a std::vector<BYTE>
          // We get everything to encode the full data
          std::vector<BYTE> data(
            byte_ptr,
            byte_ptr + sizeof(std::size_t) + data_size
          );

          // Encode the data
          std::string serialized_data = Base64::Encode(data);

          // Serialize as a json string
          os << R"({"type":")" << "void*" << R"(","data":")" << serialized_data << R"("})";
        }
        else
        {
          os << "null";
        }
      }
      virtual void Deserialize(void* obj, const json& value) const override
      {
        if (value.IsNull())
        {
          // Set the ptr to null
          obj = nullptr;
        }
        else
        {
          // Deserialize as a json string
          std::string data = value["data"].Get<std::string>();

          // Decode the string
          // The decoded data will be in the format: std::size_t + data
          std::vector<BYTE> decoded_data = Base64::Decode(data);

          // Get the size
          void* void_ptr = reinterpret_cast<void*>(decoded_data.data());
          std::size_t ptr_size = *static_cast<std::size_t*>(void_ptr);

          // Allocate memory for the data
          void* ptr = new char[sizeof(std::size_t) + ptr_size];

          // Copy the data
          memcpy(ptr, void_ptr, sizeof(std::size_t) + ptr_size);

          obj = ptr;
        }
      }
    };
    template <>
    __FLX_API TypeDescriptor* GetPrimitiveDescriptor<void*>()
    {
      static TypeDescriptor_VoidPtr type_desc;
      if (TYPE_DESCRIPTOR_LOOKUP.count(type_desc.name) == 0)
      {
        TYPE_DESCRIPTOR_LOOKUP[type_desc.name] = &type_desc;
      }
      return &type_desc;
    }

  }

}
