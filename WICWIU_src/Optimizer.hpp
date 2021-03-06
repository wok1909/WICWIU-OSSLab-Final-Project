#ifndef OPTIMIZER_H_
#define OPTIMIZER_H_    value

#include "LossFunction_utils.hpp"

enum OptimizeDirection {
    MAXIMIZE,
    MINIMIZE
};

template<typename DTYPE> class Optimizer {
private:
    float m_LearningRate;
    int m_OptimizeDirection;  // 1 or -1
    float m_weightDecayRate;

    Container<Operator<DTYPE> *> *m_ppParameters;
    int m_numOfParameter;

    int m_idOfDevice;

#ifdef __CUDNN__
    cudnnHandle_t m_pCudnnHandle;
#endif  // if __CUDNN__

public:
    Optimizer(Operator<DTYPE> **pParameters, float pLearningRate, OptimizeDirection pOptimizeDirection);
    Optimizer(Container<Operator<DTYPE> *> *pParameters, float pLearningRate, OptimizeDirection pOptimizeDirection);
    Optimizer(Container<Operator<DTYPE> *> *pParameters, float pLearningRate, float pWeightDecayRate, OptimizeDirection pOptimizeDirection);

    virtual ~Optimizer();

    int                           Alloc(Container<Operator<DTYPE> *> *pParameters, float pLearningRate, OptimizeDirection pOptimizeDirection);
    int                           Alloc(Container<Operator<DTYPE> *> *pParameters, float pLearningRate, float pWeightDecayRate, OptimizeDirection pOptimizeDirection);
    int                           Delete();

    virtual int                   UpdateParameter();
    virtual int                   UpdateParameter(Operator<DTYPE> *pTrainableTensor) = 0;

    void                          SetLearningRate(float pLearningRate);
    void                          SetTrainableTensorDegree(int pTrainableTensorDegree);
    void                          SetWeightDecayRate(int pWeightDecayRate);

    float                         GetLearningRate() const;
    int                           GetOptimizeDirection() const;
    Container<Operator<DTYPE> *>* GetTrainableTensor();
    int                           GetTrainableTensorDegree() const;
    float                         GetWeightDecayRate() const;

    int                           ResetParameterGradient();

#ifdef __CUDNN__
    void                          SetDeviceGPU(cudnnHandle_t& pCudnnHandle, unsigned int idOfDevice);
    virtual void                  InitializeAttributeForGPU(unsigned int idOfDevice) = 0;
    virtual void                  SetCudnnHandle(cudnnHandle_t& pCudnnHandle);
    virtual int                   UpdateParameterOnGPU();
    virtual int                   UpdateParameterOnGPU(Operator<DTYPE> *pTrainableTensor) = 0;

    cudnnHandle_t& GetCudnnHandle();
    int            GetDeviceID();

#endif  // if __CUDNN__
};


/*!
 * @brief Optimizer ????????? ?????????
 * @details ?????? ???????????? 0 ?????? NULL??? ???????????????,
 * @details ???????????? ??????????????? ??????????????? ?????? Optimizer??? Alloc ???????????? ????????????.
 * @param pParameters Optimizer ???????????? alloc ???????????? ??????????????? ????????? Trainable Tensor container
 * @param pLearningRate Optimizer ???????????? alloc ???????????? ??????????????? ????????? learning Rate
 * @param pOptimizeDirection Optimizer ???????????? alloc ???????????? ??????????????? ????????? optimize Direction
 * @return ??????
 * @see Optimizer<DTYPE>::Alloc(Container<Operator<DTYPE> *> *pParameters, float pLearningRate, OptimizeDirection pOptimizeDirection)
 */
template<typename DTYPE> Optimizer<DTYPE>::Optimizer(Container<Operator<DTYPE> *> *pParameters, float pLearningRate, OptimizeDirection pOptimizeDirection) {
    #ifdef __DEBUG__
    std::cout << "Optimizer::Optimizer(Operator<DTYPE> *, float, OptimizeDirection)" << '\n';
    #endif  // __DEBUG__
    m_LearningRate          = 0.f;
    m_OptimizeDirection     = 1;
    m_ppParameters          = NULL;
    m_numOfParameter = 0;
    m_weightDecayRate       = 0.f;
    m_idOfDevice            = -1;

    Alloc(pParameters, pLearningRate, pOptimizeDirection);
}

