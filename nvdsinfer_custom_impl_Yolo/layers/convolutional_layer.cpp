/*
 * Created by Marcos Luciano
 * https://www.github.com/marcoslucianops
 */

#include <math.h>
#include "convolutional_layer.h"

nvinfer1::ILayer* convolutionalLayer(
    int layerIdx,
    std::map<std::string, std::string>& block,
    std::vector<float>& weights,
    std::vector<nvinfer1::Weights>& trtWeights,
    int& weightPtr,
    std::string weightsType,
    int& inputChannels,
    float eps,
    nvinfer1::ITensor* input,
    nvinfer1::INetworkDefinition* network)
{
    assert(block.at("type") == "convolutional");
    assert(block.find("filters") != block.end());
    assert(block.find("pad") != block.end());
    assert(block.find("size") != block.end());
    assert(block.find("stride") != block.end());

    int filters = std::stoi(block.at("filters"));
    int padding = std::stoi(block.at("pad"));
    int kernelSize = std::stoi(block.at("size"));
    int stride = std::stoi(block.at("stride"));
    std::string activation = block.at("activation");
    int bias = filters;

    bool batchNormalize = false;
    if (block.find("batch_normalize") != block.end())
    {
        bias = 0;
        batchNormalize = (block.at("batch_normalize") == "1");
    }

    int groups = 1;
    if (block.find("groups") != block.end())
    {
        groups = std::stoi(block.at("groups"));
    }

    int pad;
    if (padding)
        pad = (kernelSize - 1) / 2;
    else
        pad = 0;

    int size = filters * inputChannels * kernelSize * kernelSize / groups;
    std::vector<float> bnBiases;
    std::vector<float> bnWeights;
    std::vector<float> bnRunningMean;
    std::vector<float> bnRunningVar;
    nvinfer1::Weights convWt{nvinfer1::DataType::kFLOAT, nullptr, size};
    nvinfer1::Weights convBias{nvinfer1::DataType::kFLOAT, nullptr, bias};

    if (weightsType == "weights") {
        if (batchNormalize == false)
        {
            float* val = new float[filters];
            for (int i = 0; i < filters; ++i)
            {
                val[i] = weights[weightPtr];
                weightPtr++;
            }
            convBias.values = val;
            trtWeights.push_back(convBias);
            val = new float[size];
            for (int i = 0; i < size; ++i)
            {
                val[i] = weights[weightPtr];
                weightPtr++;
            }
            convWt.values = val;
            trtWeights.push_back(convWt);
        }
        else
        {
            for (int i = 0; i < filters; ++i)
            {
                bnBiases.push_back(weights[weightPtr]);
                weightPtr++;
            }
            for (int i = 0; i < filters; ++i)
            {
                bnWeights.push_back(weights[weightPtr]);
                weightPtr++;
            }
            for (int i = 0; i < filters; ++i)
            {
                bnRunningMean.push_back(weights[weightPtr]);
                weightPtr++;
            }
            for (int i = 0; i < filters; ++i)
            {
                bnRunningVar.push_back(sqrt(weights[weightPtr] + 1.0e-5));
                weightPtr++;
            }
            float* val = new float[size];
            for (int i = 0; i < size; ++i)
            {
                val[i] = weights[weightPtr];
                weightPtr++;
            }
            convWt.values = val;
            trtWeights.push_back(convWt);
            trtWeights.push_back(convBias);
        }
    }
    else {
        if (batchNormalize == false)
        {
            float* val = new float[size];
            for (int i = 0; i < size; ++i)
            {
                val[i] = weights[weightPtr];
                weightPtr++;
            }
            convWt.values = val;
            trtWeights.push_back(convWt);
            val = new float[filters];
            for (int i = 0; i < filters; ++i)
            {
                val[i] = weights[weightPtr];
                weightPtr++;
            }
            convBias.values = val;
            trtWeights.push_back(convBias);
        }
        else
        {
            float* val = new float[size];
            for (int i = 0; i < size; ++i)
            {
                val[i] = weights[weightPtr];
                weightPtr++;
            }
            convWt.values = val;
            for (int i = 0; i < filters; ++i)
            {
                bnWeights.push_back(weights[weightPtr]);
                weightPtr++;
            }
            for (int i = 0; i < filters; ++i)
            {
                bnBiases.push_back(weights[weightPtr]);
                weightPtr++;
            }
            for (int i = 0; i < filters; ++i)
            {
                bnRunningMean.push_back(weights[weightPtr]);
                weightPtr++;
            }
            for (int i = 0; i < filters; ++i)
            {
                bnRunningVar.push_back(sqrt(weights[weightPtr] + eps));
                weightPtr++;
            }
            trtWeights.push_back(convWt);
            trtWeights.push_back(convBias);
        }
    }

    nvinfer1::IConvolutionLayer* conv = network->addConvolutionNd(
        *input, filters, nvinfer1::DimsHW{kernelSize, kernelSize}, convWt, convBias);
    assert(conv != nullptr);
    std::string convLayerName = "conv_" + std::to_string(layerIdx);
    conv->setName(convLayerName.c_str());
    conv->setStrideNd(nvinfer1::DimsHW{stride, stride});
    conv->setPaddingNd(nvinfer1::DimsHW{pad, pad});

    if (block.find("groups") != block.end())
    {
        conv->setNbGroups(groups);
    }

    nvinfer1::ILayer* output = conv;

    if (batchNormalize == true)
    {
        size = filters;
        nvinfer1::Weights shift{nvinfer1::DataType::kFLOAT, nullptr, size};
        nvinfer1::Weights scale{nvinfer1::DataType::kFLOAT, nullptr, size};
        nvinfer1::Weights power{nvinfer1::DataType::kFLOAT, nullptr, size};
        float* shiftWt = new float[size];
        for (int i = 0; i < size; ++i)
        {
            shiftWt[i]
                = bnBiases.at(i) - ((bnRunningMean.at(i) * bnWeights.at(i)) / bnRunningVar.at(i));
        }
        shift.values = shiftWt;
        float* scaleWt = new float[size];
        for (int i = 0; i < size; ++i)
        {
            scaleWt[i] = bnWeights.at(i) / bnRunningVar[i];
        }
        scale.values = scaleWt;
        float* powerWt = new float[size];
        for (int i = 0; i < size; ++i)
        {
            powerWt[i] = 1.0;
        }
        power.values = powerWt;
        trtWeights.push_back(shift);
        trtWeights.push_back(scale);
        trtWeights.push_back(power);

        nvinfer1::IScaleLayer* bn = network->addScale(
            *output->getOutput(0), nvinfer1::ScaleMode::kCHANNEL, shift, scale, power);
        assert(bn != nullptr);
        std::string bnLayerName = "batch_norm_" + std::to_string(layerIdx);
        bn->setName(bnLayerName.c_str());
        output = bn;
    }

    output = activationLayer(layerIdx, activation, output, output->getOutput(0), network);
    assert(output != nullptr);

    return output;
}
