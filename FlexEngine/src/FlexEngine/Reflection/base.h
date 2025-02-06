// WLVERSE [https://wlverse.web.app]
// base.h
//
// Reflection system for C++ 
// 
// A reflection system (also called a property system) is a coding methodology or
// language feature which allows exposing certain information about your code to
// other systems at runtime. 
// This is often used in serialization, GUIs, and other systems. 
// 
// I use some of the tricks from the following articles to implement this. 
// However, I have rewritten most of the code to fit my own needs. 
// 
// References: 
//  A Complete Example for C++ Reflection 
//    https://yingnanwang.com/coding/2020-10-05-c-reflection/ 
//  A Flexible Reflection System in C++: Part 1 
//    https://preshing.com/20180116/a-primitive-reflection-system-in-cpp-part-1/ 
//  Unreal Engine Implementation 
//    https://www.unrealengine.com/en-US/blog/unreal-property-system-reflection 
//  properties by LIONant 
//    https://gitlab.com/LIONant/properties/blob/master/docs/Documentation.md 
//  RTTR (Run Time Type Reflection) 
//    https://www.rttr.org/doc/rttr-0-9-6/five_minute_tutorial_page.html 
// 
// 
// These macros are the heart and soul of the reflection system. 
// They allow you to easily register custom types and member variables from
// anywhere in your code. 
// 
// For basic serialization support, you only need to use the following macros: 
//  - FLX_REFL_SERIALIZABLE 
//  - FLX_REFL_REGISTER_START 
//  - FLX_REFL_REGISTER_PROPERTY 
//  - FLX_REFL_REGISTER_END 
//
// AUTHORS
// [100%] Chan Wen Loong (wenloong.c\@digipen.edu)
//   - Main Author
//
// Copyright (c) 2025 DigiPen, All rights reserved.

#pragma once

#include "flx_api.h"

#include "flexassert.h"
#include "Utilities/flexbase64.h"

#include <rapidjson/document.h>
using namespace rapidjson;

#include <cstddef>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <unordered_map>
#include <functional>

#pragma region Macros

#pragma region FLX_REFL_SERIALIZABLE / FLX_REFL_REGISTER_START / FLX_REFL_REGISTER_PROPERTY / FLX_REFL_REGISTER_END

// Enables reflection for a custom type (struct/class)
// Place at the top of the class definition in the .h file
#define FLX_REFL_SERIALIZABLE \
  friend struct FlexEngine::Reflection::DefaultResolver; \
  static FlexEngine::Reflection::TypeDescriptor_Struct Reflection; \
  static void InitReflection(FlexEngine::Reflection::TypeDescriptor_Struct*);

// Starts the registration of member variables for reflection
// Remember to end with FLX_REFL_REGISTER_END
// Place inside any .cpp file that includes the declaration of the custom type
#define FLX_REFL_REGISTER_START(TYPE) \
  FlexEngine::Reflection::TypeDescriptor_Struct TYPE::Reflection{TYPE::InitReflection}; \
  void TYPE::InitReflection(FlexEngine::Reflection::TypeDescriptor_Struct* type_desc) \
  { \
    using T = TYPE; \
    type_desc->name = #TYPE; \
    type_desc->size = sizeof(T); \
    type_desc->members = {

// Registers a member variable for reflection
// Place inside the FLX_REFL_REGISTER_START block
// Use the name of the member variable as the argument
// It's best practice to indent the block for readability
#define FLX_REFL_REGISTER_PROPERTY(VARIABLE) \
      { \
        #VARIABLE, \
        offsetof(T, VARIABLE), \
        FlexEngine::Reflection::TypeResolver<decltype(T::VARIABLE)>::Get() \
      },

// Ends the reflection registration
// Pair this with FLX_REFL_REGISTER_START
#define FLX_REFL_REGISTER_END \
    }; \
    /* Register custom type to the TypeDescriptor lookup */ \
    TYPE_DESCRIPTOR_LOOKUP[type_desc->name] = type_desc; \
  }

