// Copyright 2024 solar-mist


#include "vipir/IR/Instruction/BinaryInst.h"

#include "vipir/IR/BasicBlock.h"
#include "vipir/IR/Function.h"

#include "vipir/MC/CmpOperand.h"

#include "vipir/Module.h"

#include "vipir/LIR/Instruction/Arithmetic.h"
#include "vipir/LIR/Instruction/Compare.h"
#include "vipir/LIR/Instruction/Move.h"

#include "vasm/instruction/twoOperandInstruction/LogicalInstruction.h"
#include "vasm/instruction/twoOperandInstruction/MovInstruction.h"
#include "vasm/instruction/variableOperandInstruction/IMulInstruction.h"

#include "vasm/instruction/operand/Immediate.h"

#include <cassert>
#include <format>

namespace vipir
{
    void BinaryInst::print(std::ostream& stream)
    {
        std::string operatorName;
        switch (mOperator)
        {
            case Instruction::ADD:
                operatorName = "add";
                break;
            case Instruction::SUB:
                operatorName = "sub";
                break;

            case Instruction::SMUL:
                operatorName = "smul";
                break;
            case Instruction::UMUL:
                operatorName = "umul";
                break;

            case Instruction::SDIV:
                operatorName = "sdiv";
                break;
            case Instruction::UDIV:
                operatorName = "udiv";
                break;

            case Instruction::ULSHIFT:
                operatorName = "ulshift";
                break;
            case Instruction::URSHIFT:
                operatorName = "urshift";
                break;
            case Instruction::SRSHIFT:
                operatorName = "srshift";
                break;

            case Instruction::BWOR:
                operatorName = "bwor";
                break;
            case Instruction::BWAND:
                operatorName = "bwand";
                break;
            case Instruction::BWXOR:
                operatorName = "bwxor";
                break;
            
            case Instruction::EQ:
                operatorName = "cmp eq";
                break;
            case Instruction::NE:
                operatorName = "cmp ne";
                break;
            
            case Instruction::LT:
                operatorName = "cmp lt";
                break;
            case Instruction::GT:
                operatorName = "cmp gt";
                break;
            
            case Instruction::LE:
                operatorName = "cmp le";
                break;
            case Instruction::GE:
                operatorName = "cmp ge";
                break;
        }
        stream << std::format("{} %{}, {}, {}", operatorName, getName(mValueId), mLeft->ident(), mRight->ident());
    }

    std::string BinaryInst::ident() const
    {
        return std::format("%{}", getName(mValueId));
    }

    std::vector<Value*> BinaryInst::getOperands()
    {
        return {mLeft, mRight};
    }

    std::vector<int> BinaryInst::getRegisterSmashes()
    {
        if (mOperator == Instruction::UMUL || mOperator == Instruction::SDIV || mOperator == Instruction::UDIV)
        {
            return {0, 2}; // rax, rdx
        }
        else if (mOperator == Instruction::ULSHIFT || mOperator == Instruction::URSHIFT || mOperator == Instruction::SRSHIFT)
        {
            return {1}; // rcx
        }
        return {};
    }

