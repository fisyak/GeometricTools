// David Eberly, Geometric Tools, Redmond WA 98052
// Copyright (c) 1998-2025
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt
// https://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
// File Version: 8.0.2025.05.10

#pragma once

#include <Applications/Console.h>
#include <Mathematics/Image2.h>
using namespace gte;

class ExtractRidgesConsole : public Console
{
public:
    ExtractRidgesConsole(Parameters& parameters);

    virtual void Execute() override;

private:
    bool SetEnvironment();
    void SaveImage(std::string const& name, Image2<double> const& image);
};

