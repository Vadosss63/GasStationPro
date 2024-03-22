#pragma once

#include <QString>
#include <optional>

struct AzsButton
{
    int idAzs{};
    int value{};
    int button{};
};

std::optional<AzsButton> readAzsButton(const QString& jsonText);
