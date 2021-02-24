#ifndef OS_SUPERVISOR_H
#define OS_SUPERVISOR_H

#include <iostream>
#include <vector>
#include "Net.h"
#include "../Class_of_state.h"
#include "../RdPBDD.h"

using namespace std;
/****Class that defines the supervisor****/
class OS_Supervisor
{
    public:
        /***attributes***/
        string name;
        set <int> controllable;
        set <int> observable;
        std::map<string, set<int>> control;//to be set to empty at first
        /***operations***/
        /*constructor destructor*/
        OS_Supervisor();
        OS_Supervisor(string name, set <int> controllable, set <int> observable);
        virtual ~OS_Supervisor();
        /*getter setter*/
        string getName() const;
        set <int> getControllable() const;
        set <int> getObservable() const;
        std::map<string, set<int>>  getControl() const;
        void setName(string name);
        void setControllable ( set <int> controllable );
        void setObservable ( set <int> controllable );
        void setControl (std::map<string, set<int>> control);
        void addControlAction(string trace, int transToDesactivate);
        void addControlAction_Version2(Class_Of_State* aggregate, int transToDesactivate);
        void printControl();

        friend ostream& operator<<(ostream& flux, const OS_Supervisor& supervisor);
        /*methods to manage this class will be found in OS_Supervisory_Manager*/
    protected:

    private:
};

#endif // OS_SUPERVISOR_H
