// David Eberly, Geometric Tools, Redmond WA 98052
// Copyright (c) 1998-2025
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt
// https://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
// File Version: 8.0.2025.05.10

#pragma once

#include <Applications/Window3.h>
#include <Graphics/DirectionalLightEffect.h>
#include <Graphics/GlossMapEffect.h>
using namespace gte;

class GlossMapsWindow3 : public Window3
{
public:
    GlossMapsWindow3(Parameters& parameters);

    virtual void OnIdle();

private:
    bool SetEnvironment();
    void CreateScene();
    void UpdateConstants();

    std::shared_ptr<Node> mScene;
    std::shared_ptr<Visual> mSquareNoGloss, mSquareGloss;
    std::shared_ptr<DirectionalLightEffect> mDLEffect;
    std::shared_ptr<GlossMapEffect> mGMEffect;
    Vector4<float> mLightWorldDirection;
};

