﻿//
// Copyright © 2017 Arm Ltd. All rights reserved.
// SPDX-License-Identifier: MIT
//

#include "WorkloadTestUtils.hpp"

#include <armnn/Exceptions.hpp>

#include <backendsCommon/CpuTensorHandle.hpp>
#include <backendsCommon/Workload.hpp>

#include <reference/workloads/RefWorkloads.hpp>
#include <reference/RefWorkloadFactory.hpp>

#include <boost/test/unit_test.hpp>

using namespace armnn;

BOOST_AUTO_TEST_SUITE(WorkloadInfoValidation)



BOOST_AUTO_TEST_CASE(QueueDescriptor_Validate_WrongNumOfInputsOutputs)
{
    InputQueueDescriptor invalidData;
    WorkloadInfo invalidInfo;
    //Invalid argument exception is expected, because no inputs and no outputs were defined.
    BOOST_CHECK_THROW(RefWorkloadFactory().CreateInput(invalidData, invalidInfo), armnn::InvalidArgumentException);
}

BOOST_AUTO_TEST_CASE(RefPooling2dFloat32Workload_Validate_WrongDimTensor)
{
    armnn::TensorInfo inputTensorInfo;
    armnn::TensorInfo outputTensorInfo;

    unsigned int inputShape[]  = {2, 3, 4}; // <- Invalid - input tensor has to be 4D.
    unsigned int outputShape[] = {2, 3, 4, 5};

    outputTensorInfo = armnn::TensorInfo(4, outputShape, armnn::DataType::Float32);
    inputTensorInfo  = armnn::TensorInfo(3, inputShape, armnn::DataType::Float32);

    Pooling2dQueueDescriptor invalidData;
    WorkloadInfo           invalidInfo;

    AddOutputToWorkload(invalidData, invalidInfo, outputTensorInfo, nullptr);
    AddInputToWorkload(invalidData, invalidInfo, inputTensorInfo, nullptr);

    // Invalid argument exception is expected, input tensor has to be 4D.
    BOOST_CHECK_THROW(RefPooling2dWorkload(invalidData, invalidInfo), armnn::InvalidArgumentException);
}

BOOST_AUTO_TEST_CASE(SoftmaxQueueDescriptor_Validate_WrongInputHeight)
{
    unsigned int inputHeight = 1;
    unsigned int inputWidth = 1;
    unsigned int inputChannels = 4;
    unsigned int inputNum = 2;

    unsigned int outputChannels = inputChannels;
    unsigned int outputHeight = inputHeight + 1;    //Makes data invalid - Softmax expects height and width to be 1.
    unsigned int outputWidth = inputWidth;
    unsigned int outputNum = inputNum;

    armnn::TensorInfo inputTensorInfo;
    armnn::TensorInfo outputTensorInfo;

    unsigned int inputShape[] = { inputNum, inputChannels, inputHeight, inputWidth };
    unsigned int outputShape[] = { outputNum, outputChannels, outputHeight, outputWidth };

    inputTensorInfo = armnn::TensorInfo(4, inputShape, armnn::DataType::Float32);
    outputTensorInfo = armnn::TensorInfo(4, outputShape, armnn::DataType::Float32);

    SoftmaxQueueDescriptor invalidData;
    WorkloadInfo           invalidInfo;

    AddInputToWorkload(invalidData, invalidInfo, inputTensorInfo, nullptr);
    AddOutputToWorkload(invalidData, invalidInfo, outputTensorInfo, nullptr);

    //Invalid argument exception is expected, because height != 1.
    BOOST_CHECK_THROW(RefSoftmaxWorkload(invalidData, invalidInfo), armnn::InvalidArgumentException);
}

