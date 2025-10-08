#include <cmath>
#include <algorithm>

// Helper function for lfilter (simple FIR filter)
void lfilter(const float* kernel, int kernelLen, float* signal, int signalLen) {
    float temp[signalLen];
    for (int i = 0; i < signalLen; ++i) temp[i] = signal[i];
    for (int i = 0; i < signalLen; ++i) {
        signal[i] = 0.0f;
        for (int k = 0; k < kernelLen; ++k) {
            if (i - k >= 0)
                signal[i] += kernel[k] * temp[i - k];
        }
    }
}

// Main function
void f120ElectrodogramFunc(
    int nChan, int nEl, int nFrameFt, int pulseWidth,
    const int* chanOrder, bool cathodicFirst, float fsOut,
    const float* ampIn, // [2*nChan][nFrameFt]
    float* elGram       // [nEl][nFrameOut]
) {
    int phasesPerCyc = 2 * nChan;
    float dtIn = phasesPerCyc * pulseWidth * 1e-6f;
    float durIn = nFrameFt * dtIn;
    int nFrameOut = nFrameFt * phasesPerCyc;

    // Zero output
    for (int i = 0; i < nEl * nFrameOut; ++i) elGram[i] = 0.0f;

    // Fill elGram
    for (int iCh = 0; iCh < nChan; ++iCh) {
        int phaseOffset = 2 * (chanOrder[iCh] - 1);
        for (int f = 0; f < nFrameFt; ++f) {
            int outIdx = phaseOffset + f * phasesPerCyc;
            elGram[iCh * nFrameOut + outIdx] = ampIn[2 * iCh * nFrameFt + f];
            elGram[(iCh + 1) * nFrameOut + outIdx] = ampIn[(2 * iCh + 1) * nFrameFt + f];
        }
    }

    // Apply kernel filter
    float kernel[2];
    kernel[0] = cathodicFirst ? -1.0f : 1.0f;
    kernel[1] = cathodicFirst ? 1.0f : -1.0f;
    for (int el = 0; el < nEl; ++el) {
        lfilter(kernel, 2, &elGram[el * nFrameOut], nFrameOut);
    }

    // Optional: Interpolation for fsOut (not implemented here, as HLS prefers fixed rates)
    // If needed, implement nearest-neighbor or previous-value interpolation.

    // Output: elGram is filled
}
