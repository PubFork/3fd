#include "stdafx.h"
#include "cmdline.h"
#include <algorithm>
#include <iostream>

namespace _3fd
{
namespace core
{
    static bool isNullOrEmpty(const char *str)
    {
        return str == nullptr || str[0] == 0;
    }

    // Looks for inconsistencies that are common to declarations of all types of arguments
    static void LookForCommonInconsistencies(const CommandLineArguments::ArgDeclaration &argDecl,
                                             bool lastPositionIsTaken,
                                             const char *stdExMsg)
    {
        CALL_STACK_TRACE;

        // Argument is an option, but no valid label (single char or long name) is specified:
        if (static_cast<uint8_t> (argDecl.type) & CommandLineArguments::argIsOptionFlag != 0
            && isNullOrEmpty(argDecl.optName)
            && !isalnum(argDecl.optChar))
        {
            std::ostringstream oss;
            oss << "Argument ID " << argDecl.id
                << ": an option requires a valid label (single character and/or name)";

            throw AppException<std::invalid_argument>(stdExMsg, oss.str());
        }

        if (static_cast<uint8_t> (argDecl.type) & CommandLineArguments::argIsOptionFlag == 0)
        {
            // Single char label was specified, bur argument is a value:
            if (argDecl.optChar != 0)
            {
                std::ostringstream oss;
                oss << "Argument ID " << argDecl.id << ": value cannot have a single character label";
                throw AppException<std::invalid_argument>(stdExMsg, oss.str());
            }

            // Argument is a value, but a name label was not specified:
            if (isNullOrEmpty(argDecl.optName))
            {
                std::ostringstream oss;
                oss << "Argument ID " << argDecl.id << ": value must have a name label";
                throw AppException<std::invalid_argument>(stdExMsg, oss.str());
            }
        }

        // Argument has empty description:
        if (isNullOrEmpty(argDecl.description))
        {
            std::ostringstream oss;
            oss << "Argument ID " << argDecl.id << ": description cannot be empty";
            throw AppException<std::invalid_argument>(stdExMsg, oss.str());
        }

        // Argument is to be placed at last, but such position is already taken:
        if (argDecl.placement == CommandLineArguments::ArgPlacement::MustComeLast && lastPositionIsTaken)
        {
            std::ostringstream oss;
            oss << "Argument ID " << argDecl.id
                << ": cannot be placed at last because such position is "
                   "already taken by a previously declared argument";

            throw AppException<std::invalid_argument>(stdExMsg, oss.str());
        }
    }