BOOST_AUTO_TEST_CASE(FullyConnectedQueueDescriptor_Validate_RequiredDataMissing)
{
    unsigned int inputWidth = 1;
    unsigned int inputHeight = 1;
    unsigned int inputChannels = 5;
    unsigned int inputNum = 2;

    unsigned int outputWidth = 1;
    unsigned int outputHeight = 1;
    unsigned int outputChannels = 3;
    unsigned int outputNum = 2;

    // Define the tensor descriptors.
    armnn::TensorInfo inputTensorInfo;
    armnn::TensorInfo outputTensorInfo;
    armnn::TensorInfo weightsDesc;
    armnn::TensorInfo biasesDesc;

    unsigned int inputShape[] = { inputNum, inputChannels, inputHeight, inputWidth };
    unsigned int outputShape[] = { outputNum, outputChannels, outputHeight, outputWidth };
    unsigned int weightsShape[] = { 1, 1, inputChannels, outputChannels };
    unsigned int biasShape[] = { 1, outputChannels, outputHeight, outputWidth };

    inputTensorInfo = armnn::TensorInfo(4, inputShape, armnn::DataType::Float32);
    outputTensorInfo = armnn::TensorInfo(4, outputShape, armnn::DataType::Float32);
    weightsDesc = armnn::TensorInfo(4, weightsShape, armnn::DataType::Float32);
    biasesDesc = armnn::TensorInfo(4, biasShape, armnn::DataType::Float32);

    FullyConnectedQueueDescriptor invalidData;
    WorkloadInfo                  invalidInfo;

    ScopedCpuTensorHandle weightTensor(weightsDesc);
    ScopedCpuTensorHandle biasTensor(biasesDesc);

    AddInputToWorkload(invalidData, invalidInfo, inputTensorInfo, nullptr);
    AddOutputToWorkload(invalidData, invalidInfo, outputTensorInfo, nullptr);
    invalidData.m_Weight = &weightTensor;
    invalidData.m_Bias = &biasTensor;
    invalidData.m_Parameters.m_BiasEnabled = true;
    invalidData.m_Parameters.m_TransposeWeightMatrix = false;


    //Invalid argument exception is expected, because not all required fields have been provided.
    //In particular inputsData[0], outputsData[0] and weightsData can not be null.
    BOOST_CHECK_THROW(RefFullyConnectedWorkload(invalidData, invalidInfo), armnn::InvalidArgumentException);
}


BOOST_AUTO_TEST_CASE(NormalizationQueueDescriptor_Validate_WrongInputHeight)
{
    constexpr unsigned int inputNum = 5;
    constexpr unsigned int inputHeight   = 32;
    constexpr unsigned int inputWidth    = 24;
    constexpr unsigned int inputChannels = 3;

    constexpr unsigned int outputNum = inputNum;
    constexpr unsigned int outputChannels = inputChannels;
    constexpr unsigned int outputHeight = inputHeight + 1; //Makes data invalid - normalization requires.
                                                           //Input and output to have the same dimensions.
    constexpr unsigned int outputWidth  = inputWidth;


    armnn::TensorInfo inputTensorInfo;
    armnn::TensorInfo outputTensorInfo;

    unsigned int inputShape[]  = {inputNum, inputChannels, inputHeight, inputWidth};
    unsigned int outputShape[] = {outputNum, outputChannels, outputHeight, outputWidth};

    inputTensorInfo = armnn::TensorInfo(4, inputShape, armnn::DataType::Float32);
    outputTensorInfo = armnn::TensorInfo(4, outputShape, armnn::DataType::Float32);


    armnn::NormalizationAlgorithmMethod normMethod = armnn::NormalizationAlgorithmMethod::LocalBrightness;
    armnn::NormalizationAlgorithmChannel normChannel = armnn::NormalizationAlgorithmChannel::Across;
    float alpha = 1.f;
    float beta = 1.f;
    float kappa = 1.f;
    uint32_t normSize = 5;

    NormalizationQueueDescriptor invalidData;
    WorkloadInfo                 invalidInfo;

    AddInputToWorkload(invalidData, invalidInfo, inputTensorInfo, nullptr);
    AddOutputToWorkload(invalidData, invalidInfo, outputTensorInfo, nullptr);
    invalidData.m_Parameters.m_NormChannelType = normChannel;
    invalidData.m_Parameters.m_NormMethodType  = normMethod;
    invalidData.m_Parameters.m_NormSize        = normSize;
    invalidData.m_Parameters.m_Alpha           = alpha;
    invalidData.m_Parameters.m_Beta            = beta;
    invalidData.m_Parameters.m_K               = kappa;

    //Invalid argument exception is expected, because input height != output height.
    BOOST_CHECK_THROW(RefNormalizationWorkload(invalidData, invalidInfo), armnn::InvalidArgumentException);
}

