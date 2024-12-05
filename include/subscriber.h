#ifndef SUBSCRIBER_H
#define SUBSCRIBER_H

#include <memory>

class Subscriber {
public:
    virtual ~Subscriber() = default;
    virtual void notify() = 0;

    using SubscriberWeakPtr = std::weak_ptr<Subscriber>;
};

#endif // SUBSCRIBER_H
