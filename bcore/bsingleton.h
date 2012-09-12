#ifndef BSINGLETON_H
#define BSINGLETON_H

template <typename T>
class BSingleton
{
public:
    static T *instance()
    {
        static T *inst = new T;
        return inst;
    }
private:
    BSingleton();
    BSingleton(const BSingleton &other);
    ~BSingleton();
    //
    BSingleton &operator=(const BSingleton &other);
};

#endif // BSINGLETON_H
