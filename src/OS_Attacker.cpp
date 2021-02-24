#include "OS_Attacker.h"
#include <iostream>

OS_Attacker::OS_Attacker()
{
    set<int> observable;
    this->setName("-undefined-");
    this->setObservable(observable);
}//default constructor
OS_Attacker::OS_Attacker(string name, set<int> observable){
    this->setName(name);
    this->setObservable(observable);

} //parametric constructor

OS_Attacker::OS_Attacker(const OS_Attacker& attacker){
    this->setName(attacker.name);
    this->setObservable(attacker.observable);

} //copy constructor
string OS_Attacker::getName()const{
    return this->name;
}
set<int> OS_Attacker::getObservable(){
    return this->observable;
}
void OS_Attacker::setName(string name){
    this->name=name;
}
void OS_Attacker::setObservable(set<int> observable){
    this->observable=observable;

}
OS_Attacker::~OS_Attacker()
{
    //dtor
}
ostream& operator<<(ostream& flux, const OS_Attacker& attacker)
 {
        flux<<"***Attacker*** \n"<< "Attacker's name : " <<attacker.name <<"\nAttacker's observation : ";
        set<int>::iterator ptr;
        for (ptr = attacker.observable.begin(); ptr != attacker.observable.end(); ptr++)
        {
            flux<<"t"<<*ptr<<",\t";
        }
        return flux;
}
bool OS_Attacker::isObservable(int trans){
   if(this->observable.find(trans)!=this->observable.end())
        return true;
    return false;

}
