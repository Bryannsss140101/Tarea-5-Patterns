#include <iostream>
#include <memory>
#include <string>
#include <cmath>

class Expression {
public:
    virtual ~Expression() = default;
    virtual double evaluate(double x) const = 0;
    virtual std::shared_ptr<Expression> derivative() const = 0;
    virtual std::string toString() const = 0;
};

class Sin;
class Cos;

class Constant : public Expression {
private:
    double val;
public:
    explicit Constant(double v) : val(v) {}
    
    double evaluate(double) const override { 
        return val; 
    }
    
    std::shared_ptr<Expression> derivative() const override {
        return std::make_shared<Constant>(0);
    }
    
    std::string toString() const override {
        if (val == static_cast<int>(val)) {
            return std::to_string(static_cast<int>(val));
        }
        return std::to_string(val);
    }
};

class Variable : public Expression {
public:
    double evaluate(double x) const override { 
        return x; 
    }
    
    std::shared_ptr<Expression> derivative() const override {
        return std::make_shared<Constant>(1);
    }
    
    std::string toString() const override { 
        return "x"; 
    }
};

class Add : public Expression {
private:
    std::shared_ptr<Expression> left, right;
public:
    Add(std::shared_ptr<Expression> l, std::shared_ptr<Expression> r)
        : left(std::move(l)), right(std::move(r)) {}

    double evaluate(double x) const override {
        return left->evaluate(x) + right->evaluate(x);
    }
    
    std::shared_ptr<Expression> derivative() const override {
        return std::make_shared<Add>(left->derivative(), right->derivative());
    }
    
    std::string toString() const override {
        return "(" + left->toString() + " + " + right->toString() + ")";
    }
};

class Multiply : public Expression {
private:
    std::shared_ptr<Expression> left, right;
public:
    Multiply(std::shared_ptr<Expression> l, std::shared_ptr<Expression> r)
        : left(std::move(l)), right(std::move(r)) {}

    double evaluate(double x) const override {
        return left->evaluate(x) * right->evaluate(x);
    }
    
    std::shared_ptr<Expression> derivative() const override {
        auto u_prime_v = std::make_shared<Multiply>(left->derivative(), right);
        auto u_v_prime = std::make_shared<Multiply>(left, right->derivative());
        return std::make_shared<Add>(u_prime_v, u_v_prime);
    }
    
    std::string toString() const override {
        return "(" + left->toString() + " * " + right->toString() + ")";
    }
};

class Power : public Expression {
private:
    std::shared_ptr<Expression> base;
    double exp;
public:
    Power(std::shared_ptr<Expression> b, double e) 
        : base(std::move(b)), exp(e) {}

    double evaluate(double x) const override {
        return std::pow(base->evaluate(x), exp);
    }
    
    std::shared_ptr<Expression> derivative() const override {
        auto factor1 = std::make_shared<Constant>(exp);
        auto factor2 = std::make_shared<Power>(base, exp - 1);
        auto term1 = std::make_shared<Multiply>(factor1, factor2);
        return std::make_shared<Multiply>(term1, base->derivative());
    }
    
    std::string toString() const override {
        std::string expStr = (exp == static_cast<int>(exp)) ? 
            std::to_string(static_cast<int>(exp)) : std::to_string(exp);
        return base->toString() + "^" + expStr;
    }
};

class Sin : public Expression {
private:
    std::shared_ptr<Expression> arg;
public:
    explicit Sin(std::shared_ptr<Expression> a) : arg(std::move(a)) {}

    double evaluate(double x) const override {
        return std::sin(arg->evaluate(x));
    }
    
    std::shared_ptr<Expression> derivative() const;
    
    std::string toString() const override {
        return "sin(" + arg->toString() + ")";
    }
};

class Cos : public Expression {
private:
    std::shared_ptr<Expression> arg;
public:
    explicit Cos(std::shared_ptr<Expression> a) : arg(std::move(a)) {}

    double evaluate(double x) const override {
        return std::cos(arg->evaluate(x));
    }
    
    std::shared_ptr<Expression> derivative() const override {
        auto neg_sin = std::make_shared<Multiply>(
            std::make_shared<Constant>(-1),
            std::make_shared<Sin>(arg)
        );
        return std::make_shared<Multiply>(neg_sin, arg->derivative());
    }
    
    std::string toString() const override {
        return "cos(" + arg->toString() + ")";
    }
};

inline std::shared_ptr<Expression> Sin::derivative() const {
    return std::make_shared<Multiply>(
        std::make_shared<Cos>(arg),
        arg->derivative()
    );
}

int main() {
    auto x = std::make_shared<Variable>();

    auto expr = std::make_shared<Multiply>(
        std::make_shared<Add>(
            std::make_shared<Power>(x, 2),
            std::make_shared<Multiply>(
                std::make_shared<Constant>(3),
                x
            )
        ),
        std::make_shared<Sin>(x)
    );

    auto deriv = expr->derivative();

    std::cout << "f(x) = " << expr->toString() << std::endl;
    std::cout << "f'(x) = " << deriv->toString() << std::endl;
    std::cout << "f(2) = " << expr->evaluate(2) << std::endl;

    return 0;
}