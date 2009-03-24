/*
 * Copyright (C) 2008 Pleyo.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 * 3.  Neither the name of Pleyo nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY PLEYO AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL PLEYO OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef TextCodecUTF16_h
#define TextCodecUTF16_h
/**
 *  @file  TextCodecUTF16.t
 *  TextCodecUTF16 description
 *  Repository informations :
 * - $URL$
 * - $Rev$
 * - $Date$
 */
#include "BALBase.h"

#include "TextCodec.h"

namespace WebCore {

    class TextCodecUTF16 : public WebCoreBase, public TextCodec {
    public:
    /**
     * registerEncodingNames
     * @param[in] : EncodingNameRegistrar
     * @code
     * TextCodecUTF16::registerEncodingNames(r);
     * @endcode
     */
        static void registerEncodingNames(EncodingNameRegistrar);

    /**
     * registerCodecs
     * @param[in] : TextCodecRegistrar
     * @code
     * TextCodecUTF16::registerCodecs(r);
     * @endcode
     */
        static void registerCodecs(TextCodecRegistrar);

    /**
     * TextCodecUTF16 constructor
     * @param[in] : little endian status
     */
        TextCodecUTF16(bool littleEndian);

    /**
     * @see TextCodec
     */
        virtual String decode(const char*, size_t length, bool flush, bool stopOnError, bool& sawError);
    /**
     * @see TextCodec
     */
        virtual CString encode(const UChar*, size_t length, UnencodableHandling);

    private:
        bool m_littleEndian;
        bool m_haveBufferedByte;
        unsigned char m_bufferedByte;
    };

} // namespace WebCore

#endif // TextCodecUTF16_h




