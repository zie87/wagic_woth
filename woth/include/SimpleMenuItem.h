#ifndef SIMPLEMENUITEM_H
#define SIMPLEMENUITEM_H

#include <string>
#include <JLBFont.h>
#include <JGui.h>
#include "SimpleButton.h"
#include "SimpleMenu.h"

using std::string;

#define SCALE_SELECTED 1.2f
#define SCALE_NORMAL 1.0f

class SimpleMenuItem : public SimpleButton {
private:
    std::string mDescription;

public:
    SimpleMenuItem(int id);
    SimpleMenuItem(SimpleMenu* _parent,
                   int id,
                   int fontId,
                   const std::string& text,
                   float x,
                   float y,
                   bool hasFocus      = false,
                   bool autoTranslate = false);

    void Entering() override;
    virtual void setDescription(const std::string& desc);
    virtual std::string getDescription() const;
    std::ostream& toString(std::ostream& out) const override;
};

#endif