#pragma endregion

#pragma endregion


namespace FlexEngine
{

  namespace Reflection
  {

    #pragma region TypeDescriptor

    struct DefaultResolver;
    struct TypeDescriptor_Struct;

    // Base class for all type descriptors.
    // A type descriptor is a class that describes a type,
    // including its name, size, and how to serialize/deserialize it.
    struct __FLX_API TypeDescriptor
    { FLX_REFL_SERIALIZABLE
      using json = rapidjson::Value;

      //const char* name; // The name of the type.
      std::string name; // The name of the type.
      size_t size;      // The size of the type in bytes.


      // Store a umap of all the type descriptors.
      // This is used to deserialize the TypeDescriptor from its name.
      // This is a static member function to avoid the static initialization order fiasco.
      // Just use the macro TYPE_DESCRIPTOR_LOOKUP to access this map.
      static std::unordered_map<std::string, TypeDescriptor*>& type_descriptor_lookup()
      {
        static std::unordered_map<std::string, TypeDescriptor*> type_descriptor_lookup;
        return type_descriptor_lookup;
      }
      // Macro to access the type_descriptor_lookup map.
      // Usage: TYPE_DESCRIPTOR_LOOKUP["int"]
      #define TYPE_DESCRIPTOR_LOOKUP FlexEngine::Reflection::TypeDescriptor::type_descriptor_lookup()


      //TypeDescriptor(const char* name, size_t size) : name{ name }, size{ size } {}
      TypeDescriptor(const std::string& name, size_t size) : name{ name }, size{ size } {}
      virtual ~TypeDescriptor() {}

      // Overload the comparison operator to compare the type name.
      // We need this to sort TypeDescriptors.
      bool operator<(const TypeDescriptor& other) const
      {
        //return std::strcmp(name, other.name) < 0;
        return name < other.name;
      }

      // Get the full name of the type, including any template parameters.
      virtual std::string ToString() const { return name; }

      // Dumps the contents of an object to the console.
      // Defaults to using std::cout.
      virtual void Dump(const void* obj, std::ostream& os = std::cout, int indent_level = 0) const = 0;

      // Serializes an object to a stream.
      // This recursively serializes the object into the json format.
      virtual void Serialize(const void* obj, std::ostream& out) const = 0;

      // Serializes an object to a rapidjson document.
      // This recursively serializes the object into the json format.
      // It is an extension of Serialize() that uses rapidjson.
      virtual void SerializeJson(const void* obj, rapidjson::Document& out) const
      {
        std::stringstream ss;
        Serialize(obj, ss);
        out.Parse(ss.str().c_str());
      }

      // Deserializes an object from a json document.
      // This recursively deserializes the object from the json format
      // The deserializer uses the rapidjson library.
      virtual void Deserialize(void* obj, const json& value) const = 0;
    };


    // Declare the function template that handles primitive types
    // such as int, std::string, etc. in primitives.cpp
    template <typename T>
    __FLX_API TypeDescriptor* GetPrimitiveDescriptor();

    #pragma endregion

    #pragma region TypeResolver

    // A helper class to find TypeDescriptors with SFINAE.
    struct DefaultResolver
    {
      // Detects if the type T has a static member named "Reflection"
      // This is done by overloading a function with two different return types
      // and using SFINAE to select the right one.
      // Use as "IsReflected<T>::value"
      template <typename T> static char func(decltype(&T::Reflection));
      template <typename T> static int func(...);
      template <typename T>
      struct IsReflected
      {
        enum { value = (sizeof(func<T>(nullptr)) == sizeof(char)) };
      };

      // This version is called if T has a static member named "Reflection"
      template <typename T, typename std::enable_if<IsReflected<T>::value, int>::type = 0>
      static TypeDescriptor* Get()
      {
        return &T::Reflection;
      }

