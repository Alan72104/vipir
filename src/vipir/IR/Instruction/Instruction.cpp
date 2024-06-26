// Copyright 2024 solar-mist

#include "vipir/IR/Instruction/Instruction.h"

#include "vipir/IR/BasicBlock.h"

namespace vipir
{
    void Instruction::eraseFromParent()
    {
        mParent->eraseValue(this);
    }
}