    /// <summary>
    /// Validates the argument labels against previously added ones, issuing
    /// an error whenever collision occurs. The labels themselves are also
    /// verified for forbidden characters or whether too big, as well as
    /// the description.
    /// </summary>
    /// <param name="argDecl">The argument declaration.</param>
    /// <param name="stdExMsg">The standard message for thrown exceptions.</param>
    void CommandLineArguments::ValidateArgDescAndLabels(const CommandLineArguments::ArgDeclaration &argDecl,
                                                        const char *stdExMsg)
    {
        CALL_STACK_TRACE;

        const uint32_t maxLengthArgDesc(2000);

        // is description too large?
        if (strlen(argDecl.description) > maxLengthArgDesc)
        {
            std::ostringstream oss;
            oss << "Argument ID " << argDecl.id
                << ": description is too large (limit is "
                << maxLengthArgDesc << " UTF-8 characters)";

            throw AppException<std::invalid_argument>(stdExMsg, oss.str());
        }

        // single char label specified?
        if (argDecl.optChar != 0)
        {
            auto iter = m_argsByCharLabel.find(argDecl.optChar);

            // collides with another single char label?
            if (m_argsByCharLabel.end() == iter)
                m_argsByCharLabel[argDecl.optChar] = argDecl.id;
            else
            {
                std::ostringstream oss;
                oss << "Argument ID " << argDecl.id
                    << ": single character label '" << argDecl.optChar
                    << "' is already in use by argument ID " << iter->second;

                throw AppException<std::invalid_argument>(stdExMsg, oss.str());
            }
        }

        // name label not specified?
        if (isNullOrEmpty(argDecl.optName))
            return;

        auto iter = m_argsByNameLabel.find(argDecl.optName);

        // colides with another name label?
        if (m_argsByNameLabel.end() == iter)
            m_argsByNameLabel[argDecl.optName] = argDecl.id;
        else
        {
            std::ostringstream oss;
            oss << "Argument ID " << argDecl.id
                << ": name label '" << argDecl.optName
                << "' is already in use by argument ID " << iter->second;

            throw AppException<std::invalid_argument>(stdExMsg, oss.str());
        }

        /* Windows notation in command line normally uses a slash to mark options,
           and adopts an underline when composing names like 'no_warnings', which
           otherwise would require undesired camel notation for better readability.
           POSIX notation, in the other hand, chooses a dash for the same purpose,
           like in '--no-warnings'. Those practices are enforced as rules here, so
           the application is made compliant with these standards. */
        const char dash = (m_useOptSignSlash) ? '_' : '-';

        char ch;
        uint32_t nameCharCount(0);
        
        // Compute size of name label while checking for disallowed chars:
        while ((ch = *(argDecl.optName + nameCharCount)) != 0)
        {
            // alphanumeric ASCII chars are allowed
            if (isalnum(ch) || ch == dash)
                ++nameCharCount;
            // spaces are allowed when the argument is a list of values:
            else if (iswspace(ch))
            {
                if (argDecl.type == ArgType::ValuesList)
                    ++nameCharCount;
                else
                {
                    std::ostringstream oss;
                    oss << "Argument ID " << argDecl.id
                        << ": white spaces in name label are allowed only for arguments that are a list of values";

                    throw AppException<std::invalid_argument>(stdExMsg, oss.str());
                }
            }
            else // disallowed character:
            {
                std::ostringstream oss;
                oss << "Argument ID " << argDecl.id
                    << ": only alphanumeric ASCII characters (and dash for POSIX "
                       "option notation which also uses dash, or underline for Windows "
                       "option notation which uses slash)  are allowed in name label";

                throw AppException<std::invalid_argument>(stdExMsg, oss.str());
            }
        }

        const uint32_t maxLengthNameLabel(24);

        // is label too big?
        if (nameCharCount > maxLengthNameLabel)
        {
            std::ostringstream oss;
            oss << "Argument ID " << argDecl.id
                << ": name label is too large (limit is "
                << maxLengthNameLabel << " UTF-8 characters)";

            throw AppException<std::invalid_argument>(stdExMsg, oss.str());
        }

        /* Keep the largest name label. Later this will be used to plan
           the layout of a pretty table of arguments when printing the
           usage of command line. */
        if (nameCharCount > m_largestNameLabel)
            m_largestNameLabel = nameCharCount;
    }