    void BinaryInst::emit(lir::Builder& builder)
    {
        auto createArithmetic = [&builder, this](lir::BinaryArithmetic::Operator op){
            mLeft->lateEmit(builder);
            mRight->lateEmit(builder);
            lir::OperandPtr vreg = std::make_unique<lir::VirtualReg>(mVReg, mType->getOperandSize());
            if (*mRight->getEmittedValue() == vreg)
            {
                builder.addValue(std::make_unique<lir::BinaryArithmetic>(mRight->getEmittedValue(), op, mLeft->getEmittedValue()));
            }
            else
            {
                builder.addValue(std::make_unique<lir::Move>(vreg->clone(), mLeft->getEmittedValue()));
                builder.addValue(std::make_unique<lir::BinaryArithmetic>(vreg->clone(), op, mRight->getEmittedValue()));
            }
            mEmittedValue = std::move(vreg);
        };

        auto createSingleOpArithmetic = [&builder, this](lir::BinaryArithmetic::Operator op){
            mLeft->lateEmit(builder);
            mRight->lateEmit(builder);
            lir::OperandPtr sourceReg = std::make_unique<lir::PhysicalReg>(0, mType->getOperandSize());
            builder.addValue(std::make_unique<lir::Move>(sourceReg->clone(), mLeft->getEmittedValue()));

            if (op == lir::BinaryArithmetic::Operator::IDiv || op == lir::BinaryArithmetic::Operator::Div)
            {
                lir::OperandPtr rdx = std::make_unique<lir::PhysicalReg>(2, mType->getOperandSize());
                lir::OperandPtr zero = std::make_unique<lir::Immediate>(0);
                builder.addValue(std::make_unique<lir::Move>(std::move(rdx), std::move(zero)));
            }

            lir::OperandPtr vreg = std::make_unique<lir::VirtualReg>(mVReg, mType->getOperandSize());
            lir::OperandPtr right = mRight->getEmittedValue();
            if (dynamic_cast<lir::Immediate*>(right.get()))
            {
                builder.addValue(std::make_unique<lir::Move>(vreg->clone(), std::move(right)));
                right = vreg->clone();
            }
            builder.addValue(std::make_unique<lir::BinaryArithmetic>(sourceReg->clone(), op, std::move(right)));
            builder.addValue(std::make_unique<lir::Move>(vreg->clone(), std::move(sourceReg)));
            mEmittedValue = std::move(vreg);
        };
        
        switch (mOperator)
        {
            case Instruction::ADD:
                createArithmetic(lir::BinaryArithmetic::Operator::Add);
                break;
            case Instruction::SUB:
                createArithmetic(lir::BinaryArithmetic::Operator::Sub);
                break;

            case Instruction::SMUL:
                createArithmetic(lir::BinaryArithmetic::Operator::IMul);
                break;
            case Instruction::UMUL:
                createSingleOpArithmetic(lir::BinaryArithmetic::Operator::Mul);
                break;

            case Instruction::SDIV:
                createSingleOpArithmetic(lir::BinaryArithmetic::Operator::IDiv);
                break;
            case Instruction::UDIV:
                createSingleOpArithmetic(lir::BinaryArithmetic::Operator::Div);
                break;

            case Instruction::ULSHIFT:
                createSingleOpArithmetic(lir::BinaryArithmetic::Operator::ULShift);
                break;
            case Instruction::URSHIFT:
                createSingleOpArithmetic(lir::BinaryArithmetic::Operator::URShift);
                break;
            case Instruction::SRSHIFT:
                createSingleOpArithmetic(lir::BinaryArithmetic::Operator::SRShift);
                break;

            case Instruction::BWAND:
                createArithmetic(lir::BinaryArithmetic::Operator::BWAnd);
                break;
            case Instruction::BWOR:
                createArithmetic(lir::BinaryArithmetic::Operator::BWOr);
                break;
            case Instruction::BWXOR:
                createArithmetic(lir::BinaryArithmetic::Operator::BWXor);
                break;

            default:
                break;
        }
    }

    void BinaryInst::lateEmit(lir::Builder& builder)
    {
        auto createCompare = [&builder, this](lir::CMP::Operator op){
            mLeft->lateEmit(builder);
            mRight->lateEmit(builder);
            builder.addValue(std::make_unique<lir::Compare>(mLeft->getEmittedValue(), op, mRight->getEmittedValue()));
            mEmittedValue = std::make_unique<lir::CMP>(op);
        };

        switch (mOperator)
        {
            case Instruction::EQ:
                createCompare(lir::CMP::Operator::EQ);
                break;
            case Instruction::NE:
                createCompare(lir::CMP::Operator::NE);
                break;
            case Instruction::LT:
                createCompare(lir::CMP::Operator::LT);
                break;
            case Instruction::GT:
                createCompare(lir::CMP::Operator::GT);
                break;
            case Instruction::LE:
                createCompare(lir::CMP::Operator::LE);
                break;
            case Instruction::GE:
                createCompare(lir::CMP::Operator::GE);
                break;

            default:
                break;
        }
    }

    BinaryInst::BinaryInst(BasicBlock* parent, Value* left, Instruction::BinaryOperators op, Value* right)
        : Instruction(parent->getParent()->getModule(), parent)
        , mLeft(left)
        , mOperator(op)
        , mRight(right)
        , mValueId(mModule.getNextValueId())
    {
        assert(left->getType() == right->getType());
        
        switch (mOperator)
        {
            case Instruction::ADD:
            case Instruction::SUB:
            case Instruction::SMUL:
            case Instruction::UMUL:
            case Instruction::SDIV:
            case Instruction::UDIV:
            case Instruction::ULSHIFT:
            case Instruction::URSHIFT:
            case Instruction::SRSHIFT:
            case Instruction::BWOR:
            case Instruction::BWAND:
            case Instruction::BWXOR:
            {
                mType = left->getType();
                break;
            }

            case Instruction::EQ:
            case Instruction::NE:
            case Instruction::LT:
            case Instruction::GT:
            case Instruction::LE:
            case Instruction::GE:
            {
                mType = Type::GetBooleanType();
                mRequiresVReg = false;
                break;
            }
        }
    }
}