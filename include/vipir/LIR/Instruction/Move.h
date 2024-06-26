// Copyright 2024 solar-mist

#ifndef VIPIR_LIR_INSTRUCTION_MOVE_H
#define VIPIR_LIR_INSTRUCTION_MOVE_H 1

#include "vipir/LIR/Value.h"

namespace vipir
{
    namespace lir
    {
        class Move : public Value
        {
        friend class opt::Peephole;
        public:
            Move(OperandPtr left, OperandPtr right);

            void print(std::ostream& stream) const override;
            void emit(MC::Builder& builder) override;

            std::vector<std::reference_wrapper<OperandPtr> > getOutputOperands() override;
            std::vector<std::reference_wrapper<OperandPtr> > getInputOperands() override;

        private:
            OperandPtr mLeft;
            OperandPtr mRight;
        };

        class MoveSX : public Value
        {
        friend class opt::Peephole;
        public:
            MoveSX(OperandPtr left, OperandPtr right);

            void print(std::ostream& stream) const override;
            void emit(MC::Builder& builder) override;

            std::vector<std::reference_wrapper<OperandPtr> > getOutputOperands() override;
            std::vector<std::reference_wrapper<OperandPtr> > getInputOperands() override;

        private:
            OperandPtr mLeft;
            OperandPtr mRight;
        };

        class MoveZX : public Value
        {
        friend class opt::Peephole;
        public:
            MoveZX(OperandPtr left, OperandPtr right);

            void print(std::ostream& stream) const override;
            void emit(MC::Builder& builder) override;

            std::vector<std::reference_wrapper<OperandPtr> > getOutputOperands() override;
            std::vector<std::reference_wrapper<OperandPtr> > getInputOperands() override;

        private:
            OperandPtr mLeft;
            OperandPtr mRight;
        };


        class Push : public Value
        {
        friend class opt::Peephole;
        public:
            Push(OperandPtr operand);

            void print(std::ostream& stream) const override;
            void emit(MC::Builder& builder) override;

            std::vector<std::reference_wrapper<OperandPtr> > getInputOperands() override;

        private:
            OperandPtr mOperand;
        };
    }
}

#endif // VIPIR_LIR_INSTRUCTION_MOVE_H