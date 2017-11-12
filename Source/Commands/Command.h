/*
  ==============================================================================

    Command.h
    Created: 5 Nov 2017 1:05:02pm
    Author:  srejv

  ==============================================================================
*/

#pragma once


struct Command
{
    virtual ~Command() {}

    virtual void execute() = 0;
    virtual void undo() = 0;
};
