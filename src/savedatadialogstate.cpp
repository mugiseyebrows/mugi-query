#include "savedatadialogstate.h"

#include "outputtype.h"
#include "dataformat.h"

SaveDataDialogState::SaveDataDialogState() {

}

SaveDataDialogState::SaveDataDialogState(const QString& outputType, const QString& dataFormat,
                                         const QString& outputDir)
    : mOutputType(outputType), mDataFormat(dataFormat), mOutputDir(outputDir) {
}

QString SaveDataDialogState::outputType() const {
    return mOutputType;
}

void SaveDataDialogState::setOutputType(const QString& outputType) {
    mOutputType = outputType;
}

void SaveDataDialogState::setOutputType(int type) {
    mOutputType = OutputType::name((OutputType::Type) type);
}

QString SaveDataDialogState::dataFormat() const {
    return mDataFormat;
}

void SaveDataDialogState::setDataFormat(const QString& dataFormat) {
    mDataFormat = dataFormat;
}

void SaveDataDialogState::setDataFormat(int format) {
    mDataFormat = DataFormat::name((DataFormat::Format) format);
}

QString SaveDataDialogState::outputDir() const {
    return mOutputDir;
}

void SaveDataDialogState::setOutputDir(const QString& outputDir) {
    mOutputDir = outputDir;
}

bool SaveDataDialogState::isValid() const {
    return !mOutputType.isEmpty();
}