      // This version is called if T is a primitive type
      template <typename T, typename std::enable_if<!IsReflected<T>::value, int>::type = 0>
      static TypeDescriptor* Get()
      {
        return GetPrimitiveDescriptor<T>();
      }
    };

    // This is the primary class template for finding all TypeDescriptors.
    // Usage: FlexEngine::Reflection::TypeDescriptor* type_desc = FlexEngine::Reflection::TypeResolver<int>::Get();
    template <typename T>
    struct TypeResolver
    {
      static TypeDescriptor* Get()
      {
        return DefaultResolver::Get<T>();
      }
    };

    #pragma endregion



    // Specializations for C++ containers
    // 
    // These are used to handle containers and wrappers such as
    // std::vector and std::shared_ptr which are not primitive types,
    // but are still commonly used.
    // 
    // Writing a new TypeDescriptor for each container type is a bit tedious,
    // but it's necessary because each container type has different behavior.
    // ChatGPT can easily help write these specializations.
    // 
    // Here is a list of all the specializations in this file.
    // Referenced from https://en.cppreference.com/w/cpp/container
    // Use CTRL+F to find the one you're looking for.
    // 
    // pair is supported.
    // 
    // unique_ptr, shared_ptr, and weak_ptr are supported. (C++11)
    // auto_ptr is not supported. (deprecated in C++11, removed in C++17)
    // Smart void pointers are supported. (std::shared_ptr<void>)
    // 
    // span (C++20) and md_span (C++23) are not supported.
    // flat_set, flat_map, flat_multiset, and flat_multimap are not supported. (C++23)
    // 
    // A completed specialization is marked with [X]
    // Incomplete specializations are marked with [ ]
    // 
    //  [X] TypeDescriptor_Struct
    //  [ ] TypeDescriptor_StdArray (C++11)
    //  [X] TypeDescriptor_StdVector
    //  [ ] TypeDescriptor_StdDeque
    //  [ ] TypeDescriptor_StdForwardList (C++11)
    //  [ ] TypeDescriptor_StdList
    //  [ ] TypeDescriptor_StdSet
    //  [ ] TypeDescriptor_StdMap
    //  [ ] TypeDescriptor_StdMultiSet
    //  [ ] TypeDescriptor_StdMultiMap
    //  [ ] TypeDescriptor_StdUnorderedSet (C++11)
    //  [X] TypeDescriptor_StdUnorderedMap (C++11)
    //  [ ] TypeDescriptor_StdUnorderedMultiSet (C++11)
    //  [ ] TypeDescriptor_StdUnorderedMultiMap (C++11)
    //  [ ] TypeDescriptor_StdStack
    //  [ ] TypeDescriptor_StdQueue
    //  [ ] TypeDescriptor_StdPriorityQueue
    //  [ ] TypeDescriptor_StdUniquePtr (C++11)
    //  [X] TypeDescriptor_StdSharedPtr (C++11)
    //  [ ] TypeDescriptor_StdWeakPtr (C++11)
    //  [X] TypeDescriptor_StdPair
    // 

    #pragma region Specializations for C++ containers

    // Type descriptor for user-defined structs/classes.
    // Specialized for structs/classes.
    struct TypeDescriptor_Struct : TypeDescriptor
    {
      struct Member
      {
        const char* name;
        size_t offset;
        TypeDescriptor* type;
      };

      std::vector<Member> members;

      TypeDescriptor_Struct(void (*init)(TypeDescriptor_Struct*))
        : TypeDescriptor{ "", 0}
      {
        init(this);
      }

      TypeDescriptor_Struct(const char*, size_t, const std::initializer_list<Member>& init)
        : TypeDescriptor{ "", 0}, members{init}
      {
      }

