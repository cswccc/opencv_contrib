/// -*- mode:c++; tab-width:2; indent-tabs-mode:nil; c-basic-offset:2 -*-
/*
 *  IntMatrix.cpp
 *  zxing
 *
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

#include <zxing/qrcode/encoder/IntMatrix.hpp>

using zxing::qrcode::IntMatrix;

IntMatrix::IntMatrix(int width, int height) :
width_(width),
height_(height) {
    data_.resize(width_ * height_);
}

IntMatrix::~IntMatrix() {
}

int IntMatrix::get(int x, int y) const {
    return data_[x + y * width_];
}

void IntMatrix::set(int x, int y, int value) {
    data_[x + y * width_] = value;
}

void IntMatrix::clear() {
    data_.clear();
    data_.resize(width_ * height_);
}

int IntMatrix::getWidth() const {
    return width_;
}

int IntMatrix::getHeight() const {
    return height_;
}
