// -*- mode:c++; tab-width:2; indent-tabs-mode:nil; c-basic-offset:2 -*-
/*
 *  Copyright 2013 ZXing authors All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http:// www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <zxing/qrcode/encoder/ReedSolomonEncoder.hpp>
#include <zxing/common/reedsolomon/ReedSolomonException.hpp>
#include <zxing/common/IllegalArgumentException.hpp>
#include <zxing/IllegalStateException.hpp>

namespace zxing {

ReedSolomonEncoder::ReedSolomonEncoder(Ref<GenericGF> field) :
field_(field) {
    ArrayRef<int> coefficients(new Array<int>(1));
    coefficients[0] = 1;
    ErrorHandler err_handler;
    Ref<GenericGFPoly> one(new GenericGFPoly(*field, coefficients, err_handler));
    if (err_handler.ErrCode()) throw IllegalArgumentException("GenericGFPoly init error");;
    cachedGenerators_.push_back(one);
}

ReedSolomonEncoder::~ReedSolomonEncoder() {
}

void ReedSolomonEncoder::encode(ArrayRef<int> toEncode, int ecBytes) {
    if (ecBytes == 0)
    {
        throw IllegalArgumentException("No error correction bytes");
    }
    int dataBytes = toEncode->size() - ecBytes;
    if (dataBytes <= 0)
    {
        throw IllegalArgumentException("No data bytes provided");
    }
    Ref<GenericGFPoly> generator = _buildGenerator(ecBytes);
    ArrayRef<int> infoCoefficients(new Array<int>(dataBytes));
    for (int i = 0; i < dataBytes; ++i) {
        infoCoefficients[i] = toEncode[i];
    }
    ErrorHandler err_handler;
    Ref<GenericGFPoly> info(new GenericGFPoly(*field_, infoCoefficients, err_handler));
    if (err_handler.ErrCode()) throw IllegalArgumentException("GenericGFPoly error");
    info = info->multiplyByMonomial(ecBytes, 1, err_handler);
    if (err_handler.ErrCode()) throw IllegalArgumentException("multiplyByMonomial error");
    // Ref<GenericGFPoly> remainder = info->divide(generator)[1];
    std::vector<Ref<GenericGFPoly> > vec_remainder = info->divide(generator, err_handler);
    if (err_handler.ErrCode()) throw IllegalArgumentException("divide error");
    Ref<GenericGFPoly> remainder = vec_remainder[1];
    ArrayRef<int> coefficients = remainder->getCoefficients();
    int numZeroCoefficients = ecBytes - coefficients->size();
    for (int i = 0; i < numZeroCoefficients; i++) {
        toEncode[dataBytes + i] = 0;
    }
    for (int i = 0; i < coefficients->size(); ++i) {
        toEncode[dataBytes + numZeroCoefficients + i] = coefficients[i];
    }
}

Ref<GenericGFPoly> ReedSolomonEncoder::_buildGenerator(int degree) {
    if (degree >= static_cast<int>(cachedGenerators_.size()))
    {
        Ref<GenericGFPoly> lastGenerator = cachedGenerators_[cachedGenerators_.size() - 1];
        ErrorHandler err_handler;
        for (int d = cachedGenerators_.size(); d <= degree; ++d) {
            ArrayRef<int> coefficients(new Array<int>(2));
            coefficients[0] = 1;
            coefficients[1] = field_->exp(d - 1 + field_->getGeneratorBase());
            Ref<GenericGFPoly> poly(new GenericGFPoly(*field_, coefficients, err_handler));
            if (err_handler.ErrCode()) throw IllegalArgumentException("init GenericGFPoly error");
            
            Ref<GenericGFPoly> nextGenerator = lastGenerator->multiply(poly, err_handler);
            if (err_handler.ErrCode()) throw IllegalArgumentException("multiply error");
            cachedGenerators_.push_back(nextGenerator);
            lastGenerator = nextGenerator;
        }
    }
    return cachedGenerators_[degree];
}
}  // namespace zxing
