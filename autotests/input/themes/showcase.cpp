/**
 * SPDX-FileCopyrightText: 2020 Christoph Cullmann <cullmann@kde.org>
 * SPDX-License-Identifier: MIT
 */

// BEGIN
#include <string>
#include <QString>
// END

/**
 * TODO: improve documentation
 * @param magicArgument some magic argument
 * @return magic return value
 */
int main(uint64_t magicArgument)
{
    if (magicArgument > 1) {
        const std::string string = "source file: \"" __FILE__ "\"";
        const QString qString(QStringLiteral("test"));
        return qrand();
    }

    /* BUG: bogus integer constant inside next line */
    const double g = 1.1e12 * 0b01'01'01'01 - 43a + 0x11234 * 0234ULL - 'c' * 42;
    return g > 1.3f;
}