BOOST_AUTO_TEST_CASE(SplitterQueueDescriptor_Validate_WrongWindow)
{
    constexpr unsigned int inputNum = 1;
    constexpr unsigned int inputHeight   = 32;
    constexpr unsigned int inputWidth    = 24;
    constexpr unsigned int inputChannels = 3;

    constexpr unsigned int outputNum = inputNum;
    constexpr unsigned int outputChannels = inputChannels;
    constexpr unsigned int outputHeight = 18;
    constexpr unsigned int outputWidth  = inputWidth;


    armnn::TensorInfo inputTensorInfo;
    armnn::TensorInfo outputTensorInfo;

    unsigned int inputShape[]  = {inputNum, inputChannels, inputHeight, inputWidth};
    unsigned int outputShape[] = {outputNum, outputChannels, outputHeight, outputWidth};

    inputTensorInfo = armnn::TensorInfo(4, inputShape, armnn::DataType::Float32);
    outputTensorInfo = armnn::TensorInfo(4, outputShape, armnn::DataType::Float32);

    SplitterQueueDescriptor invalidData;
    WorkloadInfo            invalidInfo;

    AddInputToWorkload(invalidData, invalidInfo, inputTensorInfo, nullptr);
    AddOutputToWorkload(invalidData, invalidInfo, outputTensorInfo, nullptr);

    // Invalid, since it has only 3 dimensions while the input tensor is 4d.
    std::vector<unsigned int> wOrigin = {0, 0, 0};
    armnn::SplitterQueueDescriptor::ViewOrigin window(wOrigin);
    invalidData.m_ViewOrigins.push_back(window);

    BOOST_TEST_INFO("Invalid argument exception is expected, because split window dimensionality does not "
        "match input.");
    BOOST_CHECK_THROW(RefSplitterWorkload(invalidData, invalidInfo), armnn::InvalidArgumentException);

    // Invalid, since window extends past the boundary of input tensor.
    std::vector<unsigned int> wOrigin3 = {0, 0, 15, 0};
    armnn::SplitterQueueDescriptor::ViewOrigin window3(wOrigin3);
    invalidData.m_ViewOrigins[0] = window3;
    BOOST_TEST_INFO("Invalid argument exception is expected (wOrigin3[2]+ outputHeight > inputHeight");
    BOOST_CHECK_THROW(RefSplitterWorkload(invalidData, invalidInfo), armnn::InvalidArgumentException);


    std::vector<unsigned int> wOrigin4 = {0, 0, 0, 0};
    armnn::SplitterQueueDescriptor::ViewOrigin window4(wOrigin4);
    invalidData.m_ViewOrigins[0] = window4;

    std::vector<unsigned int> wOrigin5 = {1, 16, 20, 2};
    armnn::SplitterQueueDescriptor::ViewOrigin window5(wOrigin4);
    invalidData.m_ViewOrigins.push_back(window5);

    BOOST_TEST_INFO("Invalid exception due to number of split windows not matching number of outputs.");
    BOOST_CHECK_THROW(RefSplitterWorkload(invalidData, invalidInfo), armnn::InvalidArgumentException);
}


