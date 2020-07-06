#pragma once

/// <summary>
/// An interface for a game scene
/// </summary>
class IScene
{
public:

    /// <summary>
    /// Handle inputs, collisions, and everything else that should'nt be in the DrawScene function
    /// </summary>
    /// <param name="deltaTime"> Time passed between frames </param>
    virtual void UpdateScene(float deltaTime) = 0;

    /// <summary>
    /// Draw the current scence
    /// </summary>
    virtual void DrawScene() = 0;

};