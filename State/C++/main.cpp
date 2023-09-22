#include <iostream>
class State;
class Context;

//State interface
class State{
    protected:
        Context* ctx;
    public:
        State(){};
        virtual void ejecutar() = 0;
        void setContext(Context * _ctx){ctx = _ctx;}; 
};

//Context
class Context{
    friend class State;

    private:
        State* state;

    public: 
        Context(State * _initial){ setState(_initial); };
        void setState(State* _state) {  state = _state; 
                                        state->setContext(this);};

        void run(){state->ejecutar();};
};

//Concrete states
class State2 : public State{
    public:
        State2(){};
        void ejecutar() override {
            std::cout << "State 2" << std::endl;
        };
};

class State1 : public State{
    public:
        State1(){};
        void ejecutar() override {
            std::cout << "State 1" << std::endl; 
            ctx->setState(new State2());
        };
};

int main(){
    State1* initial = new State1();
    Context ctx(initial);

    int states = 2;

    do{
        ctx.run();
    }while(--states > 0);

    return 0;
}