BOOST_AUTO_TEST_CASE(ConcatQueueDescriptor_Validate_WrongWindow)
{
    constexpr unsigned int inputNum = 1;
    constexpr unsigned int inputChannels = 3;
    constexpr unsigned int inputHeight   = 32;
    constexpr unsigned int inputWidth    = 24;

    constexpr unsigned int outputNum = 1;
    constexpr unsigned int outputChannels = 3;
    constexpr unsigned int outputHeight = 32;
    constexpr unsigned int outputWidth  = 24;


    armnn::TensorInfo inputTensorInfo;
    armnn::TensorInfo outputTensorInfo;

    unsigned int inputShape[]  = {inputNum, inputChannels, inputHeight, inputWidth};
    unsigned int outputShape[] = {outputNum, outputChannels, outputHeight, outputWidth};

    inputTensorInfo = armnn::TensorInfo(4, inputShape, armnn::DataType::Float32);
    outputTensorInfo = armnn::TensorInfo(4, outputShape, armnn::DataType::Float32);

    ConcatQueueDescriptor invalidData;
    WorkloadInfo          invalidInfo;

    AddInputToWorkload(invalidData, invalidInfo, inputTensorInfo, nullptr);
    AddOutputToWorkload(invalidData, invalidInfo, outputTensorInfo, nullptr);

    // Invalid, since it has only 3 dimensions while the input tensor is 4d.
    std::vector<unsigned int> wOrigin = {0, 0, 0};
    armnn::ConcatQueueDescriptor::ViewOrigin window(wOrigin);
    invalidData.m_ViewOrigins.push_back(window);

    BOOST_TEST_INFO("Invalid argument exception is expected, because merge window dimensionality does not "
        "match input.");
    BOOST_CHECK_THROW(RefConcatWorkload(invalidData, invalidInfo), armnn::InvalidArgumentException);

    // Invalid, since window extends past the boundary of output tensor.
    std::vector<unsigned int> wOrigin3 = {0, 0, 15, 0};
    armnn::ConcatQueueDescriptor::ViewOrigin window3(wOrigin3);
    invalidData.m_ViewOrigins[0] = window3;
    BOOST_TEST_INFO("Invalid argument exception is expected (wOrigin3[2]+ inputHeight > outputHeight");
    BOOST_CHECK_THROW(RefConcatWorkload(invalidData, invalidInfo), armnn::InvalidArgumentException);


    std::vector<unsigned int> wOrigin4 = {0, 0, 0, 0};
    armnn::ConcatQueueDescriptor::ViewOrigin window4(wOrigin4);
    invalidData.m_ViewOrigins[0] = window4;

    std::vector<unsigned int> wOrigin5 = {1, 16, 20, 2};
    armnn::ConcatQueueDescriptor::ViewOrigin window5(wOrigin4);
    invalidData.m_ViewOrigins.push_back(window5);

    BOOST_TEST_INFO("Invalid exception due to number of merge windows not matching number of inputs.");
    BOOST_CHECK_THROW(RefConcatWorkload(invalidData, invalidInfo), armnn::InvalidArgumentException);
}

BOOST_AUTO_TEST_CASE(AdditionQueueDescriptor_Validate_InputNumbers)
{
    armnn::TensorInfo input1TensorInfo;
    armnn::TensorInfo input2TensorInfo;
    armnn::TensorInfo input3TensorInfo;
    armnn::TensorInfo outputTensorInfo;

    unsigned int shape[]  = {1, 1, 1, 1};

    input1TensorInfo = armnn::TensorInfo(4, shape, armnn::DataType::Float32);
    input2TensorInfo = armnn::TensorInfo(4, shape, armnn::DataType::Float32);
    input3TensorInfo = armnn::TensorInfo(4, shape, armnn::DataType::Float32);
    outputTensorInfo = armnn::TensorInfo(4, shape, armnn::DataType::Float32);

    AdditionQueueDescriptor invalidData;
    WorkloadInfo            invalidInfo;

    AddInputToWorkload(invalidData, invalidInfo, input1TensorInfo, nullptr);
    AddOutputToWorkload(invalidData, invalidInfo, outputTensorInfo, nullptr);

    // Too few inputs.
    BOOST_CHECK_THROW(RefAdditionWorkload(invalidData, invalidInfo), armnn::InvalidArgumentException);

    AddInputToWorkload(invalidData, invalidInfo, input2TensorInfo, nullptr);

    // Correct.
    BOOST_CHECK_NO_THROW(RefAdditionWorkload(invalidData, invalidInfo));

    AddInputToWorkload(invalidData, invalidInfo, input3TensorInfo, nullptr);

    // Too many inputs.
    BOOST_CHECK_THROW(RefAdditionWorkload(invalidData, invalidInfo), armnn::InvalidArgumentException);
}