      virtual void Dump(const void* obj, std::ostream& os, int indent_level) const override
      {
        os << name << "\n" << std::string(4 * indent_level, ' ') << "{\n";
        for (const Member& member : members)
        {
          os << std::string(4 * (indent_level + 1), ' ') << member.name << " = ";
          member.type->Dump((char*)obj + member.offset, os, indent_level + 1);
          os << "\n";
        }
        os << std::string(4 * indent_level, ' ') << "}\n";
      }

      virtual void Serialize(const void* obj, std::ostream& os) const override
      {
        os << R"({"type":")" << name << R"(","data":[)";
        bool first = true;
        for (const Member& member : members)
        {
          if (!first) os << ",";
          first = false;
          member.type->Serialize((char*)obj + member.offset, os);
        }
        os << "]}";
      }

      virtual void Deserialize(void* obj, const json& value) const override
      {
        const auto& arr = value["data"].GetArray();

        // guard against array size mismatch
        FLX_INTERNAL_ASSERT(arr.Size() == members.size(),
          "Array size mismatch while deserializing struct\n"
          "This is most likely caused by a corrupted .flx file"
        );

        // deserialize each member
        for (SizeType i = 0; i < members.size(); i++)
        {
          members[i].type->Deserialize((char*)obj + members[i].offset, arr[i]);
        }
      }

    };



    // TypeDescriptor for std::vector.
    // Specialized for std::vector.
    struct TypeDescriptor_StdVector : TypeDescriptor
    {
      TypeDescriptor* item_type;
      size_t (*get_size)(const void*);
      const void* (*get_item)(const void*, size_t);
      void* (*set_item)(void*, size_t);

      template <typename ItemType>
      TypeDescriptor_StdVector(ItemType*)
        : TypeDescriptor{ "std::vector<>", sizeof(std::vector<ItemType>) }
        , item_type{ TypeResolver<ItemType>::Get() }
      {
        get_size = [](const void* vec_ptr) -> size_t {
          const auto& vec = *(const std::vector<ItemType>*) vec_ptr;
          return vec.size();
        };
        get_item = [](const void* vec_ptr, size_t index) -> const void* {
          const auto& vec = *(const std::vector<ItemType>*) vec_ptr;
          return &vec[index];
        };
        set_item = [](void* vec_ptr, size_t index) -> void* {
          auto& vec = *(std::vector<ItemType>*) vec_ptr;
          if (index >= vec.size()) vec.resize(index + 1);
          return &vec[index];
        };
      }

      virtual std::string ToString() const override
      {
        return std::string("std::vector<") + item_type->ToString() + ">";
      }

      virtual void Dump(const void* obj, std::ostream& os, int indent_level) const override
      {
        size_t num_items = get_size(obj);
        os << "\n" << ToString();
        if (num_items == 0)
        {
          os << "{}\n";
        }
        else
        {
          os << "\n" << std::string(4 * indent_level, ' ') << "{\n";
          for (size_t index = 0; index < num_items; index++)
          {
            os << std::string(4 * (indent_level + 1), ' ') << "[" << index << "]\n"
              << std::string(4 * (indent_level + 1), ' ');
            item_type->Dump(get_item(obj, index), os, indent_level + 1);
            os << "\n";
          }
          os << std::string(4 * indent_level, ' ') << "}\n";
        }
      }

      virtual void Serialize(const void* obj, std::ostream& os) const override
      {
        size_t num_items = get_size(obj);
        if (num_items == 0)
        {
          os << R"({"type":")" << ToString() << R"(","data":[]})";
        }
        else
        {
          os << R"({"type":")" << ToString() << R"(","data":[)";
          for (size_t index = 0; index < num_items; index++)
          {
            item_type->Serialize(get_item(obj, index), os);
            if (index < num_items - 1) os << ",";
          }
          os << "]}";
        }
      }

      virtual void Deserialize(void* obj, const json& value) const override
      {
        const auto& arr = value["data"].GetArray();

        for (SizeType i = 0; i < arr.Size(); i++)
        {
          item_type->Deserialize(set_item(obj, i), arr[i]);
        }
      }

    };

