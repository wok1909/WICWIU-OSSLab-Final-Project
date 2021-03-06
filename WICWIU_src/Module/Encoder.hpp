#ifndef __ENCODER__
#define __ENCODER__    value

#include "../Module.hpp"


template<typename DTYPE> class Encoder : public Module<DTYPE>{
private:

    int timesize;

public:

    Encoder(Operator<DTYPE> *pInput, int vocabLength, int embeddingDim, int hiddenSize, int useBias = TRUE, std::string pName = "No Name") : Module<DTYPE>(pName) {
        Alloc(pInput, vocabLength, embeddingDim, hiddenSize, useBias, pName);
    }


    virtual ~Encoder() {}

    int Alloc(Operator<DTYPE> *pInput, int vocabLength, int embeddingDim, int hiddenSize, int useBias, std::string pName) {

        timesize = pInput->GetResult()->GetTimeSize();
        this->SetInput(pInput);

        Operator<DTYPE> *out = pInput;

        out = new EmbeddingLayer<DTYPE>(out, vocabLength, embeddingDim, "Embedding");

        // out = new RecurrentLayer<DTYPE>(out, embeddingDim, hiddenSize, NULL, useBias, "Recur_1");
        out = new LSTMLayer<DTYPE>(out, embeddingDim, hiddenSize, NULL, useBias, "Recur_1");
        // out = new GRULayer<DTYPE>(out, embeddingDim, hiddenSize, NULL, useBias, "Recur_1");



        this->AnalyzeGraph(out);

        return TRUE;
    }

    //이거 없애도 되는거 같은데!!!....???                                        !!! 중요!!! 여기는 없어도 되는거 같음!!!!!!!!!!!! 확인해보고 수정하기!!!
    int ForwardPropagate(int pTime=0) {


        int numOfExcutableOperator = this->GetNumOfExcutableOperator();
        Container<Operator<DTYPE> *> *ExcutableOperator = this->GetExcutableOperatorContainer();

        for (int i = 0; i < numOfExcutableOperator; i++) {
            (*ExcutableOperator)[i]->ForwardPropagate(pTime);
        }

        return TRUE;
    }

    int BackPropagate(int pTime=0) {

        //std::cout<<"****************encoder Backward 호출****************"<<'\n';

        int numOfExcutableOperator = this->GetNumOfExcutableOperator();
        Container<Operator<DTYPE> *> *ExcutableOperator = this->GetExcutableOperatorContainer();

        for (int i = numOfExcutableOperator - 1; i >= 0; i--) {
            (*ExcutableOperator)[i]->BackPropagate(pTime);
        }



        return TRUE;
    }
};



#endif  // __ENCODER__
