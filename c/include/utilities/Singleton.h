#ifndef SINGLETON_H_INCLUDE__
#define SINGLETON_H_INCLUDE__

#include <assert.h>

//////////////////////////////////////////////////////////////////////////
//功能, 继承于此类后成为单件类

template < class T > 
class CSingleton
{
public:
    
    //Gets a reference to the instance of the singleton class.
    static T & Instance()
    {
        return *InstancePtr();
    };

    static T * InstancePtr()
    {
        if (ms_instance == 0) ms_instance = new T;
        
        assert(ms_instance != 0);
        
        return ms_instance;
    };
    //Destroys the singleton class instance.
    static void DestroyInstance()
    {
        delete ms_instance;
        ms_instance = 0;
    };
    
protected:
    
    // shield the constructor and destructor to prevent outside sources
    // from creating or destroying a CSingleton instance.
    
    //Default constructor.
    CSingleton()
    {
    };

    //Destructor.
    virtual ~CSingleton()
    {
    };
    
private:
    
    //Copy constructor.
    //No copies do not implement
    CSingleton(const CSingleton& source);
    CSingleton &operator=(const CSingleton &rhs);

    static T* ms_instance; // singleton class instance
};

// static class member initialisation.
template < class T > T* CSingleton<T>::ms_instance = 0;

#endif //SINGLETON_H_INCLUDE__
