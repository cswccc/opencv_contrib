// -*- mode:c++; tab-width:2; indent-tabs-mode:nil; c-basic-offset:2 -*-
#ifndef __INT_MATRIX_H__
#define __INT_MATRIX_H__

/*
 *  IntMatrix.hpp
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

#include <zxing/ZXing.hpp>
#include <zxing/common/Counted.hpp>
#include <vector>

namespace zxing {
namespace qrcode {

class IntMatrix : public Counted {
public:
  IntMatrix(int width, int height);
  ~IntMatrix();

  int get(int x, int y) const;
  void set(int x, int y, int value);
  void clear();
  int getWidth() const;
  int getHeight() const;

private:
  int width_;
  int height_;
  std::vector<int> data_;
};

}  // namespace qrcode
}  // namespace zxing

#endif  // __INT_MATRIX_H__
