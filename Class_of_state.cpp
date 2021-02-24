#include "Class_of_state.h"
//not used yet
 Class_Of_State operator-(Class_Of_State& a, Class_Of_State& b){
            a.class_state -= (a.class_state)- (b.class_state) ;//other parameters of a do not change
            return a;}//must be outside the class but do not compile otherwise
