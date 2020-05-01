#include "pch.h"
#include "utils_io.h"
#include <cerrno>

namespace _3fd {
namespace utils {

    /// <summary>Constructs a new object by parsing the text and finding the placeholders.</summary>
    /// <param name="placeholderMarker">
    /// The character that marks the start of a placeholder.
    /// Alphanumeric ASCII characters and '_' are the only allowed for a placeholder,
    /// while being forbidden for a marker.
    /// </param>
    /// <param name="text">The text where the placeholders must be replaced.</param>
    TextPlaceholderReplacementHelper::TextPlaceholderReplacementHelper(char placeholderMarker,
                                                                       std::string_view text)
        : m_referenceText(text)
        , m_placeholderMarker(placeholderMarker)
    {
        size_t offset(0);
        do
        {
            size_t tokenPos = text.find(placeholderMarker, offset);
            if (tokenPos == std::string::npos)
                tokenPos = text.size();

            // store text piece before token:
            if (tokenPos > offset)
                m_pieces.push_back(std::string_view(text.data() + offset, tokenPos - offset));

            // parse placeholder:
            ptrdiff_t placeholderLength(0);
            auto placeholderIterBegin = text.begin() + tokenPos;
            if (text.end() != placeholderIterBegin)
            {
                placeholderLength =
                    std::find_if_not(placeholderIterBegin + 1, text.end(),
                                     [](char ch) { return isalnum(ch) != 0 || ch == '_'; })
                    - placeholderIterBegin;

                // store placeholder piece (with marker)
                m_pieces.push_back(std::string_view(text.data() + tokenPos, placeholderLength));
            }

            offset = tokenPos + placeholderLength;

        } while (offset < text.size());
    }

    /// <summary>Carry out replacements and emit string.</summary>
    /// <returns>String of reference text but with placeholders replaced.</returns>
    std::string TextPlaceholderReplacementHelper::Emit() const
    {
        std::ostringstream oss;

        for (auto &piece : m_pieces)
        {
            _ASSERTE(!piece.empty());
            if (piece[0] != m_placeholderMarker)
            {
                oss << piece;
            }
            else
            {
                // find placeholder (without marker) among replacements
                auto iter = m_replacements.find(std::string_view(piece.data() + 1, piece.size() - 1));
                if (m_replacements.end() != iter)
                    oss << iter->second;
            }
        }

        return oss.str();
    }

    ///////////////////////////////////////
    // Specialization of serializations
    ///////////////////////////////////////

    size_t _estimate_string_size()
    {
        return 0;
    }

#ifdef _3FD_PLATFORM_WINRT
    
    SerializableValue<const wchar_t *> FormatArg(const winrt::hstring &value)
    {
        return SerializableValue<const wchar_t *>(value.c_str());
    }
#endif

}// end of namespace utils
}// end of namespace _3fd