/*
 * Created by Marcos Luciano
 * https://www.github.com/marcoslucianops
 */

#ifndef __CONVOLUTIONAL_LAYER_H__
#define __CONVOLUTIONAL_LAYER_H__

#include <map>
#include <vector>

#include "NvInfer.h"

#include "activation_layer.h"

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
    nvinfer1::INetworkDefinition* network);

#endif