    // Partially specialize TypeResolver for std::vectors.
    template <typename T>
    struct TypeResolver<std::vector<T>>
    {
      static TypeDescriptor* Get()
      {
        static TypeDescriptor_StdVector type_desc{ (T*) nullptr };
        if (TYPE_DESCRIPTOR_LOOKUP.count(type_desc.name) == 0)
        {
          TYPE_DESCRIPTOR_LOOKUP[type_desc.name] = &type_desc;
        }
        return &type_desc;
      }
    };



    // TypeDescriptor for std::unordered_map.
    // Specialized for std::unordered_map.
    // Important note that raw pointers are not supported
    // Use std::shared_ptr instead.
    template <typename KeyType, typename ValueType>
    struct TypeDescriptor_StdUnorderedMap : TypeDescriptor
    {
      TypeDescriptor* key_type;
      TypeDescriptor* value_type;

      TypeDescriptor_StdUnorderedMap(std::unordered_map<KeyType, ValueType>*)
        : TypeDescriptor{ "std::unordered_map<>", sizeof(std::unordered_map<KeyType, ValueType>) }
        , key_type{ TypeResolver<KeyType>::Get() }
        , value_type{ TypeResolver<ValueType>::Get() }
      {
      }

      virtual std::string ToString() const override
      {
        return std::string("std::unordered_map<") + key_type->ToString() + ", " + value_type->ToString() + ">";
      }

      virtual void Dump(const void* obj, std::ostream& os, int indent_level) const override
      {
        const auto& map = *(const std::unordered_map<KeyType, ValueType>*)obj;
        os << "\n"
          << std::string(4 * indent_level, ' ') << ToString() << "\n"
          << std::string(4 * indent_level, ' ') << "{\n";
        for (const auto& pair : map)
        {
          os << std::string(4 * (indent_level + 1), ' ');
          key_type->Dump(&pair.first, os, indent_level + 1);
          os << ": ";
          value_type->Dump(&pair.second, os, indent_level + 1);
          os << "\n";
        }
        os << std::string(4 * indent_level, ' ') << "}\n";
      }

      virtual void Serialize(const void* obj, std::ostream& os) const override
      {
        const auto& map = *(const std::unordered_map<KeyType, ValueType>*)obj;
        os << R"({"type":")" << ToString() << R"(","data":[)";
        bool first = true;
        for (const auto& pair : map)
        {
          if (!first) os << ",";
          first = false;
          os << "[";
          key_type->Serialize(&pair.first, os);
          os << ",";
          value_type->Serialize(&pair.second, os);
          os << "]";
        }
        os << "]}";
      }

      virtual void Deserialize(void* obj, const rapidjson::Value& value) const override
      {
        std::unordered_map<KeyType, ValueType>& map = *(std::unordered_map<KeyType, ValueType>*)obj;

        const auto& arr = value["data"].GetArray();

        for (SizeType i = 0; i < arr.Size(); i++)
        {
          KeyType key;
          ValueType val;
          key_type->Deserialize(&key, arr[i][0]);
          value_type->Deserialize(&val, arr[i][1]);
          map[key] = val;
        }
      }
    };

    // Partially specialize TypeResolver for std::unordered_maps.
    template <typename KeyType, typename ValueType>
    struct TypeResolver<std::unordered_map<KeyType, ValueType>>
    {
      static TypeDescriptor* Get()
      {
        static TypeDescriptor_StdUnorderedMap<KeyType, ValueType> type_desc{ (std::unordered_map<KeyType, ValueType>*)nullptr };
        if (TYPE_DESCRIPTOR_LOOKUP.count(type_desc.name) == 0)
        {
          TYPE_DESCRIPTOR_LOOKUP[type_desc.name] = &type_desc;
        }
        return &type_desc;
      }
    };



