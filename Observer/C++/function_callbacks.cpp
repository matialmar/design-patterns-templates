#include <iostream>
#include <forward_list>
#include <unordered_map>
#include <memory>
#include <algorithm>
#include <functional>

//From https://stackoverflow.com/questions/51848507/good-way-to-create-observer-design-pattern-in-c

class Observer{
    friend class Subject;
    public:
        virtual ~Observer(){};
        virtual void update() = 0;
};

class Subject{
    public:
        enum Id : uint64_t {};

        enum MessageType{
            TYPE1 = 0,
            TYPE2,
            TYPE3,
            LAST //Place-holder
        };

        Subject(){
            for(int type = MessageType::TYPE1; type < MessageType::LAST; type++ ){
                _callbacks.emplace(static_cast<MessageType>(type),std::unordered_map<Id, std::function<void()>>());
            }
        };

        static auto id_value()
            -> uint64_t&
        {
            static uint64_t the_id;
            return the_id;
        }

        Id subscribe(const MessageType msg_type, std::function<void()> callback){
            const auto id = Id(++id_value());
            _callbacks[msg_type].emplace(id, std::move(callback));
            return id;
        };

        bool unsubscribe(const Id id) {
            bool unsubscribed = false;

            for (auto& pair : _callbacks) {
                auto& callbackMap = pair.second;
                const auto it = callbackMap.find(id);

                if (it != callbackMap.end()) {
                    callbackMap.erase(it);
                    unsubscribed = true;
                }
            }

            return unsubscribed;
        }

        bool unsubscribe(const MessageType msg_type, const Id id){
            auto _callbackMap = _callbacks[msg_type];
            const auto it = _callbackMap.find( id );

            if( it == _callbackMap.end() )
            {
                return false;
            }
            _callbackMap.erase( it );
            return true;
        };

        void notify(const MessageType msg_type) const {
            auto _callbackMap = _callbacks.at(msg_type);

            for( const auto& pair : _callbackMap)
            {
                pair.second();
            }
        };
        
        private:
            struct MessageTypetHash
            {
                std::size_t operator()(const MessageType &event) const
                {
                    return static_cast<unsigned int>(event);
                }
            };

            std::unordered_map<MessageType, std::unordered_map<Id, std::function<void()>>, MessageTypetHash> _callbacks;

};

class ConcreteObserver : public Observer{
    public:
        ConcreteObserver(std::string name) : _name(name){};
        ~ConcreteObserver(){};
        
        void update() override {
            std::cout << "Observer " << _name << std::endl;
        };
    
    private:
        std::string _name;

};

int main(){
    Subject sub;

    std::shared_ptr<ConcreteObserver> obs1(new ConcreteObserver("Foo"));
    std::shared_ptr<ConcreteObserver> obs2(new ConcreteObserver("Bar"));
    std::shared_ptr<ConcreteObserver> obs3(new ConcreteObserver("Buz"));

    const Subject::Id id1 = sub.subscribe(Subject::MessageType::TYPE1, [&]{obs1->update();});
    const Subject::Id id2 = sub.subscribe(Subject::MessageType::TYPE3, [&]{obs2->update();});
    const Subject::Id id3 = sub.subscribe(Subject::MessageType::TYPE2, [&]{obs3->update();});


    std::cout << "ID1: " << id1 << " - " << "ID2: " << id2 << " - " << "ID3: " << id3 << std::endl;
    std::cout << "Subject::notify(TYPE1):" << std::endl;
    sub.notify(Subject::MessageType::TYPE1);
    std::cout << "Subject::notify(TYPE2):" << std::endl;
    sub.notify(Subject::MessageType::TYPE2);
    std::cout << "Subject::notify(TYPE3):" << std::endl;
    sub.notify(Subject::MessageType::TYPE3);
}