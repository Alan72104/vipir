// Copyright 2023 solar-mist

/*
 * An instruction that allocates a local variable
 * inside a function
*/

#ifndef VIPIR_IR_INSTRUCTION_ALLOCA_INST_H
#define VIPIR_IR_INSTRUCTION_ALLOCA_INST_H 1

#include "vipir/IR/Instruction/Instruction.h"

namespace vipir
{
    class Builder;

    class AllocaInst : public Instruction
    {
    friend class Builder;
    public:
        Type* getAllocatedType() const;

        void print(std::ostream& stream) const override;
        std::string ident() const override;

    protected:
        instruction::OperandPtr emit(std::vector<instruction::ValuePtr>& values) const override;

    private:
        AllocaInst(BasicBlock* parent, Type* allocatedType, std::string name);

        std::string mName;
        Type* mAllocatedType;
    };
}

#endif // VIPIR_IR_INSTRUCTION_ALLOCA_INST_H