    // TypeDescriptor for std::shared_ptr.
    // Specialized for std::shared_ptr.
    template <typename T>
    struct TypeDescriptor_StdSharedPtr : TypeDescriptor
    {
      TypeDescriptor* item_type;

      TypeDescriptor_StdSharedPtr(T*)
        : TypeDescriptor{ "std::shared_ptr<>", sizeof(std::shared_ptr<T>) }
        , item_type{ TypeResolver<T>::Get() }
      {
      }

      virtual std::string ToString() const override
      {
        return "std::shared_ptr<" + item_type->ToString() + ">";
      }

      virtual void Dump(const void* obj, std::ostream& os, int indent_level) const override
      {
        const auto& shared_ptr = *reinterpret_cast<const std::shared_ptr<T>*>(obj);
        if (shared_ptr)
        {
          os << "\n" << ToString() << "\n"
            << std::string(4 * indent_level, ' ') << "{\n"
            << std::string(4 * (indent_level + 1), ' ');
          item_type->Dump(shared_ptr.get(), os, indent_level + 1);
          os << "\n" << std::string(4 * indent_level, ' ') << "}\n";
        }
        else
        {
          os << "null";
        }
      }

      virtual void Serialize(const void* obj, std::ostream& os) const override
      {
        const auto& shared_ptr = *reinterpret_cast<const std::shared_ptr<T>*>(obj);
        if (shared_ptr)
        {
          item_type->Serialize(shared_ptr.get(), os);
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
          // Set the shared_ptr to null
          *reinterpret_cast<std::shared_ptr<T>*>(obj) = nullptr;
        }
        else
        {
          // Deserialize the object and store it in the shared_ptr
          std::shared_ptr<T> shared_ptr = std::make_shared<T>();
          item_type->Deserialize(shared_ptr.get(), value);
          *reinterpret_cast<std::shared_ptr<T>*>(obj) = shared_ptr;
        }
      }

    };

    // Specialization for std::shared_ptr<void>.
    // This is used for type erasure.
    template <>
    struct TypeDescriptor_StdSharedPtr<void> : TypeDescriptor
    {
      TypeDescriptor* item_type;

      TypeDescriptor_StdSharedPtr(void*)
        : TypeDescriptor{ "std::shared_ptr<>", sizeof(std::shared_ptr<void>) }
        , item_type{ nullptr }
      {
      }

      virtual std::string ToString() const override
      {
        return "std::shared_ptr<void>";
      }

      virtual void Dump(const void* obj, std::ostream& os, int indent_level) const override
      {
        const auto& shared_ptr = *reinterpret_cast<const std::shared_ptr<void>*>(obj);
        if (shared_ptr)
        {
          os
            << "\n" << ToString() << "\n"
            << std::string(4 * indent_level, ' ') << "{\n"
            << std::string(4 * (indent_level + 1), ' ') << shared_ptr.get() << "\n"
            << std::string(4 * indent_level, ' ') << "}\n"
          ;
        }
        else
        {
          os << "null";
        }
      }

      virtual void Serialize(const void* obj, std::ostream& os) const override
      {
        const auto& shared_ptr = *reinterpret_cast<const std::shared_ptr<void>*>(obj);
        if (shared_ptr)
        {
          // Obtain the raw pointer and serialize it
          // 
          // How this works:
          // Since the shared_ptr is a void pointer, we can't get the size directly.
          // The serialized_str needs to be constructed from the raw pointer which needs the full size.
          // Thus, the shared_ptr stores the size of the data in the first sizeof(std::size_t) == 4 or 8 bytes.
          // This allows us to get the size of the data, add sizeof(std::size_t), which gives us the full size.
          // Implemented in FlexECS::Internal_GetComponentData.

          void* ptr = shared_ptr.get();
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
          os << R"({"type":")" << "std::shared_ptr<void>" << R"(","data":")" << serialized_data << R"("})";
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
          // Set the shared_ptr to null
          *reinterpret_cast<std::shared_ptr<void>*>(obj) = nullptr;
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
          
          // Convert it to a std::shared_ptr<void>
          std::shared_ptr<void> shared_ptr = std::shared_ptr<void>(
            ptr,
            [](void* ptr)
            {
              delete[] reinterpret_cast<char*>(ptr);
            }
          );

          *reinterpret_cast<std::shared_ptr<void>*>(obj) = shared_ptr;
        }
      }

    };

