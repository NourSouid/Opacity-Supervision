#ifndef ATTACKER_H
#define ATTACKER_H

#include <iostream>
#include <cstring>
#include <set>
#include "../RdPBDD.h"
using namespace std;

class OS_Attacker
{
    public:
        string name;
        set <int> observable;
        OS_Attacker();//default constructor
        OS_Attacker(string, set<int>); //parametric constructor
        OS_Attacker(const OS_Attacker&); //copy constructor
        string getName()const;
        set<int> getObservable();
        void setName(string);
        void setObservable(set<int>);
        virtual ~OS_Attacker();//destructor
        bool isObservable(int);
        friend ostream& operator<<(ostream& flux, const OS_Attacker& attacker);
    protected:

    private:
};



#endif // ATTACKER_H
