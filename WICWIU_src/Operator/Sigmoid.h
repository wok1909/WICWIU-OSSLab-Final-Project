#ifndef SIGMOID_H_
#define SIGMOID_H_    value

#include "..//Operator.h"

template<typename DTYPE>
class Sigmoid : public Operator<DTYPE>{
public:
    Sigmoid(Operator<DTYPE> *pInput, std::string pName) : Operator<DTYPE>(pInput, pName) {
        #ifdef __DEBUG__
        std::cout << "Sigmoid::Sigmoid(Operator *)" << '\n';
        #endif  // __DEBUG__
        this->Alloc(pInput);
    }

    ~Sigmoid() {
        std::cout << "Sigmoid::~Sigmoid()" << '\n';
    }

    int Alloc(Operator<DTYPE> *pInput) {
        #ifdef __DEBUG__
        std::cout << "Sigmoid::Alloc(Operator *, Operator *)" << '\n';
        #endif  // __DEBUG__

        int timesize    = pInput->GetResult()->GetTimeSize();
        int batchsize   = pInput->GetResult()->GetBatchSize();
        int channelsize = pInput->GetResult()->GetChannelSize();
        int rowsize     = pInput->GetResult()->GetRowSize();
        int colsize     = pInput->GetResult()->GetColSize();

        this->SetResult(new Tensor<DTYPE>(timesize, batchsize, channelsize, rowsize, colsize));

        this->SetDelta(new Tensor<DTYPE>(timesize, batchsize, channelsize, rowsize, colsize));

        return TRUE;
    }

    int ForwardPropagate(int pTime = 0, int pThreadNum = 0) {
        Tensor<DTYPE> *input  = this->GetInput()[0]->GetResult();
        Tensor<DTYPE> *result = this->GetResult();

        int timesize    = result->GetTimeSize();
        int batchsize   = result->GetBatchSize();
        int channelsize = result->GetChannelSize();
        int rowsize     = result->GetRowSize();
        int colsize     = result->GetColSize();

        Shape *resultTenShape = result->GetShape();

        int ti = pTime;
        int numOfThread = this->GetNumOfThread();

        for (int ba = pThreadNum; ba < batchsize; ba += numOfThread) {
            for (int ch = 0; ch < channelsize; ch++) {
                for (int ro = 0; ro < rowsize; ro++) {
                    for (int co = 0; co < colsize; co++) {
                        (*result)[Index5D(resultTenShape, ti, ba, ch, ro, co)]
                            = this->SIGMOID((*input)[Index5D(resultTenShape, ti, ba, ch, ro, co)]);
                    }
                }
            }
        }


        return TRUE;
    }

    int BackPropagate(int pTime = 0, int pThreadNum = 0) {
        Tensor<DTYPE> *result      = this->GetResult();
        Tensor<DTYPE> *this_delta  = this->GetDelta();
        Tensor<DTYPE> *input_delta = this->GetInput()[0]->GetDelta();

        int timesize    = result->GetTimeSize();
        int batchsize   = result->GetBatchSize();
        int channelsize = result->GetChannelSize();
        int rowsize     = result->GetRowSize();
        int colsize     = result->GetColSize();

        Shape *resultTenShape = result->GetShape();

        int ti = pTime;
        int numOfThread = this->GetNumOfThread();

        for (int ba = pThreadNum; ba < batchsize; ba += numOfThread) {
            for (int ch = 0; ch < channelsize; ch++) {
                for (int ro = 0; ro < rowsize; ro++) {
                    for (int co = 0; co < colsize; co++) {
                        (*input_delta)[Index5D(resultTenShape, ti, ba, ch, ro, co)]
                            += (*result)[Index5D(resultTenShape, ti, ba, ch, ro, co)]
                               * (1 - (*result)[Index5D(resultTenShape, ti, ba, ch, ro, co)])
                               * (*this_delta)[Index5D(resultTenShape, ti, ba, ch, ro, co)];
                    }
                }
            }
        }

        return TRUE;
    }

#ifdef __CUDNN__
    int ForwardPropagateOnGPU(int pTime) {
        this->ForwardPropagate(pTime);
        return TRUE;
    }

    int BackPropagateOnGPU(int pTime) {
        this->BackPropagate(pTime);

        return TRUE;
    }

#endif  // __CUDNN__

    inline DTYPE SIGMOID(DTYPE data) {
        return 1.F / (1.F + (DTYPE)exp(-data));
    }
};

#endif  // SIGMOID_H_
