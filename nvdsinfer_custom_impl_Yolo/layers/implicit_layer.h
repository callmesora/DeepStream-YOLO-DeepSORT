/*
 * Created by Marcos Luciano
 * https://www.github.com/marcoslucianops
 */

#ifndef __IMPLICIT_LAYER_H__
#define __IMPLICIT_LAYER_H__

#include <map>
#include <vector>
#include <cassert>

#include "NvInfer.h"

nvinfer1::ILayer* implicitLayer(
    int channels,
    std::vector<float>& weights,
    std::vector<nvinfer1::Weights>& trtWeights,
    int& weightPtr,
    nvinfer1::INetworkDefinition* network);

#endif
