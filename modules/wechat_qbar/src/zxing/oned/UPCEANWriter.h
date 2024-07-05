/*
 * Copyright 2009 ZXing authors
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


/**
 * <p>Encapsulates functionality and implementation that is common to UPC and EAN families
 * of one-dimensional barcodes.</p>
 *
 * @author aripollak@gmail.com (Ari Pollak)
 * @author dsbnatut@gmail.com (Kazuki Nishiura)
 */

#ifndef __UPCEAN_WRITER_H__
#define __UPCEAN_WRITER_H__

#include <zxing/oned/OneDimensionalCodeWriter.h>
#include <zxing/oned/OneDConstant.h>

using namespace zxing;
using namespace oned;

using namespace zxing::oned::constant::UPCEAN;

namespace zxing {
namespace oned {

class UPCEANWriter : public OneDimensionalCodeWriter {
public:
    int getDefaultMargin();
};

}
}

#endif
