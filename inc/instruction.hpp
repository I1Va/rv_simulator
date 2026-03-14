#pragma once

#include <memory>
#include "instruction_impl.hpp"

namespace rv
{

class Instruction {
    struct I_Instruction {
        virtual ~I_Instruction() = default;
        virtual std::unique_ptr<I_Instruction> copy_() const = 0;
        virtual void execute_(ICPU &, IMEM &) const = 0;
        virtual std::string name_() const = 0;
        virtual std::string operands_() const = 0;
    };

    template <typename T>
    struct InstructionObject final : I_Instruction {
        T data_;
        InstructionObject(T x) : data_(std::move(x)) {}
        std::unique_ptr<I_Instruction> copy_() const override {
            return std::make_unique<InstructionObject>(*this);
        }

        void execute_(ICPU &cpu, IMEM &mem) const override {
            rv::execute(data_, cpu, mem);
        }

        std::string name_() const override {
            return rv::name(data_);
        }

        std::string operands_() const override {
            return rv::operands(data_);
        }
    };

    std::unique_ptr<I_Instruction> self_;

public:
    template <typename T>
    Instruction(T &x) : self_(std::make_unique<InstructionObject<T>>(x)) {}

    template <typename T>
    Instruction(T &&x) : self_(std::make_unique<InstructionObject<T>>(std::move(x))) {}

    template <typename T>
    Instruction operator=(T x) { 
        Instruction tmp{std::move(x)};
        std::swap(this->self_, tmp.self_); 
        return *this;
    }
    
    template <typename T>
    Instruction operator=(T &&x) { 
        self_ = std::make_unique<InstructionObject<T>>(std::move(x)); 
        return *this;
    }

public:
    friend void execute(const Instruction &x, ICPU &cpu, IMEM &mem) {
        x.self_->execute_(cpu, mem);
    }

    friend std::string name(const Instruction &x) {
        return x.self_->name_();
    }

    friend std::string operands(const Instruction &x) {
        return x.self_->operands_();
    }

    friend std::string to_string(const Instruction &x) {
        return x.self_->name_() + " " + x.self_->operands_();
    }
};

} // namspace rv