BOOST_AUTO_TEST_CASE(AdditionQueueDescriptor_Validate_InputShapes)
{
    armnn::TensorInfo input1TensorInfo;
    armnn::TensorInfo input2TensorInfo;
    armnn::TensorInfo outputTensorInfo;

    unsigned int shape1[] = {1, 1, 2, 1};
    unsigned int shape2[] = {1, 1, 3, 2};

    // Incompatible shapes even with broadcasting.
    {
        input1TensorInfo = armnn::TensorInfo(4, shape1, armnn::DataType::Float32);
        input2TensorInfo = armnn::TensorInfo(4, shape2, armnn::DataType::Float32);
        outputTensorInfo = armnn::TensorInfo(4, shape1, armnn::DataType::Float32);

        AdditionQueueDescriptor invalidData;
        WorkloadInfo            invalidInfo;

        AddInputToWorkload(invalidData, invalidInfo, input1TensorInfo, nullptr);
        AddInputToWorkload(invalidData, invalidInfo, input2TensorInfo, nullptr);
        AddOutputToWorkload(invalidData, invalidInfo, outputTensorInfo, nullptr);

        BOOST_CHECK_THROW(RefAdditionWorkload(invalidData, invalidInfo), armnn::InvalidArgumentException);
    }

    // Output size not compatible with input sizes.
    {
        input1TensorInfo = armnn::TensorInfo(4, shape1, armnn::DataType::Float32);
        input2TensorInfo = armnn::TensorInfo(4, shape1, armnn::DataType::Float32);
        outputTensorInfo = armnn::TensorInfo(4, shape2, armnn::DataType::Float32);

        AdditionQueueDescriptor invalidData;
        WorkloadInfo            invalidInfo;

        AddInputToWorkload(invalidData, invalidInfo, input1TensorInfo, nullptr);
        AddInputToWorkload(invalidData, invalidInfo, input2TensorInfo, nullptr);
        AddOutputToWorkload(invalidData, invalidInfo, outputTensorInfo, nullptr);

        // Output differs.
        BOOST_CHECK_THROW(RefAdditionWorkload(invalidData, invalidInfo), armnn::InvalidArgumentException);
    }
}

