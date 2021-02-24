#ifndef SOG_GENERATOR_H
#define SOG_GENERATOR_H
#include"../MDGraph.h"

class OS_SOG_Generator
{
    public:
        OS_SOG_Generator();
        virtual ~OS_SOG_Generator();
        virtual void compute_canonical_deterministic_graph_Opt(MDGraph& g) =0;
        //virtual void computeAndOpacify_SOG(MDGraph& g) =0;//moved to OS_Opacity_Manager

    protected:

    private:
};

#endif // SOG_GENERATOR_H