/*!
 * @brief Optimizer ????????? ?????????
 * @details ?????? ???????????? 0 ?????? NULL??? ???????????????,
 * @details ???????????? ??????????????? ??????????????? ?????? Optimizer??? Alloc ???????????? ????????????.
 * @param pParameters Optimizer ???????????? alloc ???????????? ??????????????? ????????? Trainable Tensor container
 * @param pLearningRate Optimizer ???????????? alloc ???????????? ??????????????? ????????? learning Rate
 * @param pWeightDecayRate Optimizer ???????????? alloc ???????????? ??????????????? ????????? Weight Decay Rate
 * @param pOptimizeDirection Optimizer ???????????? alloc ???????????? ??????????????? ????????? optimize Direction
 * @return ??????
 * @see Optimizer<DTYPE>::Alloc(Container<Operator<DTYPE> *> *pParameters, float pLearningRate, float pWeightDecayRate, OptimizeDirection pOptimizeDirection)
 */
template<typename DTYPE> Optimizer<DTYPE>::Optimizer(Container<Operator<DTYPE> *> *pParameters, float pLearningRate, float pWeightDecayRate, OptimizeDirection pOptimizeDirection) {
    #ifdef __DEBUG__
    std::cout << "Optimizer::Optimizer(Operator<DTYPE> *, float, OptimizeDirection)" << '\n';
    #endif  // __DEBUG__
    m_LearningRate          = 0.f;
    m_OptimizeDirection     = 1;
    m_ppParameters          = NULL;
    m_numOfParameter = 0;
    m_weightDecayRate       = 0.f;
    m_idOfDevice            = -1;

    Alloc(pParameters, pLearningRate, pWeightDecayRate, pOptimizeDirection);
}

/*!
 * @brief Optimizer ????????? ?????????
 * @details Optimizer<DTYPE>::Delete() ???????????? ???????????? ???????????? ???????????????.
 * @return ??????
 */
template<typename DTYPE> Optimizer<DTYPE>::~Optimizer() {
    #ifdef __DEBUG__
    std::cout << "Optimizer::~Optimizer()" << '\n';
    #endif  // __DEBUG__

    this->Delete();
}

/*!
 * @brief Optimizer ???????????? ?????? ???????????? ?????? ???????????? ?????????
 * @details ??????????????? ?????? ?????? ????????? ?????? Trainable Tensor Conatiner, learning rate, Optimize Direction, Weight Decay Rate ?????? ????????? ????????????.
 * @param pParameters Optimizer ??????????????? Trainable Tensor container ?????? ??????
 * @param pLearningRate Optimizer ???????????? learning Rate ?????? ??????
 * @param pOptimizeDirection Optimizer ???????????? optimize Direction ?????? ??????
 * @return TRUE
 */
template<typename DTYPE> int Optimizer<DTYPE>::Alloc(Container<Operator<DTYPE> *> *pParameters, float pLearningRate, OptimizeDirection pOptimizeDirection) {
    #ifdef __DEBUG__
    std::cout << "Optimizer::Alloc(Container<Operator<DTYPE> *> *, float , OptimizeDirection )" << '\n';
    #endif  // __DEBUG__
    m_ppParameters          = pParameters;
    m_numOfParameter = pParameters->GetSize();

    m_LearningRate = pLearningRate;

    if (pOptimizeDirection == MAXIMIZE) m_OptimizeDirection = 1;
    else if (pOptimizeDirection == MINIMIZE) m_OptimizeDirection = -1;

    return TRUE;
}

/*!
 * @brief Optimizer ???????????? ?????? ???????????? ?????? ???????????? ?????????
 * @details ??????????????? ?????? ?????? ????????? ?????? Trainable Tensor Conatiner, learning rate, Optimize Direction, Weight Decay Rate ?????? ????????? ????????????.
 * @param pParameters Optimizer ??????????????? Trainable Tensor container ?????? ??????
 * @param pLearningRate Optimizer ???????????? learning Rate ?????? ??????
 * @param pWeightDecayRate Optimizer ???????????? Weight Decay Rate ?????? ??????
 * @param pOptimizeDirection Optimizer ???????????? optimize Direction ?????? ??????
 * @return TRUE
 */