BOOST_AUTO_TEST_CASE(MultiplicationQueueDescriptor_Validate_InputTensorDimensionMismatch)
{
    armnn::TensorInfo input0TensorInfo;
    armnn::TensorInfo input1TensorInfo;
    armnn::TensorInfo outputTensorInfo;

    constexpr unsigned int input0Shape[] = { 2, 2, 4, 4 };
    constexpr std::size_t dimensionCount = std::extent<decltype(input0Shape)>::value;

    // Checks dimension consistency for input tensors.
    for (unsigned int dimIndex = 0; dimIndex < dimensionCount; ++dimIndex)
    {
        unsigned int input1Shape[dimensionCount];
        for (unsigned int i = 0; i < dimensionCount; ++i)
        {
            input1Shape[i] = input0Shape[i];
        }

        ++input1Shape[dimIndex];

        input0TensorInfo = armnn::TensorInfo(dimensionCount, input0Shape, armnn::DataType::Float32);
        input1TensorInfo = armnn::TensorInfo(dimensionCount, input1Shape, armnn::DataType::Float32);
        outputTensorInfo = armnn::TensorInfo(dimensionCount, input0Shape, armnn::DataType::Float32);

        MultiplicationQueueDescriptor invalidData;
        WorkloadInfo                  invalidInfo;

        AddOutputToWorkload(invalidData, invalidInfo, outputTensorInfo, nullptr);
        AddInputToWorkload(invalidData, invalidInfo, input0TensorInfo, nullptr);
        AddInputToWorkload(invalidData, invalidInfo, input1TensorInfo, nullptr);

        BOOST_CHECK_THROW(RefMultiplicationWorkload(invalidData, invalidInfo), armnn::InvalidArgumentException);
    }

    // Checks dimension consistency for input and output tensors.
    for (unsigned int dimIndex = 0; dimIndex < dimensionCount; ++dimIndex)
    {
        unsigned int outputShape[dimensionCount];
        for (unsigned int i = 0; i < dimensionCount; ++i)
        {
            outputShape[i] = input0Shape[i];
        }

        ++outputShape[dimIndex];

        input0TensorInfo = armnn::TensorInfo(dimensionCount, input0Shape, armnn::DataType::Float32);
        input1TensorInfo = armnn::TensorInfo(dimensionCount, input0Shape, armnn::DataType::Float32);
        outputTensorInfo = armnn::TensorInfo(dimensionCount, outputShape, armnn::DataType::Float32);

        MultiplicationQueueDescriptor invalidData;
        WorkloadInfo                  invalidInfo;

        AddOutputToWorkload(invalidData, invalidInfo, outputTensorInfo, nullptr);
        AddInputToWorkload(invalidData, invalidInfo, input0TensorInfo, nullptr);
        AddInputToWorkload(invalidData, invalidInfo, input1TensorInfo, nullptr);

        BOOST_CHECK_THROW(RefMultiplicationWorkload(invalidData, invalidInfo), armnn::InvalidArgumentException);
    }
}

BOOST_AUTO_TEST_CASE(ReshapeQueueDescriptor_Validate_MismatchingNumElements)
{
    armnn::TensorInfo inputTensorInfo;
    armnn::TensorInfo outputTensorInfo;

    // The input and output shapes should have the same number of elements, but these don't.
    unsigned int inputShape[] = { 1, 1, 2, 3 };
    unsigned int outputShape[] = { 1, 1, 1, 2 };

    inputTensorInfo = armnn::TensorInfo(4, inputShape, armnn::DataType::Float32);
    outputTensorInfo = armnn::TensorInfo(4, outputShape, armnn::DataType::Float32);

    ReshapeQueueDescriptor invalidData;
    WorkloadInfo           invalidInfo;

    AddInputToWorkload(invalidData, invalidInfo, inputTensorInfo, nullptr);
    AddOutputToWorkload(invalidData, invalidInfo, outputTensorInfo, nullptr);

    // InvalidArgumentException is expected, because the number of elements don't match.
    BOOST_CHECK_THROW(RefReshapeWorkload(invalidData, invalidInfo), armnn::InvalidArgumentException);
}


