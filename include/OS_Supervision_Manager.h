#ifndef OS_SUPERVISORY_MANAGER_H
#define OS_SUPERVISORY_MANAGER_H
#include <vector>
#include "OS_Supervisor.h"

using namespace std;
/**** a class that creates and manages the supervisor; all supervisor manipulations should be here ****/
class OS_Supervision_Manager
{
    public:
        /***attributes***/
        vector<OS_Supervisor> managedSupervisors;
        string name;
        /***operations***/
        /*constructor destructor*/
        OS_Supervision_Manager();//default constructor
        OS_Supervision_Manager( OS_Supervisor supervisor, string name);
        //parameterized constructor using one supervisor and a name
        OS_Supervision_Manager( OS_Supervision_Manager&);//copy constructor
        virtual ~OS_Supervision_Manager();//destructor

        /*getter setter*/
        vector<OS_Supervisor> getManagedSupervisors();
        string getName();
        void setSupervisor(vector<OS_Supervisor> supervisors);
        void setName(string);

        /*other methods*/
        FILE* SupervisorToJson(OS_Supervisor supervisor);

        void desactivate(OS_Supervisor* supervisor, int transToDesactivate, string trace);
        void desactivate_Version2(OS_Supervisor supervisor, int transToDesactivate, Class_Of_State* aggregate);

        void Supervise(); //creates a supervisor and a manager for this supervisor, uses default constructors
        bool isControllable (OS_Supervisor supervisor, int);
        bool isObservable (OS_Supervisor supervisor, int);
        void addSupervisor(OS_Supervisor);//add this supervisor to your managed vector of supervisors
        friend ostream& operator<<(ostream& flux, const OS_Supervision_Manager& manager);
    protected:

    private:
};

#endif // OS_SUPERVISORY_MANAGER_H
