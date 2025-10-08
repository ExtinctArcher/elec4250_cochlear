#include <cmath>
#include <algorithm>

void f120MappingFunc(
    const float* mapA, const float* mapK, const float* mapClip,
    const int* chanToEl, int carrierMode,
    int nChan, int nFtFrames,
    const float* carrier, // [nChan][nFtFrames]
    const float* env,     // [nChan][nFtFrames]
    const float* weights, // [2*nChan][nFtFrames]
    const int* idxAudioFrame, // [nFtFrames]
    float* ampWords       // [30][nFtFrames]
) {
    for (int iChan = 0; iChan < nChan; ++iChan) {
        int iElLo = chanToEl[iChan];
        int iElHi = iElLo + 1;
        int iAmpLo = iElLo * 2;
        int iAmpHi = iAmpLo + 1;
        for (int f = 0; f < nFtFrames; ++f) {
            int idx = idxAudioFrame[f];
            float envVal = env[iChan * nFtFrames + idx];
            float carrierVal = carrier[iChan * nFtFrames + f];
            float mappedLo, mappedHi;
            if (carrierMode == 0) {
                mappedLo = mapA[iElLo] * envVal + mapK[iElLo];
                mappedHi = mapA[iElHi] * envVal + mapK[iElHi];
            } else if (carrierMode == 1) {
                mappedLo = mapA[iElLo] * envVal * carrierVal + mapK[iElLo];
                mappedHi = mapA[iElHi] * envVal * carrierVal + mapK[iElHi];
            } else if (carrierMode == 2) {
                mappedLo = (mapA[iElLo] * envVal + mapK[iElLo]) * carrierVal;
                mappedHi = (mapA[iElHi] * envVal + mapK[iElHi]) * carrierVal;
            }
            mappedLo = std::max(std::min(mappedLo, mapClip[iElLo]), 0.0f);
            mappedHi = std::max(std::min(mappedHi, mapClip[iElHi]), 0.0f);
            ampWords[iAmpLo * nFtFrames + f] = mappedLo * weights[iChan * nFtFrames + idx];
            ampWords[iAmpHi * nFtFrames + f] = mappedHi * weights[(iChan + nChan) * nFtFrames + idx];
        }
    }
}