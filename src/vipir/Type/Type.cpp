// Copyright 2024 solar-mist

#include "vipir/Type/Type.h"
#include "vipir/Type/IntegerType.h"
#include "vipir/Type/VoidType.h"
#include "vipir/Type/BooleanType.h"
#include "vipir/Type/FunctionType.h"
#include "vipir/Type/PointerType.h"
#include "vipir/Type/StructType.h"
#include "vipir/Type/ArrayType.h"

#include <cassert>
#include <memory>
#include <vector>

namespace vipir
{
    std::vector<std::unique_ptr<vipir::Type>> types;

    Type* Type::GetIntegerType(int bits)
    {
        assert(bits % 8 == 0);
        for (const auto& type : types)
        {
            if (IntegerType* integerType = dynamic_cast<IntegerType*>(type.get()))
            {
                if (integerType->getSizeInBits() == bits)
                {
                    return integerType;
                }
            }
        }

        types.push_back(std::make_unique<IntegerType>(bits));
        return types.back().get();
    }

    Type* Type::GetVoidType()
    {
        for (const auto& type : types)
        {
            if (VoidType* voidType = dynamic_cast<VoidType*>(type.get()))
            {
                return voidType;
            }
        }

        types.push_back(std::make_unique<VoidType>());
        return types.back().get();
    }

    Type* Type::GetBooleanType()
    {
        for (const auto& type : types)
        {
            if (BooleanType* booleanType = dynamic_cast<BooleanType*>(type.get()))
            {
                return booleanType;
            }
        }

        types.push_back(std::make_unique<BooleanType>());
        return types.back().get();
    }

    Type* Type::GetFunctionType(Type* returnType, std::vector<Type*> argumentTypes)
    {
        for (const auto& type : types)
        {
            if (FunctionType* functionType = dynamic_cast<FunctionType*>(type.get()))
            {
                if (functionType->getReturnType() == returnType)
                {
                    if (argumentTypes == functionType->getArgumentTypes())
                    {
                        return functionType;
                    }
                }
            }
        }

        types.push_back(std::make_unique<FunctionType>(returnType, std::move(argumentTypes)));
        return types.back().get();
    }

    Type* Type::GetPointerType(Type* baseType)
    {
        for (const auto& type : types)
        {
            if (PointerType* pointerType = dynamic_cast<PointerType*>(type.get()))
            {
                if (pointerType->getBaseType() == baseType)
                {
                    return pointerType;
                }
            }
        }

        types.push_back(std::make_unique<PointerType>(baseType));
        return types.back().get();
    }

    Type* Type::GetStructType(std::vector<Type*> fieldTypes)
    {
        for (const auto& type : types)
        {
            if (StructType* structType = dynamic_cast<StructType*>(type.get()))
            {
                if (structType->getFields() == fieldTypes)
                {
                    return structType;
                }
            }
        }

        types.push_back(std::make_unique<StructType>(std::move(fieldTypes)));
        return types.back().get();
    }

    Type* Type::GetArrayType(Type* baseType, int count)
    {
        for (const auto& type : types)
        {
            if (ArrayType* arrayType = dynamic_cast<ArrayType*>(type.get()))
            {
                if (arrayType->getBaseType() == baseType && arrayType->getCount() == count)
                {
                    return arrayType;
                }
            }
        }

        types.push_back(std::make_unique<ArrayType>(baseType, count));
        return types.back().get();
    }
}