#include "action.h"

Action::Action(Action::Type type, const QVariantList &args) : mType(type), mArgs(args) {

}

Action::Type Action::type() const {
    return mType;
}

QVariantList Action::args() const {
    return mArgs;
}

QVariant Action::arg(int index) const {
    return mArgs.value(index);
}

QList<Function> Action::functions() const
{
    return mFunctions;
}
