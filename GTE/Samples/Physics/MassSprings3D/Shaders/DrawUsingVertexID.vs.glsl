// David Eberly, Geometric Tools, Redmond WA 98052
// Copyright (c) 1998-2025
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt
// https://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
// File Version: 8.0.2025.05.10

uniform PVWMatrix
{
    mat4 pvwMatrix;
};

uniform ConstantColor
{
    vec4 constantColor;
};

buffer position { vec4 data[]; } positionSB;

out vec4 vertexColor;

void main()
{
    vec4 modelPosition = positionSB.data[gl_VertexID];
#if GTE_USE_MAT_VEC
    gl_Position = pvwMatrix * modelPosition;
#else
    gl_Position = modelPosition * pvwMatrix;
#endif
    vertexColor = constantColor;
}