    /// <summary>
    /// Adds the declaration of an expected command line argument which is
    /// either a switch-type option or an option with required accompanying
    /// value (not limited to a range or set).
    /// </summary>
    /// <param name="argDecl">The argument declaration.</param>
    void CommandLineArguments::AddExpectedArgument(const CommandLineArguments::ArgDeclaration &argDecl)
    {
        CALL_STACK_TRACE;

        const char *stdExMsg("Failed to add declaration of expected command line argument");

        // Argument is switch-type option, but specifies type of accompanying value:
        if (argDecl.type == ArgType::OptionSwitch && argDecl.valueType != ArgValType::None)
        {
            std::ostringstream oss;
            oss << "Argument ID " << argDecl.id
                << ": cannot specify a type for accompanying value when argument is switch-type option";

            throw AppException<std::invalid_argument>(stdExMsg, oss.str());
        }

        // Argument is an option with non-required accompanying value, but this function overload does not handle this case:
        if (argDecl.type == ArgType::OptionWithNonReqValue)
        {
            std::ostringstream oss;
            oss << "Argument ID " << argDecl.id
                << ": option with non-required accompanying value must specify default value";

            throw AppException<std::invalid_argument>(stdExMsg, oss.str());
        }

        // Argument is value limited to a range, but this function overload does not handle this case:
        if (static_cast<uint8_t> (argDecl.type) & argIsValueFlag != 0
            && static_cast<uint8_t> (argDecl.valueType) & argValIsRangedTypeFlag != 0)
        {
            std::ostringstream oss;
            oss << "Argument ID " << argDecl.id
                << ": value limited to a range requires specification of boundaries";

            throw AppException<std::invalid_argument>(stdExMsg, oss.str());
        }

        // Argument is value limited to enumeration, but this function overload does not handle this case:
        if (static_cast<uint8_t> (argDecl.type) & argIsValueFlag != 0
            && static_cast<uint8_t> (argDecl.valueType) & argValIsEnumTypeFlag != 0)
        {
            std::ostringstream oss;
            oss << "Argument ID " << argDecl.id
                << ": value limited to enumeration requires specification of allowed value";

            throw AppException<std::invalid_argument>(stdExMsg, oss.str());
        }

        LookForCommonInconsistencies(argDecl, m_lastPositionIsTaken, stdExMsg);

        ValidateArgDescAndLabels(argDecl, stdExMsg);

        if (m_expectedArgs.find(argDecl.id) == m_expectedArgs.end())
            m_expectedArgs[argDecl.id] = ArgDeclExtended { argDecl, nullptr };
        else
        {// Collision of argument codes (ID's):
            std::ostringstream oss;
            oss << "Argument ID " << argDecl.id << ": collision of ID";
            throw AppException<std::invalid_argument>(stdExMsg, oss.str());
        }
    }

    // Helps verify consistency of configuration for argument value(s)
    template <typename ValType>
    static void VerifyArgTypedValConfig(
        const CommandLineArguments::ArgDeclaration &argDecl,
        const std::initializer_list<ValType> &argValCfg,
        const char *stdExMsg)
    {
        // Argument is switch-type option, but this function overload does not handle this case:
        if (argDecl.type == CommandLineArguments::ArgType::OptionSwitch)
        {
            std::ostringstream oss;
            oss << "Argument ID " << argDecl.id
                << ": configuration of values does not make sense for switch-type option";

            throw AppException<std::invalid_argument>(stdExMsg, oss.str());
        }

        // Argument value does not use default, range or enumeration:
        if (argDecl.type != CommandLineArguments::ArgType::OptionWithNonReqValue
            && static_cast<uint8_t> (argDecl.valueType) & CommandLineArguments::argValIsRangedTypeFlag == 0
            && static_cast<uint8_t> (argDecl.valueType) & CommandLineArguments::argValIsEnumTypeFlag == 0)
        {
            std::ostringstream oss;
            oss << "Argument ID " << argDecl.id
                << ": configuration of values does not make sense unless default value, "
                   "range boundaries or set of allowed values needs to be specified";

            throw AppException<std::invalid_argument>(stdExMsg, oss.str());
        }

        unsigned int minCountValConfigItems(0);

        if (argDecl.type == CommandLineArguments::ArgType::OptionWithNonReqValue)
            ++minCountValConfigItems;

        if (static_cast<uint8_t> (argDecl.valueType) & CommandLineArguments::argValIsEnumTypeFlag != 0)
            ++minCountValConfigItems;
        else if (static_cast<uint8_t> (argDecl.valueType) & CommandLineArguments::argValIsRangedTypeFlag != 0)
            minCountValConfigItems += 2;

        // Argument is supposed to receive configuration of values, but that is incomplete:
        if (argValCfg.size() < minCountValConfigItems)
        {
            std::ostringstream oss;
            oss << "Argument ID " << argDecl.id
                << ": configuration of values has too few items (default, range boundaries, allowed values)";

            throw AppException<std::invalid_argument>(stdExMsg, oss.str());
        }
    }

