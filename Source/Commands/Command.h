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


#if 0
struct AddCommand : public Command
{
    AddComponent(Component &componentToAdd, Component& parent)
        : componentToAdd(componentToAdd)
        , parent(parent)
    {
    }
    
    void execute() override
    {
        parent.addAndMakeVisible(componentToAdd);
    }
    void undo() override {
        parent.removeChildComponent(componentToAdd);
    }
    
private:
    Component &componentToAdd;
    Component &parent;
};

struct RemoveCommand : public Command
{
    RemoveCommand(Component &componentToRemove, Component& parent)
        : componentToRemove(componentToRemove)
        , parent(parent)
    {
    }
    
    void execute() override
    {
        parent.removeChildComponent(componentToRemove);
    }
    void undo() override {
        parent.addAndMakeVisible(componentToRemove);
    }
    
private:
    Component &componentToRemove;
    Component &parent;
};



struct ChangeVolumeCommand : public Command {
    void execute() override {
        jassertfalse;
    }
    void undo() override {
        jassertfalse;
    }
};
struct SetFadeInCommand    : public Command {void execute() override {
        jassertfalse;
    }
    void undo() override {
        jassertfalse;
    }
};
struct SetFadeOutCommand   : public Command {void execute() override {
        jassertfalse;
    }
    void undo() override {
        jassertfalse;
    }
};

#endif