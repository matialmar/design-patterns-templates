#include <iostream>
#include <forward_list>
#include <unordered_map>
#include <memory>

class Observer{
    friend class Subject;
    public:
        Observer(){};
        virtual ~Observer(){};

    private:
        virtual void update() = 0;
};

class Subject{
    typedef std::shared_ptr<Observer> ObserverPtr; //Beware one Observer can be in multiple Types 

    public:
        enum MessageType{
            TYPE1 = 0,
            TYPE2,
            TYPE3,
            ALL,
            LAST //Place-holder
        };

        Subject(){
            for(int type = MessageType::TYPE1; type < MessageType::LAST; type++ ){
                _observers.emplace(static_cast<MessageType>(type),std::forward_list<ObserverPtr>());
            }
        };

        void subscribe(MessageType msg_type, ObserverPtr obs){
            _observers[msg_type].push_front(obs);
            _observers[MessageType::ALL].push_front(obs);
        };

        void unsubscribe(MessageType msg_type, ObserverPtr obs){
            _observers[msg_type].remove(obs);
        };

        void notify(MessageType msg_type){
            for(auto obs : _observers[msg_type]){
                obs->update();
            }
        };

        void notifyAll(){
            for(auto obs : _observers[MessageType::ALL]){
                obs->update();
            }
        };

        private:
            std::unordered_map<MessageType, std::forward_list<ObserverPtr>> _observers;

};

class ConcreteObserver : public Observer{
    public:
        ConcreteObserver(std::string name) : _name(name){};
        ~ConcreteObserver(){};
    private:
        std::string _name;

        void update() override {
            std::cout << "Observer " << _name << std::endl;
        };
};

int main(){
    Subject sub;

    std::shared_ptr<ConcreteObserver> obs1(new ConcreteObserver("Foo"));
    std::shared_ptr<ConcreteObserver> obs2(new ConcreteObserver("Bar"));
    std::shared_ptr<ConcreteObserver> obs3(new ConcreteObserver("Buz"));

    sub.subscribe(Subject::MessageType::TYPE1, obs1);
    sub.subscribe(Subject::MessageType::TYPE1, obs2);
    sub.subscribe(Subject::MessageType::TYPE2, obs3);

    std::cout << "Subject::notify(TYPE1):" << std::endl;
    sub.notify(Subject::MessageType::TYPE1);
    std::cout << "Subject::notify(TYPE2):" << std::endl;
    sub.notify(Subject::MessageType::TYPE2);
    std::cout << "Subject::notify(TYPE3):" << std::endl;
    sub.notify(Subject::MessageType::TYPE3);
    std::cout << "Subject::notifyAll():" << std::endl;
    sub.notifyAll();
}