    /// <summary>
    /// Adds the declaration of an expected command line argument which has
    /// an accompanying value of integer type that requires specification of
    /// default value, range or allowed set of values.
    /// </summary>
    /// <param name="argDecl">The argument declaration.</param>
    /// <param name="argValCfg">A list of configured values for the accompanying value.
    /// When such value is not required, the first item is the default value.
    /// When such value is limited to a range, the following 2 items are the min & max.
    /// When such value is limited to a set, the following N (> 1) items are enumerated.</param>
    void CommandLineArguments::AddExpectedArgument(
        const CommandLineArguments::ArgDeclaration &argDecl,
        std::initializer_list<long long> &&argValCfg)
    {
        CALL_STACK_TRACE;

        const char *stdExMsg("Failed to add declaration of expected command line argument");

        // Argument declaration conflicts with value configuration:
        if (static_cast<uint8_t> (argDecl.valueType) & static_cast<uint8_t> (ArgValType::Integer) == 0)
        {
            std::ostringstream oss;
            oss << "Argument ID " << argDecl.id
                << ": configuration of integer values conflicts with declaration";

            throw AppException<std::invalid_argument>(stdExMsg, oss.str());
        }

        LookForCommonInconsistencies(argDecl, m_lastPositionIsTaken, stdExMsg);

        VerifyArgTypedValConfig(argDecl, argValCfg, stdExMsg);

        AddVerifiedArgSpecIntoMap(argDecl, argValCfg, stdExMsg);
    }

    /// <summary>
    /// Adds the declaration of an expected command line argument which has
    /// an accompanying floating point value that requires specification of
    /// default value, range or allowed set of values.
    /// </summary>
    /// <param name="argDecl">The argument declaration.</param>
    /// <param name="argValCfg">A list of configured values for the accompanying value.
    /// When such value is not required, the first item is the default value.
    /// When such value is limited to a range, the following 2 items are the min & max.
    /// When such value is limited to a set, the following N (> 1) items are enumerated.</param>
    void CommandLineArguments::AddExpectedArgument(const CommandLineArguments::ArgDeclaration &argDecl,
                                                   std::initializer_list<double> &&argValCfg)
    {
        CALL_STACK_TRACE;

        const char *stdExMsg("Failed to add declaration of expected command line argument");

        // Argument declaration conflicts with value configuration:
        if (static_cast<uint8_t> (argDecl.valueType) & static_cast<uint8_t> (ArgValType::Float) == 0)
        {
            std::ostringstream oss;
            oss << "Argument ID " << argDecl.id
                << ": configuration of floating point values conflicts with declaration";

            throw AppException<std::invalid_argument>(stdExMsg, oss.str());
        }

        LookForCommonInconsistencies(argDecl, m_lastPositionIsTaken, stdExMsg);

        VerifyArgTypedValConfig(argDecl, argValCfg, stdExMsg);

        AddVerifiedArgSpecIntoMap(argDecl, argValCfg, stdExMsg);
    }

    /// <summary>
    /// Adds the declaration of an expected command line argument which has
    /// an accompanying value of string type that requires specification of
    /// default value, range or allowed set of values.
    /// </summary>
    /// <param name="argDecl">The argument declaration.</param>
    /// <param name="argValCfg">A list of configured values for the accompanying value.
    /// When such value is not required, the first item is the default value.
    /// When such value is limited to a range, the following 2 items are the min & max.
    /// When such value is limited to a set, the following N (> 1) items are enumerated.</param>
    void CommandLineArguments::AddExpectedArgument(const CommandLineArguments::ArgDeclaration &argDecl,
                                                   std::initializer_list<const char *> &&argValCfg)
    {
        CALL_STACK_TRACE;

        const char *stdExMsg("Failed to add declaration of expected command line argument");

        // Argument declaration conflicts with value configuration:
        if (static_cast<uint8_t> (argDecl.valueType) & static_cast<uint8_t> (ArgValType::String) == 0)
        {
            std::ostringstream oss;
            oss << "Argument ID " << argDecl.id
                << ": configuration of string values conflicts with declaration";

            throw AppException<std::invalid_argument>(stdExMsg, oss.str());
        }

        LookForCommonInconsistencies(argDecl, m_lastPositionIsTaken, stdExMsg);

        VerifyArgTypedValConfig(argDecl, argValCfg, stdExMsg);

        AddVerifiedArgSpecIntoMap(argDecl, argValCfg, stdExMsg);
    }