    /// Partially specialize TypeResolver for std::shared_ptrs.
    template <typename T>
    struct TypeResolver<std::shared_ptr<T>>
    {
      static TypeDescriptor* Get()
      {
        static TypeDescriptor_StdSharedPtr type_desc{ (T*) nullptr };
        if (TYPE_DESCRIPTOR_LOOKUP.count(type_desc.name) == 0)
        {
          TYPE_DESCRIPTOR_LOOKUP[type_desc.name] = &type_desc;
        }
        return &type_desc;
      }
    };



    // TypeDescriptor for std::pair.
    // Specialized for std::pair.
    template <typename FirstType, typename SecondType>
    struct TypeDescriptor_StdPair : TypeDescriptor
    {
      TypeDescriptor* first_type;
      TypeDescriptor* second_type;

      TypeDescriptor_StdPair(std::pair<FirstType, SecondType>*)
        : TypeDescriptor{ "std::pair<>", sizeof(std::pair<FirstType, SecondType>) }
        , first_type{ TypeResolver<FirstType>::Get() }
        , second_type{ TypeResolver<SecondType>::Get() }
      {
      }

      virtual std::string ToString() const override
      {
        return std::string("std::pair<") + first_type->ToString() + ", " + second_type->ToString() + ">";
      }

      virtual void Dump(const void* obj, std::ostream& os, int indent_level) const override
      {
        const auto& pair = *(const std::pair<FirstType, SecondType>*)obj;
        os << "\n"
          << std::string(4 * indent_level, ' ') << ToString() << "\n"
          << std::string(4 * indent_level, ' ') << "{\n"
          << std::string(4 * (indent_level + 1), ' ');
        first_type->Dump(&pair.first, os, indent_level + 1);
        os << ",\n"
          << std::string(4 * (indent_level + 1), ' ');
        second_type->Dump(&pair.second, os, indent_level + 1);
        os << "\n"
          << std::string(4 * indent_level, ' ') << "}\n";
      }

      virtual void Serialize(const void* obj, std::ostream& os) const override
      {
        const auto& pair = *(const std::pair<FirstType, SecondType>*)obj;
        os << R"({"type":")" << ToString() << R"(","data":[)";
        first_type->Serialize(&pair.first, os);
        os << ",";
        second_type->Serialize(&pair.second, os);
        os << "]}";
      }

      virtual void Deserialize(void* obj, const json& value) const override
      {
        const auto& arr = value["data"].GetArray();
        first_type->Deserialize(&((std::pair<FirstType, SecondType>*)obj)->first, arr[0]);
        second_type->Deserialize(&((std::pair<FirstType, SecondType>*)obj)->second, arr[1]);
      }

    };

    // Partially specialize TypeResolver for std::pairs.
    template <typename FirstType, typename SecondType>
    struct TypeResolver<std::pair<FirstType, SecondType>>
    {
      static TypeDescriptor* Get()
      {
        static TypeDescriptor_StdPair<FirstType, SecondType> type_desc{ (std::pair<FirstType, SecondType>*)nullptr };
        if (TYPE_DESCRIPTOR_LOOKUP.count(type_desc.name) == 0)
        {
          TYPE_DESCRIPTOR_LOOKUP[type_desc.name] = &type_desc;
        }
        return &type_desc;
      }
    };

    #pragma endregion

  }

}