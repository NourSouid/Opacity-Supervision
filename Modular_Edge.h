#ifndef H_MODULAR_EDGE_H
#define H_MODULAR_EDGE_H
#include <set>
#include<string>
class Modular_Class_Of_State;
typedef pair<Modular_Class_Of_State*, string> Modular_Edge;
struct LessModularEdge
{
	bool operator()(const Modular_Edge,const Modular_Edge)const;
};
inline bool LessModularEdge::operator ()(const Modular_Edge a1,const Modular_Edge a2)const
{
	//cout<<"on compare les etiquettes des arcs : "<<a1.second<<" avec "<<a2.second<<endl;
	return (a1.first<a2.first);
	
}
typedef Set<Modular_Edge,LessModularEdge> Modular_Edges;
#endif
