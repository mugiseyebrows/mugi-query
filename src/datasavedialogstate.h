#ifndef DATASAVEDIALOGSTATE_H
#define DATASAVEDIALOGSTATE_H
#include <QString>

class DataSaveDialogState {
public:
    DataSaveDialogState();
    DataSaveDialogState(const QString& outputType, const QString& dataFormat,
                        const QString& outputDir);
    QString outputType() const;
    void setOutputType(const QString& outputType);
    QString dataFormat() const;
    void setDataFormat(const QString& dataFormat);
    QString outputDir() const;
    void setOutputDir(const QString& outputDir);
    bool isValid() const;

protected:
    QString mOutputType;
    QString mDataFormat;
    QString mOutputDir;
};
#endif // DATASAVEDIALOGSTATE_H