    /// <summary>
    /// Finalizes an instance of the <see cref="CommandLineArguments"/> class.
    /// </summary>
    CommandLineArguments::~CommandLineArguments()
    {
        // Release resources from specs of expected arguments:
        for (auto &entry : m_expectedArgs)
        {
            auto &argDeclExt = entry.second;
            
            switch (argDeclExt.common.valueType)
            {
            case ArgValType::Integer:
            case ArgValType::RangeInteger:
            case ArgValType::EnumInteger:
                delete static_cast<std::initializer_list<long long> *> (argDeclExt.typedExtInfo);
                break;
            case ArgValType::Float:
            case ArgValType::RangeFloat:
                delete static_cast<std::initializer_list<double> *> (argDeclExt.typedExtInfo);
                break;
            case ArgValType::String:
            case ArgValType::EnumString:
                delete static_cast<std::initializer_list<const char *> *> (argDeclExt.typedExtInfo);
                break;
            default:
                break;
            }
        }
    }

    /// <summary>
    /// Formats the text contained in the provided string stream, placing it as
    /// a paragraph with a maximum given width (all lines) and advanced to the
    /// right by a given amount of spaces (from the second line onwards).
    /// </summary>
    /// <param name="oss">The string stream, which is supposed not to contain line breaks.</param>
    /// <param name="width">The paragraph width.</param>
    /// <param name="advance">The amount of spaces to advance to the right.</param>
    /// <param name="stdExMsg">The standard message for thrown exceptions.</param>
    /// <returns>A string containing the formatted paragraph.</returns>
    static string FormatParagraph(std::ostringstream &oss,
                                  uint16_t width,
                                  uint16_t advance,
                                  const char *stdExMsg)
    {
        CALL_STACK_TRACE;

        auto input = oss.str();
        oss.str("");

        // Get rid of leading and trailing white spaces:
        auto first = std::find_if(input.begin(), input.end(), [](char ch) { return !iswspace(ch); }); // left trim
        auto nChLTrim = std::distance(input.begin(), first); // amount of leading spaces
        auto rfirst = std::find_if(input.rbegin(), input.rend(), [](char ch){ return !iswspace(ch); }); // right trim
        auto nChRTrim = std::distance(input.rbegin(), rfirst); // amount of trailing spaces

        // only whitespaces?
        if (input.length() < nChLTrim + nChRTrim)
            return "";

        // length of trimmed string
        auto length = static_cast<size_t> (input.length() - nChLTrim - nChRTrim);
        
        // if all content fits into one line, do nothing:
        if (length <= width)
        {
            if (nChLTrim + nChRTrim == 0)
                return input; // nothing to trim
            else
                return string(input.begin() + nChLTrim, input.end() - nChLTrim); // trimmed string
        }
        
        /* Assuming that width is not too narrow and the words in the text are not too
           large, then this estimate for output buffer size should give some extra room
           to safely prevent writing memory out-of-bound. */
        auto outBufSize = static_cast<size_t> (2 * (1 + length / width) * (advance + width));
        
        /* Allocate memory already zeroed, so finishing the
           string with a null char will not be necessary */
        char *output = (char *)calloc(outBufSize, sizeof(char));

        if (output == nullptr)
        {
            std::ostringstream oss;
            oss << stdExMsg << ": failed to allocate memory";
            throw AppException<std::runtime_error>(oss.str());
        }

        try
        {
            auto outIter = output;
            auto inIter = input.data() + nChLTrim;

            // Break text into lines not bigger than paragraph width:
            do
            {
                bool whiteSpaceFound;

                /* search the line backwards to find a white space
                to break the line, but do not go past the middle: */
                uint16_t idx(width - 1);
                while (idx > width / 2)
                {
                    whiteSpaceFound = iswspace(*(inIter + idx));
                    if (!whiteSpaceFound)
                        --idx;
                    else
                        break;
                }

                /* if a white space could not be found,
                the line will break inside a word: */
                if (!whiteSpaceFound)
                    idx = width;

                // check for room before writing to output:
                if (outIter + idx >= output + outBufSize)
                {
                    std::ostringstream oss;
                    oss << stdExMsg << ": buffer is too short!";
                    throw AppException<std::logic_error>(oss.str());
                }

                strncpy(outIter, inIter, idx); // copy slice from input to output
                outIter += idx; // move output iterator past the slice
                *outIter = '\n'; // break the line
                ++outIter; // move output iterator past line break

                inIter += idx; // move input iterator past the slice
                length -= idx; // update remaining length

                // skip white space:
                if (whiteSpaceFound)
                {
                    ++inIter;
                    --length;
                }

                // check for room before writing to output:
                if (outIter + advance > output + outBufSize)
                {
                    std::ostringstream oss;
                    oss << stdExMsg << ": buffer is too short!";
                    throw AppException<std::logic_error>(oss.str());
                }

                // if any content remains, prepare the next line:
                if (length > 0)
                {
                    memset(outIter, ' ', advance); // pad with spaces to advance next line to the right
                    outIter += advance;
                }

            } while (length > width); // continue breaking lines?

            // copy the last line to output:
            if (length > 0)
            {
                // check for room before writing to output:
                if (outIter + length > output + outBufSize)
                {
                    std::ostringstream oss;
                    oss << stdExMsg << ": buffer is too short!";
                    throw AppException<std::logic_error>(oss.str());
                }

                strncpy(outIter, inIter, length);
            }

            string formattedParagraph(output);
            free(output);

            return formattedParagraph;
        }
        catch (IAppException &)
        {
            free(output);
            throw;
        }
    }

