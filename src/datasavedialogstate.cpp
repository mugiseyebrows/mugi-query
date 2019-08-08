#include "datasavedialogstate.h"

DataSaveDialogState::DataSaveDialogState() {
}

DataSaveDialogState::DataSaveDialogState(const QString& outputType, const QString& dataFormat,
                                         const QString& outputDir)
    : mOutputType(outputType), mDataFormat(dataFormat), mOutputDir(outputDir) {
}

QString DataSaveDialogState::outputType() const {
    return mOutputType;
}

void DataSaveDialogState::setOutputType(const QString& outputType) {
    mOutputType = outputType;
}

QString DataSaveDialogState::dataFormat() const {
    return mDataFormat;
}

void DataSaveDialogState::setDataFormat(const QString& dataFormat) {
    mDataFormat = dataFormat;
}

QString DataSaveDialogState::outputDir() const {
    return mOutputDir;
}

void DataSaveDialogState::setOutputDir(const QString& outputDir) {
    mOutputDir = outputDir;
}

bool DataSaveDialogState::isValid() const {
    return !mOutputType.isEmpty();
}
