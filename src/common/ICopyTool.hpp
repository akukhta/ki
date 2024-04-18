#pragma once

/// An interface of a CopyTool
class ICopyTool
{
public:
    /// Performs the copying of the file
    virtual void copy() = 0;
    virtual ~ICopyTool() = default;
};