template<typename DTYPE> int Optimizer<DTYPE>::Alloc(Container<Operator<DTYPE> *> *pParameters, float pLearningRate, float pWeightDecayRate, OptimizeDirection pOptimizeDirection) {
    #ifdef __DEBUG__
    std::cout << "Optimizer::Alloc(Container<Operator<DTYPE> *> *, float , OptimizeDirection )" << '\n';
    #endif  // __DEBUG__
    m_ppParameters          = pParameters;
    m_numOfParameter = pParameters->GetSize();

    m_LearningRate = pLearningRate;

    if (pOptimizeDirection == MAXIMIZE) m_OptimizeDirection = 1;
    else if (pOptimizeDirection == MINIMIZE) m_OptimizeDirection = -1;

    m_weightDecayRate = pWeightDecayRate;
    // std::cout << "m_weightDecayRate" << m_weightDecayRate << '\n';

    return TRUE;
}

template<typename DTYPE> int Optimizer<DTYPE>::Delete() {
    return TRUE;
}

/*!
 * @brief Trainable Tensor Container??? Operator?????? ?????????????????? ???????????? ?????????????????? ?????????
 * @details ?????? ??????????????? ????????????????????? ???????????? ?????????
 * @return TRUE
 */
template<typename DTYPE> int Optimizer<DTYPE>::UpdateParameter() {
    for (int i = 0; i < m_numOfParameter; i++) {
        if((*m_ppParameters)[i]->GetIsTrainable()) UpdateParameter((*m_ppParameters)[i]);
    }
    return TRUE;
}

#ifdef __CUDNN__

template<typename DTYPE> void Optimizer<DTYPE>::SetDeviceGPU(cudnnHandle_t& pCudnnHandle, unsigned int idOfDevice) {
    checkCudaErrors(cudaSetDevice(idOfDevice));
    SetCudnnHandle(pCudnnHandle);
    m_idOfDevice = idOfDevice;
    InitializeAttributeForGPU(idOfDevice);
}

template<typename DTYPE> void Optimizer<DTYPE>::SetCudnnHandle(cudnnHandle_t& pCudnnHandle) {
    m_pCudnnHandle = pCudnnHandle;
}

template<typename DTYPE> int Optimizer<DTYPE>::GetDeviceID() {
    return m_idOfDevice;
}

template<typename DTYPE> cudnnHandle_t& Optimizer<DTYPE>::GetCudnnHandle() {
    return m_pCudnnHandle;
}

/*!
 * @brief GPU??? ????????? Trainable Tensor Container??? Operator?????? ?????????????????? ???????????? ?????????????????? ?????????
 * @details ?????? ??????????????? ????????????????????? ???????????? ?????????
 * @return TRUE
 */
template<typename DTYPE> int Optimizer<DTYPE>::UpdateParameterOnGPU() {
    for (int i = 0; i < m_numOfParameter; i++) {
        if((*m_ppParameters)[i]->GetIsTrainable()) UpdateParameterOnGPU((*m_ppParameters)[i]);
    }
    return TRUE;
}

#endif  // if __CUDNN__

template<typename DTYPE> void Optimizer<DTYPE>::SetLearningRate(float pLearningRate) {
    m_LearningRate = pLearningRate;
}

template<typename DTYPE> void Optimizer<DTYPE>::SetTrainableTensorDegree(int pTrainableTensorDegree) {
    m_numOfParameter = pTrainableTensorDegree;
}

template<typename DTYPE> void Optimizer<DTYPE>::SetWeightDecayRate(int pWeightDecayRate) {
    m_weightDecayRate = pWeightDecayRate;
}

template<typename DTYPE> float Optimizer<DTYPE>::GetLearningRate()  const {
    return m_LearningRate;
}

template<typename DTYPE> int Optimizer<DTYPE>::GetOptimizeDirection() const {
    return m_OptimizeDirection;
}

template<typename DTYPE> float Optimizer<DTYPE>::GetWeightDecayRate() const {
    return m_weightDecayRate;
}

template<typename DTYPE> Container<Operator<DTYPE> *> *Optimizer<DTYPE>::GetTrainableTensor() {
    return m_ppParameters;
}

template<typename DTYPE> int Optimizer<DTYPE>::GetTrainableTensorDegree() const {
    return m_numOfParameter;
}

/*!
 * @brief Trainable Tensor Container??? Operator?????? Gradient??? ??????????????? ?????????
 * @return TRUE
 * @ref Operator<DTYPE>::ResetGradient()
 */
template<typename DTYPE> int Optimizer<DTYPE>::ResetParameterGradient() {
    for (int i = 0; i < m_numOfParameter; i++) {
        (*m_ppParameters)[i]->ResetGradient();
    }

    return TRUE;
}

#endif  // OPTIMIZER_H_
