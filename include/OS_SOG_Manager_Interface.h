#ifndef OS_SOG_MANAGER_INTERFACE_H
#define OS_SOG_MANAGER_INTERFACE_H
#include "../RdPBDD.h"
#include "../MDGraph.h"
using namespace std;

class OS_SOG_Manager_Interface
{
    public:
        OS_SOG_Manager_Interface();
        virtual ~OS_SOG_Manager_Interface();
        virtual Trans getTransition (MDGraph*, Class_Of_State*, Class_Of_State*) =0;
        virtual void addSOG (MDGraph*) =0;
        virtual void deleteSOG (MDGraph*) = 0;
        virtual MDGraph* findSOG (MDGraph*) =0;

    protected:

    private:
};

#endif // OS_SOG_MANAGER_INTERFACE_H
