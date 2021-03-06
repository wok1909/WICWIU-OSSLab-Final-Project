#ifndef __RECURRENT_LAYER__
#define __RECURRENT_LAYER__    value

#include "../Module.hpp"


template<typename DTYPE> class RecurrentLayer : public Module<DTYPE>{
private:
public:
    /*!
    @brief RecurrentLayer 클래스 생성자
    @details RecurrentLayer 클래스의 Alloc 함수를 호출한다.*/
    RecurrentLayer(Operator<DTYPE> *pInput, int inputSize, int hiddenSize, Operator<DTYPE> *initHidden, int useBias = TRUE, std::string pName = "No Name") : Module<DTYPE>(pName) {
        Alloc(pInput, inputSize, hiddenSize, initHidden, useBias, pName);
    }

    /*!
    @brief Recurrentlayer 클래스 소멸자
    @details 단, 동적 할당 받은 Operator들은 NeuralNetwork에서 할당 해제한다.  */
    virtual ~RecurrentLayer() {}

    /*!
    @brief RecurrentLayer 그래프를 동적으로 할당 및 구성하는 메소드
    @details Input Operator의 Element에 대해 2D Convolution을 수행한다.
    @param pInput
    @param useBias Bias 사용 유무, 0일 시 사용 안 함, 0이 아닐 시 사용
    @param pName Module의 이름
    @return TRUE
    @see
    */
    int Alloc(Operator<DTYPE> *pInput, int inputSize, int hiddenSize, Operator<DTYPE>* initHidden, int useBias, std::string pName) {
        this->SetInput(pInput);
        // this->SetInput(2, pInput, initHidden);

        Operator<DTYPE> *out = pInput;

        //xavier initialization
        // float xavier_i = sqrt(2/(inputSize+hiddenSize));
        // float xavier_h = sqrt(2/(hiddenSize+hiddenSize));

        //weight
        Tensorholder<DTYPE> *pWeight_x2h = new Tensorholder<DTYPE>(Tensor<DTYPE>::Random_normal(1, 1, 1, hiddenSize, inputSize, 0.0, 0.01), "RecurrentLayer_pWeight_x2h_" + pName);
        Tensorholder<DTYPE> *pWeight_h2h = new Tensorholder<DTYPE>(Tensor<DTYPE>::Random_normal(1, 1, 1, hiddenSize, hiddenSize, 0.0, 0.01), "RecurrentLayer_pWeight_h2h_" + pName);

#ifdef __CUDNN__
        pWeight_h2h = new Tensorholder<DTYPE>(Tensor<DTYPE>::Random_normal(1, 1, 1, hiddenSize, hiddenSize+inputSize+1, 0.0, 0.01), "RecurrentLayer_pWeight_h2h_" + pName);    //For 1 bias option
#endif  // __CUDNN__


        //bias
        Tensorholder<DTYPE> *rBias = new Tensorholder<DTYPE>(Tensor<DTYPE>::Constants(1, 1, 1, 1, hiddenSize, 0.f), "RNN_Bias_" + pName);

        out = new Recurrent<DTYPE>(out, pWeight_x2h, pWeight_h2h, rBias, initHidden);

        this->AnalyzeGraph(out);

        return TRUE;
    }

};


#endif  // __RECURRENT_LAYER__
