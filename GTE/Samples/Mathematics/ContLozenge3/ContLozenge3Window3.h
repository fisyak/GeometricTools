// David Eberly, Geometric Tools, Redmond WA 98052
// Copyright (c) 1998-2026
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt
// https://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
// File Version: 8.0.2026.09.12

#pragma once

#include <Applications/Window3.h>
#include <Graphics/MeshFactory.h>
#include <Mathematics/ContLozenge3.h>
using namespace gte;

class ContLozenge3Window3 : public Window3
{
public:
    ContLozenge3Window3(Parameters& parameters);

    virtual bool OnCharPress(uint8_t key, int32_t x, int32_t y) override;
    virtual void OnIdle() override;

private:
    void CreateScene();
    void CreatePoints(std::vector<Vector3<float>> const& points);
    void CreateFaces();
    void CreateCylinders();
    void CreateSpheres();

    VertexFormat mVFormat;
    MeshFactory mMeshFactory;
    Lozenge3<float> mLozenge;
    Vector3<float> mPerpendicular;

    std::shared_ptr<RasterizerState> mNoCullWireframeState;
    std::vector<std::shared_ptr<Visual>> mPoints;
    std::array<std::shared_ptr<Visual>, 2> mFaces;
    std::array<std::shared_ptr<Visual>, 4> mCylinders;
    std::array<std::shared_ptr<Visual>, 4> mSpheres;
    bool mDrawPoints;
    bool mDrawFaces;
    bool mDrawCylinders;
    bool mDrawSpheres;
};