BOOST_AUTO_TEST_CASE(LstmQueueDescriptor_Validate)
{
    armnn::DataType dataType = armnn::DataType::Float32;

    float qScale = 0.0f;
    int32_t qOffset = 0;

    unsigned int batchSize = 2;
    unsigned int outputSize = 3;
    unsigned int inputSize = 5;
    unsigned numUnits = 4;

    armnn::TensorInfo inputTensorInfo({batchSize , inputSize}, dataType,  qScale, qOffset );
    armnn::TensorInfo outputStateInTensorInfo({batchSize , outputSize}, dataType, qScale, qOffset);
    armnn::TensorInfo cellStateInTensorInfo({batchSize , numUnits}, dataType, qScale, qOffset);

    // Scratch buffer size with CIFG [batchSize, numUnits * 4]
    armnn::TensorInfo scratchBufferTensorInfo({batchSize, numUnits * 4}, dataType, qScale, qOffset);
    armnn::TensorInfo cellStateOutTensorInfo({batchSize, numUnits}, dataType, qScale, qOffset);
    armnn::TensorInfo outputStateOutTensorInfo({batchSize, outputSize}, dataType, qScale, qOffset);
    armnn::TensorInfo outputTensorInfo({batchSize, outputSize}, dataType, qScale, qOffset);

    armnn::TensorInfo tensorInfo3({outputSize}, dataType, qScale, qOffset);
    armnn::TensorInfo tensorInfo4({numUnits}, dataType, qScale, qOffset);
    armnn::TensorInfo tensorInfo4x5({numUnits, inputSize}, dataType, qScale, qOffset);
    armnn::TensorInfo tensorInfo4x3({numUnits, outputSize}, dataType, qScale, qOffset);
    armnn::TensorInfo tensorInfo3x4({outputSize, numUnits}, dataType, qScale, qOffset);

    LstmQueueDescriptor data;
    WorkloadInfo        info;

    AddInputToWorkload(data, info, inputTensorInfo, nullptr);
    AddInputToWorkload(data, info, outputStateInTensorInfo, nullptr);
    AddInputToWorkload(data, info, cellStateInTensorInfo, nullptr);

    AddOutputToWorkload(data, info, scratchBufferTensorInfo, nullptr);
    AddOutputToWorkload(data, info, outputStateOutTensorInfo, nullptr);
    AddOutputToWorkload(data, info, cellStateOutTensorInfo, nullptr);
    // AddOutputToWorkload(data, info, outputTensorInfo, nullptr); is left out

    armnn::ScopedCpuTensorHandle inputToInputWeightsTensor(tensorInfo4x5);
    armnn::ScopedCpuTensorHandle inputToForgetWeightsTensor(tensorInfo4x5);
    armnn::ScopedCpuTensorHandle inputToCellWeightsTensor(tensorInfo4x5);
    armnn::ScopedCpuTensorHandle inputToOutputWeightsTensor(tensorInfo4x5);
    armnn::ScopedCpuTensorHandle recurrentToForgetWeightsTensor(tensorInfo4x3);
    armnn::ScopedCpuTensorHandle recurrentToInputWeightsTensor(tensorInfo4x3);
    armnn::ScopedCpuTensorHandle recurrentToCellWeightsTensor(tensorInfo4x3);
    armnn::ScopedCpuTensorHandle recurrentToOutputWeightsTensor(tensorInfo4x3);
    armnn::ScopedCpuTensorHandle cellToInputWeightsTensor(tensorInfo4);
    armnn::ScopedCpuTensorHandle inputGateBiasTensor(tensorInfo4);
    armnn::ScopedCpuTensorHandle forgetGateBiasTensor(tensorInfo4);
    armnn::ScopedCpuTensorHandle cellBiasTensor(tensorInfo4);
    armnn::ScopedCpuTensorHandle outputGateBiasTensor(tensorInfo4);
    armnn::ScopedCpuTensorHandle cellToForgetWeightsTensor(tensorInfo4);
    armnn::ScopedCpuTensorHandle cellToOutputWeightsTensor(tensorInfo4);
    armnn::ScopedCpuTensorHandle projectionWeightsTensor(tensorInfo3x4);
    armnn::ScopedCpuTensorHandle projectionBiasTensor(tensorInfo3);
    armnn::ScopedCpuTensorHandle inputLayerNormWeightsTensor(tensorInfo4);
    armnn::ScopedCpuTensorHandle forgetLayerNormWeightsTensor(tensorInfo4);
    armnn::ScopedCpuTensorHandle cellLayerNormWeightsTensor(tensorInfo4);
    armnn::ScopedCpuTensorHandle outputLayerNormWeightsTensor(tensorInfo4);

    data.m_InputToInputWeights = &inputToInputWeightsTensor;
    data.m_InputToForgetWeights = &inputToForgetWeightsTensor;
    data.m_InputToCellWeights = &inputToCellWeightsTensor;
    data.m_InputToOutputWeights = &inputToOutputWeightsTensor;
    data.m_RecurrentToInputWeights = &recurrentToInputWeightsTensor;
    data.m_RecurrentToForgetWeights = &recurrentToForgetWeightsTensor;
    data.m_RecurrentToCellWeights = &recurrentToCellWeightsTensor;
    data.m_RecurrentToOutputWeights = &recurrentToOutputWeightsTensor;
    data.m_CellToInputWeights = &cellToInputWeightsTensor;
    data.m_InputGateBias = &inputGateBiasTensor;
    data.m_ForgetGateBias = &forgetGateBiasTensor;
    data.m_CellBias = &cellBiasTensor;
    data.m_OutputGateBias = &outputGateBiasTensor;
    data.m_CellToForgetWeights = &cellToForgetWeightsTensor;
    data.m_CellToOutputWeights = &cellToOutputWeightsTensor;
    data.m_ProjectionWeights = &projectionWeightsTensor;
    data.m_ProjectionBias = &projectionBiasTensor;

    data.m_InputLayerNormWeights = &inputLayerNormWeightsTensor;
    data.m_ForgetLayerNormWeights = &forgetLayerNormWeightsTensor;
    data.m_CellLayerNormWeights = &cellLayerNormWeightsTensor;
    data.m_OutputLayerNormWeights = &outputLayerNormWeightsTensor;

    // Flags to set test configuration
    data.m_Parameters.m_ActivationFunc = 4;
    data.m_Parameters.m_CifgEnabled = false;
    data.m_Parameters.m_PeepholeEnabled = true;
    data.m_Parameters.m_ProjectionEnabled = true;
    data.m_Parameters.m_LayerNormEnabled = true;

    // check wrong number of outputs
    BOOST_CHECK_THROW(data.Validate(info), armnn::InvalidArgumentException);
    AddOutputToWorkload(data, info, outputTensorInfo, nullptr);

    // check wrong cifg parameter configuration
    data.m_Parameters.m_CifgEnabled = true;
    armnn::TensorInfo scratchBufferTensorInfo2({batchSize, numUnits * 3}, dataType, qScale, qOffset);
    SetWorkloadOutput(data, info, 0, scratchBufferTensorInfo2, nullptr);
    BOOST_CHECK_THROW(data.Validate(info), armnn::InvalidArgumentException);
    data.m_Parameters.m_CifgEnabled = false;
    SetWorkloadOutput(data, info, 0, scratchBufferTensorInfo, nullptr);

    // check wrong inputGateBias configuration
    data.m_InputGateBias = nullptr;
    BOOST_CHECK_THROW(data.Validate(info), armnn::InvalidArgumentException);
    data.m_InputGateBias = &inputGateBiasTensor;

    // check inconsistant projection parameters
    data.m_Parameters.m_ProjectionEnabled = false;
    BOOST_CHECK_THROW(data.Validate(info), armnn::InvalidArgumentException);
    data.m_Parameters.m_ProjectionEnabled = true;
    data.m_ProjectionWeights = nullptr;
    BOOST_CHECK_THROW(data.Validate(info), armnn::InvalidArgumentException);
    data.m_ProjectionWeights = &projectionWeightsTensor;

    // check missing input layer normalisation weights
    data.m_InputLayerNormWeights = nullptr;
    BOOST_CHECK_THROW(data.Validate(info), armnn::InvalidArgumentException);
    data.m_InputLayerNormWeights = &inputLayerNormWeightsTensor;

    // layer norm disabled but normalisation weights are present
    data.m_Parameters.m_LayerNormEnabled = false;
    BOOST_CHECK_THROW(data.Validate(info), armnn::InvalidArgumentException);
    data.m_Parameters.m_LayerNormEnabled = true;

    // check invalid outputTensor shape
    armnn::TensorInfo incorrectOutputTensorInfo({batchSize, outputSize + 1}, dataType, qScale, qOffset);
    SetWorkloadOutput(data, info, 3, incorrectOutputTensorInfo, nullptr);
    BOOST_CHECK_THROW(data.Validate(info), armnn::InvalidArgumentException);
    SetWorkloadOutput(data, info, 3, outputTensorInfo, nullptr);

    // check correct configuration
    BOOST_CHECK_NO_THROW(data.Validate(info));
}

BOOST_AUTO_TEST_SUITE_END()