    /// <summary>
    /// Prints the usage.
    /// </summary>
    void CommandLineArguments::PrintUsage() const
    {
        CALL_STACK_TRACE;

        const char *spaceAdvLeftBorder(" "), // space advanced by left border of table column 1
                   *optCharSign(m_useOptSignSlash ? "/" : "-"), // sign for option with single char label
                   *commaBetweenLabels(", "), // comma placed between single char and name labels
                   *optNameSign(m_useOptSignSlash ? "/" : "--"), // sign for option with name label
                   *spaceBetweenCols("   "); // space between tables columns 1 & 2

        uint16_t widthTableCol1;

        // any name label is present?
        if (m_largestNameLabel > 0)
        {
            widthTableCol1 = static_cast<uint16_t> (
                strlen(spaceAdvLeftBorder) + 2
                + strlen(commaBetweenLabels)
                + strlen(optNameSign)
                + m_largestNameLabel
                + (m_argValSeparator == ArgValSeparator::Space) ? 0 : 2
            );
        }
        else
        {
            widthTableCol1 = static_cast<uint16_t> (
                strlen(spaceAdvLeftBorder) + 2
                + (m_argValSeparator == ArgValSeparator::Space) ? 0 : 1
            );
        }

        // amount of spaces a paragraph in col 2 is advanced to the right
        auto nSpacesAdvCol2 = static_cast<uint16_t> (widthTableCol1 + strlen(spaceBetweenCols));

        // width of table col 2, where the paragraphs must be formatted
        auto widthTableCol2 = static_cast<uint16_t> (m_minCmdLineWidth - nSpacesAdvCol2);

        for (auto &entry : m_expectedArgs)
        {
            auto &argDecl = entry.second.common;

            // is value an integer?
            if (static_cast<uint8_t> (argDecl.valueType) & static_cast<uint8_t> (ArgValType::Integer) == 0)
            {
                auto &argValCfg = *static_cast<std::initializer_list<long long> *> (entry.second.typedExtInfo);

            }
            // is value a floating point?
            else if (static_cast<uint8_t> (argDecl.valueType) & static_cast<uint8_t> (ArgValType::Float) == 0)
            {
                auto &argValCfg = *static_cast<std::initializer_list<double> *> (entry.second.typedExtInfo);

            }
            // is value a string?
            else if (static_cast<uint8_t> (argDecl.valueType) & static_cast<uint8_t> (ArgValType::String) == 0)
            {
                auto &argValCfg = *static_cast<std::initializer_list<const char *> *> (entry.second.typedExtInfo);
                
            }
            
        }// for loop end
    }

}// end of namespace core
}// end of namespace _3fd
