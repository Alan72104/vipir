// Copyright 2024 solar-mist

#include "vipir/Type/VoidType.h"

namespace vipir
{
    VoidType::VoidType()
        : Type(0, "void")
    {
    }

    bool VoidType::isVoidType() const
    {
        return true;
    }
}